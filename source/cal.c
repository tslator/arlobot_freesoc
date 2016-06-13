/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <stdio.h>
#include "cal.h"
#include "motor.h"
#include "i2c.h"
#include "pwm.h"
#include "encoder.h"
#include "utils.h"
#include "serial.h"

/*
    Explantation of Calibration:

    Calibration involves mapping count/sec values to PWM values.  Each motor is run (forward and backward) collecting
    an average count/sec for each PWM sample.  The results are stored in an structure (CAL_DATA_TYPE) that holds the
    count/sec averages, PWM samples, min/max count/sec and a scale factor.  There are four CAL_DATA_TYPE instances: one
    for left motor forward, left motor backward, right motor forward and right motor backward.  The calibration data is
    stored in NVRAM on the Psoc and pointers to the calibration data are passed to the motor module.
    
    Upon startup, the calibration data is made available to the motors via pointers to NVRAM (Note: the EEPROM component
    maps NVRAM to memory so there is no appreciable overhead in reading from NVRAM (or so I believe until proven otherwise)

 */

#define MAX_CPS_SAMPLE (65535)
#define MIN_CPS_SAMPLE (-65535)

#define CAL_DATA_ROW_START (16)
#define CAL_DATA_CPS_SAMPLES_SIZE (CAL_NUM_SAMPLES * sizeof(int32))
#define CAL_DATA_PWM_SAMPLES_SIZE (CAL_NUM_SAMPLES * sizeof(uint16))
#define BYTES_IN_ROW (CY_EEPROM_SIZEOF_ROW)
#define CAL_DATA_SIZE_IN_ROWS ( 1 + ((2 * sizeof(int32) + 1 * sizeof(int) + 1 * CAL_DATA_CPS_SAMPLES_SIZE  + 1 * CAL_DATA_PWM_SAMPLES_SIZE) / BYTES_IN_ROW))

#define CAL_DATA_LEFT_FWD_OFFSET    (CAL_DATA_ROW_START)
#define CAL_DATA_LEFT_BWD_OFFSET    (CAL_DATA_ROW_START + (1*CAL_DATA_SIZE_IN_ROWS))
#define CAL_DATA_RIGHT_FWD_OFFSET   (CAL_DATA_ROW_START + (2*CAL_DATA_SIZE_IN_ROWS))
#define CAL_DATA_RIGHT_BWD_OFFSET   (CAL_DATA_ROW_START + (3*CAL_DATA_SIZE_IN_ROWS))

typedef struct _eeprom_tag
{
    uint16 status;
    uint16 checksum;
    uint8  reserved[12];
    CAL_DATA_TYPE left_motor_fwd;
    CAL_DATA_TYPE left_motor_bwd;
    CAL_DATA_TYPE right_motor_fwd;
    CAL_DATA_TYPE right_motor_bwd;
} __attribute__ ((packed)) EEPROM_TYPE;

static volatile EEPROM_TYPE *p_eeprom;

static uint16 CAL_DATA_ROW_OFFSET[2][2] = { {CAL_DATA_LEFT_FWD_OFFSET, CAL_DATA_LEFT_BWD_OFFSET}, {CAL_DATA_RIGHT_FWD_OFFSET, CAL_DATA_RIGHT_BWD_OFFSET}};

static int32 cps_samples[CAL_NUM_SAMPLES];
static uint16 pwm_samples[CAL_NUM_SAMPLES];
static int32 cps_sum[CAL_NUM_SAMPLES];
static CAL_DATA_TYPE cal_data;


