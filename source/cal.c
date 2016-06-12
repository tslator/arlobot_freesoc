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

#define MAX_CPS_SAMPLE (2147483647)
#define MIN_CPS_SAMPLE (-2147483648)

#define CAL_DATA_ROW_START (16)
#define CAL_DATA_CPS_SAMPLES_SIZE (CAL_NUM_SAMPLES * sizeof(int32))
#define CAL_DATA_PWM_SAMPLES_SIZE (CAL_NUM_SAMPLES * sizeof(uint16))
#define BYTES_IN_ROW (16)
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
    *min = MAX_CPS_SAMPLE;
    *max = MIN_CPS_SAMPLE;
    
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
    EEPROM_WriteByte(0x00, (uint16) (&p_eeprom->status));
    I2c_ClearStatusBit(STATUS_CALIBRATED_BIT);
    I2c_SetStatusBit(STATUS_CALIBRATING_BIT);
    Ser_PutString("Starting calibration ...\r\n");
}

static void EndCalibration(uint8 success)
{
    if (success)
    {
        EEPROM_WriteByte(0x01, (uint16) (&p_eeprom->status));
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

void Cal_LeftGetCalData(CAL_DATA_TYPE *fwd_cal_data, CAL_DATA_TYPE *bwd_cal_data)
{
    fwd_cal_data = (CAL_DATA_TYPE *) &(p_eeprom->left_motor_fwd);
    bwd_cal_data = (CAL_DATA_TYPE *) &(p_eeprom->left_motor_bwd);
}

void Cal_RightGetCalData(CAL_DATA_TYPE *fwd_cal_data, CAL_DATA_TYPE *bwd_cal_data)
{
    fwd_cal_data = (CAL_DATA_TYPE *) &(p_eeprom->right_motor_fwd);
    bwd_cal_data = (CAL_DATA_TYPE *) &(p_eeprom->right_motor_bwd);
}

void Cal_Validate()
{
    /* Write to the serial port that we are validating calibration: VALIDATING CALIBRATION !!! */
    
    //Motor_ValidateCalibration();
    
    /* Write to the serial port that we are done: VALIDATION COMPLETE !!! */
}

/* [] END OF FILE */