static void CalculatePwmSamples(WHEEL_TYPE wheel, DIR_TYPE dir, uint16 *pwm_samples, uint8 *reverse_pwm)
{
    /*
        if wheel is left
            if dir is forward
                pwm start = 1500
                pwm end = 2000
                pwm step = (2000 - 1500) / num_samples
            if dir is backward
                pwm start = 1000
                pwm end = 1500
                pwm step = (1500 - 1000) / num_samples
    
        if wheel is right
            if dir is forward
                pwm start = 1000
                pwm end = 1500
                pwm step = (1500 - 1000) / num_samples
            if dir is backward
                pwm start = 1500
                pwm end = 2000
                pwm step = (2000 - 1500) / num_samples
    */
    
    uint16 pwm;
    uint16 pwm_start;
    uint16 pwm_end;
    uint16 pwm_step;
    
    if (wheel == LEFT_WHEEL)
    {
        if (dir == FORWARD_DIR)
        {
            pwm_start = LEFT_PWM_STOP;
            pwm_end = LEFT_PWM_FULL_FORWARD;
            pwm_step = LEFT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1);
            *reverse_pwm = 0;
        }
        else
        {
            pwm_start = LEFT_PWM_FULL_BACKWARD;
            pwm_end = LEFT_PWM_STOP;
            pwm_step = LEFT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1);
            *reverse_pwm = 1;
        }

        uint8 ii;
        for (ii = 0, pwm = pwm_start; pwm <= pwm_end; ++ii, pwm += pwm_step)
        {
            pwm_samples[ii] = pwm;
        }
    }
    else
    {
        if (dir == FORWARD_DIR)
        {
            pwm_start = RIGHT_PWM_STOP;
            pwm_end = RIGHT_PWM_FULL_FORWARD;
            pwm_step = RIGHT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1);
            *reverse_pwm = 0;
        }
        else
        {
            pwm_start = RIGHT_PWM_FULL_BACKWARD;
            pwm_end = RIGHT_PWM_STOP;
            pwm_step = RIGHT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1);
            *reverse_pwm = 1;
        }
        uint8 ii;
        for (ii = 0, pwm = pwm_start; pwm >= pwm_end; ++ii, pwm -= pwm_step)
        {
            pwm_samples[ii] = pwm;
        }
        
    }
}

static void AddSamplesToSum(int32 *cps_samples, int32 *cps_sum)
{
    uint8 ii;
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        cps_sum[ii] += cps_samples[ii];
    }
}
    
static void CalculateAvgCpsSamples(int32 *cps_sum, uint8 num_runs, int32 *avg_cps_samples)
{
    uint8 ii;
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        avg_cps_samples[ii] = cps_sum[ii]/num_runs;
    }
}

static void CalculateMinMaxCpsSample(int32 *samples, uint8 num_samples, int32 *min, int32 *max)
{
    uint8 ii;
    *min = 65535;
    *max = -65535;
    
    for (ii = 0; ii < num_samples; ++ii)
    {
        if (samples[ii] > *max)
        {
            *max = samples[ii];
        }
        if (samples[ii] < *min)
        {
            *min = samples[ii];
        }
    }
}

static void CalibrateWheelSpeed(WHEEL_TYPE wheel, DIR_TYPE dir, uint8 num_runs, int32 *cps_samples, uint16 *pwm_samples)
/*
    calculate the PWM samples
        Note: the pwm samples depend on the wheel and direction
    for the number of runs
        collect cps-to-pwm samples
        add the samples to the sum (for averaging)

    calculate the average cps for each pwm
*/
{
    uint8 run;
    uint8 num_avg_iter = 10;
    uint8 reverse_pwm;
    
    
    CalculatePwmSamples(wheel, dir, pwm_samples, &reverse_pwm);

    memset(cps_sum, 0, CAL_NUM_SAMPLES * sizeof(int32));
    
    for (run = 0; run < num_runs; ++run)
    {
        Motor_CollectPwmCpsSamples(wheel, reverse_pwm, num_avg_iter, pwm_samples, cps_samples);
        AddSamplesToSum(cps_samples, cps_sum);
    }
    
    CalculateAvgCpsSamples(cps_sum, num_runs, cps_samples);
}

static void StoreWheelSpeed(WHEEL_TYPE wheel, DIR_TYPE dir, int32 *cps_samples, uint16 *pwm_samples)
{
    /* Create a CAL_DATA_TYPE and store the values into EEPROM at the appropriate offset 
    
        cystatus EEPROM_Write(const uint8 *rowData, uint8 rowNumber)
        cystatus EEPROM_WriteByte(uint8 dataByte, uint16 address)

                    -----------------
                    |    status 1   | 0
                    |    status 0   | 1
                    |   checksum 0  | 2
                    |   checksum 1  | 3
                    |    reserved   | 4
        left fwd    |     min (0:3) | 16
                    |     max (0:3) | 20
                    |    scale (0:3)| 24
                    |    cps(0:203) | 228
                    |    pwm(0:101) | 330
                    |    reserved   | 334
        left bwd    |     min (0:3) | 336
                    |     max (0:3) | 340
                    |    scale (0:3)| 344
                    |    cps(0:203) | 348
                    |    pwm(0:101) | 552
                    |    reserved   | 654
        right fwd   |     min (0:3) | 656
                    |     max (0:3) | 660
                    |    scale (0:3)| 664
                    |    cps(0:203) | 668
                    |    pwm(0:101) | 872
                    |    reserved   | 974
        right bwd   |     min (0:3) | 976
                    |     max (0:3) | 980
                    |    scale (0:3)| 984
                    |    cps(0:203) | 988
                    |    pwm(0:101) | 1192
                    |    reserved   | 1294
                    |               | 1296
    
    
    */
    
    uint8 ii;
    uint8 eeprom_row[16];
    uint16 row_start = CAL_DATA_ROW_OFFSET[wheel][dir];    

    CalculateMinMaxCpsSample(cps_samples, CAL_NUM_SAMPLES, &cal_data.cps_min, &cal_data.cps_max);
    cal_data.cps_scale = CAL_SCALE_FACTOR;
    memcpy(cal_data.cps_data, cps_samples, CAL_NUM_SAMPLES);
    memcpy(cal_data.pwm_data, pwm_samples, CAL_NUM_SAMPLES);
    
    for ( ii = 0; ii < CAL_DATA_SIZE_IN_ROWS; ++ii)
    {
        memset(eeprom_row, 0, BYTES_IN_ROW);
        memcpy((void *) eeprom_row, (void *)((&cal_data) + (ii * CAL_DATA_SIZE_IN_ROWS)), CAL_DATA_SIZE_IN_ROWS);
        EEPROM_Write(eeprom_row, row_start + ii);
    }
}

void Cal_Init()
{
    p_eeprom = (volatile EEPROM_TYPE *) CYDEV_EE_BASE;
}


void Cal_Start()
{
    EEPROM_Start();
    
    /* 
        Set the calibration status as un-calibrated
        Check for calibration data stored in EEPROM (may need a status word at the start of EEPROM to indicate
        the contents and validity of contents in EEPROM, in case, there is the need to store other data, future
        capability support.)
        If calibration data not available or not valid, 
            Indicate uncalibrated state
        If calibration data available and valid, then
            The EEPROM is mirrored in memory so we need only to set the pointer to the proper address
            CYDEV_EE_BASE is the base address in the memory
            Calibration data will be at some offset from CYDEV_EE_BASE
            Indicate calibrated state
    */
    
    if (p_eeprom->status & STATUS_CALIBRATED_BIT)
    {
        /* Calculate the check to validate the data 
           if data is not valid then
                clear the calibrated bit        
        */
        
        I2c_SetStatusBit(STATUS_CALIBRATED_BIT);
    }
    else
    {
        I2c_ClearStatusBit(STATUS_CALIBRATED_BIT);
    }
}

static void StartCalibration()
{
    EEPROM_WriteByte(0x00, 0);
    I2c_ClearStatusBit(STATUS_CALIBRATED_BIT);
    I2c_SetStatusBit(STATUS_CALIBRATING_BIT);
    Ser_PutString("Starting calibration ...\r\n");
}

static void EndCalibration(uint8 success)
{
    if (success)
    {
        EEPROM_WriteByte(0x01, 0);
        I2c_SetStatusBit(STATUS_CALIBRATED_BIT);
        Ser_PutString("Calibration successful\r\n");
    }
    else
    {
        Ser_PutString("Calibration failed\r\n");
    }
    
    I2c_ClearStatusBit(STATUS_CALIBRATING_BIT);
}

void Cal_Update()
{   
    StartCalibration();
    
    CalibrateWheelSpeed(LEFT_WHEEL, FORWARD_DIR, 5, cps_samples, pwm_samples);
    StoreWheelSpeed(LEFT_WHEEL, FORWARD_DIR, cps_samples, pwm_samples);

    CalibrateWheelSpeed(LEFT_WHEEL, BACKWARD_DIR, 5, cps_samples, pwm_samples);
    StoreWheelSpeed(LEFT_WHEEL, BACKWARD_DIR, cps_samples, pwm_samples);

    CalibrateWheelSpeed(RIGHT_WHEEL, FORWARD_DIR, 5, cps_samples, pwm_samples);
    StoreWheelSpeed(RIGHT_WHEEL, FORWARD_DIR, cps_samples, pwm_samples);

    CalibrateWheelSpeed(RIGHT_WHEEL, BACKWARD_DIR, 5, cps_samples, pwm_samples);
    StoreWheelSpeed(RIGHT_WHEEL, BACKWARD_DIR, cps_samples, pwm_samples);

    Motor_LeftSetCalibration((CAL_DATA_TYPE *) &p_eeprom->left_motor_fwd, (CAL_DATA_TYPE *) &p_eeprom->left_motor_bwd);
    Motor_RightSetCalibration((CAL_DATA_TYPE *) &p_eeprom->right_motor_fwd, (CAL_DATA_TYPE *) &p_eeprom->right_motor_bwd);
    
    EndCalibration(1);
}

static CAL_DATA_TYPE left_fwd_cal_data = {
    0,
    28006,
    CAL_SCALE_FACTOR,
    { /*  1     2     3     4     5     6      7    8     9     10  */
            0,    0,    0,   95,  605, 1270, 1922, 2580, 3251, 3917,
         4504, 5040, 5629, 6395, 7064, 7591, 8220, 8890, 9404,10080,
        10736,11326,11969,12539,13161,13767,14377,14961,15590,16126,
        16711,17317,17854,18400,18915,19465,19998,20534,21071,21623,
        22157,22660,23174,23684,24175,24667,25173,26212,27496,27940,
        28006
    },
    {1500, 1510, 1520, 1530, 1540, 1550, 1560, 1570, 1580, 1590, 
     1600, 1610, 1620, 1630, 1640, 1650, 1660, 1670, 1680, 1690, 
     1700, 1710, 1720, 1730, 1740, 1750, 1760, 1770, 1780, 1790,  
     1800, 1810, 1820, 1830, 1840, 1850, 1860, 1870, 1880, 1890, 
     1900, 1910, 1920, 1930, 1940, 1950, 1960, 1970, 1980, 1990, 2000
    },
    0
};

static CAL_DATA_TYPE left_bwd_cal_data = {
    -28447,
    0,
    CAL_SCALE_FACTOR,
    { /*   1      2      3      4      5      6      7      8      9     10  */
        -28447,-28021,-26732,-25679,-25219,-24723,-24232,-23750,-23327,-22774,
        -22266,-21779,-21238,-20692,-20130,-19573,-19003,-18434,-17850,-17264,
        -16676,-16057,-15457,-14850,-14243,-13631,-12968,-12352,-11705,-11079,
        -10446, -9808, -9189, -8516, -7862, -7279, -6610, -5966, -5317, -4671,
         -4036, -3417, -2779, -2161, -1558,  -893,  -236,     0,     0,     0,
        0
    },
    {1000,1010,1020,1030,1040,1050,1060,1070,1080,1090,
     1100,1110,1120,1130,1140,1150,1160,1170,1180,1190,
     1200,1210,1220,1230,1240,1250,1260,1270,1280,1290,
     1300,1310,1320,1330,1340,1350,1360,1370,1380,1390,
     1400,1410,1420,1430,1440,1450,1460,1470,1480,1490,1500
    },
    0
};


static CAL_DATA_TYPE right_fwd_cal_data = {
    0,
    26523,
    CAL_SCALE_FACTOR,
    { /*  1     2     3     4     5     6     7     8     9     10  */
            0,    0,    0,    0,    0,  132,  679, 1344, 1990, 2558,
         3107, 3663, 4263, 4926, 5625, 6195, 6708, 7346, 8051, 8567,
        9202,  9828,10375,11009,11539,12171,12715,13365,13903,14546,
        15075,15691,16247,16783,17358,17884,18379,18873,19401,19936,
        20404,20881,21362,21848,22327,22795,23270,23744,24264,25274,
        26523
    },
    {            
        1500,1490,1480,1470,1460,1450,1440,1430,1420,1410,
        1400,1390,1380,1370,1360,1350,1340,1330,1320,1310,
        1300,1290,1280,1270,1260,1250,1240,1230,1220,1210,
        1200,1190,1180,1170,1160,1150,1140,1130,1120,1110,
        1100,1090,1080,1070,1060,1050,1040,1030,1020,1010,
        1000    
    },
    0
};

    
static CAL_DATA_TYPE right_bwd_cal_data = {
    -27555,
    0,
    CAL_SCALE_FACTOR,
    { /*   1      2      3      4      5      6      7      8      9     10  */
        -27555,-27538,-27503,-27368,-26971,-25685,-24599,-24093,-23610,-23161,
        -22669,-22213,-21733,-21267,-20755,-20240,-19714,-19212,-18693,-18131,
        -17532,-16966,-16408,-15839,-15274,-14694,-14105,-13469,-12874,-12271,
        -11665,-11039,-10445, -9817, -9231, -8592, -7984, -7384, -6771, -6106,
         -5472, -4875, -4233, -3585, -2977, -2337, -1691, -1025,  -333,   -27,
        0
    },
    {
        2000,1990,1980,1970,1960,1950,1940,1930,1920,1910,
        1900,1890,1880,1870,1860,1850,1840,1830,1820,1810,
        1800,1790,1780,1770,1760,1750,1740,1730,1720,1710,
        1700,1690,1680,1670,1660,1650,1640,1630,1620,1610,
        1600,1590,1580,1570,1560,1550,1540,1530,1520,1510,
        1500
    },
    0
};    

void Cal_LeftGetCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data)
{
    //fwd_cal_data = (CAL_DATA_TYPE *) &(p_eeprom->left_motor_fwd);
    *fwd_cal_data = &left_fwd_cal_data;
    //bwd_cal_data = (CAL_DATA_TYPE *) &(p_eeprom->left_motor_bwd);
    *bwd_cal_data = &left_bwd_cal_data;
}

void Cal_RightGetCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data)
{
    //fwd_cal_data = (CAL_DATA_TYPE *) &(p_eeprom->right_motor_fwd);
    *fwd_cal_data = &right_fwd_cal_data;
    //bwd_cal_data = (CAL_DATA_TYPE *) &(p_eeprom->right_motor_bwd);
    *bwd_cal_data = &right_bwd_cal_data;
}

void Cal_Validate()
{
    /* Write to the serial port that we are validating calibration: VALIDATING CALIBRATION !!! */
    
    //Motor_ValidateCalibration();
    
    /* Write to the serial port that we are done: VALIDATION COMPLETE !!! */
}

/* [] END OF FILE */
