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
#include "motor.h"
#include "config.h"
#include "utils.h"
#include "encoder.h"
#include "time.h"
#include <math.h>
#include "utils.h"
#include "pwm.h"
#include "cal.h"
#include "debug.h"

#ifdef MOTOR_DUMP_ENABLED
#define MOTOR_DUMP(motor)   DumpMotor(motor)
#else    
#define MOTOR_DUMP(motor)
#endif

#define CAL_DATA_SIZE (CAL_NUM_SAMPLES)

/* Add a link to the HB25 data sheet
   Ref: https://www.parallax.com/sites/default/files/downloads/HB-25-Motor-Controller-Guide-v1.3.pdf
 */

#define HB25_ENABLE     (0)
#define HB25_DISABLE    (1)


typedef void (*HB25_ENABLE_TYPE)(uint8);
typedef void (*START_PWM_TYPE)();
typedef void (*SET_PWM_TYPE)(uint16);
typedef void (*STOP_PWM_TYPE)();

typedef struct _motor_tag
{
    char name[6];
    uint16 pwm;
    float cps;
    CAL_DATA_TYPE *p_fwd_cal_data;
    CAL_DATA_TYPE *p_bwd_cal_data;
    HB25_ENABLE_TYPE enable;
    START_PWM_TYPE   start;
    STOP_PWM_TYPE    stop;
    SET_PWM_TYPE     set_pwm;
} MOTOR_TYPE;

/*
Left

Forward CPS
0,0,0,95,605,1270,1922,2580,3251,3917,4504,
5040,5629,6395,7064,7591,8220,8890,9404,10080,10736,
11326,11969,12539,13161,13767,14377,14961,15590,16126,16711,
17317,17854,18400,18915,19465,19998,20534,21071,21623,22157,
22660,23174,23684,24175,24667,25173,26212,27496,27940,28006
Forward PWM
1500,1510,1520,1530,1540,1550,1560,1570,1580,1590,1600,
1610,1620,1630,1640,1650,1660,1670,1680,1690,1700,
1710,1720,1730,1740,1750,1760,1770,1780,1790,1800,
1810,1820,1830,1840,1850,1860,1870,1880,1890,1900,
1910,1920,1930,1940,1950,1960,1970,1980,1990,2000

Backward CPS
-28447,-28021,-26732,-25679,-25219,-24723,-24232,-23750,-23327,-22774,-22266,
-21779,-21238,-20692,-20130,-19573,-19003,-18434,-17850,-17264,-16676,
-16057,-15457,-14850,-14243,-13631,-12968,-12352,-11705,-11079,-10446,
-9808,-9189,-8516,-7862,-7279,-6610,-5966,-5317,-4671,-4036,
-3417,-2779,-2161,-1558,-893,-236,0,0,0,0
Backward PWM
1000,1010,1020,1030,1040,1050,1060,1070,1080,1090,1100,
1110,1120,1130,1140,1150,1160,1170,1180,1190,1200,
1210,1220,1230,1240,1250,1260,1270,1280,1290,1300,
1310,1320,1330,1340,1350,1360,1370,1380,1390,1400,
1410,1420,1430,1440,1450,1460,1470,1480,1490,1500
*/
static MOTOR_TYPE left_motor = {
    "left",
    LEFT_PWM_STOP,
    0,
    0,
    0,
#ifdef OLD        
    {
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
         1900, 1910, 1920, 1930, 1940, 1950, 1960, 1970, 1980, 1990, 2000}
    },
    {
        { /*   1      2      3      4      5      6      7      8      9     10  */
            -28447,-28021,-26732,-25679,-25219,-24723,-24232,-23750,-23327,-22774,
            -22266,-21779,-21238,-20692,-20130,-19573,-19003,-18434,-17850,-17264,
            -16676,-16057,-15457,-14850,-14243,-13631,-12968,-12352,-11705,-11079,
            -10446, -9808, -9189, -8516, -7862, -7279, -6610, -5966, -5317, -4671,
             -4036, -3417, -2779, -2161, -1558,  -893,  -236,     0,     0,     0,
            0
        },
        -28447,
        0,
        CAL_SCALE_FACTOR,
        {1000,1010,1020,1030,1040,1050,1060,1070,1080,1090,
         1100,1110,1120,1130,1140,1150,1160,1170,1180,1190,
         1200,1210,1220,1230,1240,1250,1260,1270,1280,1290,
         1300,1310,1320,1330,1340,1350,1360,1370,1380,1390,
         1400,1410,1420,1430,1440,1450,1460,1470,1480,1490,1500}
    },
#endif
    Left_HB25_Enable_Pin_Write,
    Left_HB25_PWM_Start,
    Left_HB25_PWM_Stop,
    Left_HB25_PWM_WriteCompare
};

/*
Right

Forward CPS
0,0,0,0,0,132,679,1344,1990,2558,3107,
3663,4263,4926,5625,6195,6708,7346,8051,8567,9202,
9828,10375,11009,11539,12171,12715,13365,13903,14546,15075,
15691,16247,16783,17358,17884,18379,18873,19401,19936,20404,
20881,21362,21848,22327,22795,23270,23744,24264,25274,26523

Forward PWM
1500,1490,1480,1470,1460,1450,1440,1430,1420,1410,1400,
1390,1380,1370,1360,1350,1340,1330,1320,1310,1300,
1290,1280,1270,1260,1250,1240,1230,1220,1210,1200,
1190,1180,1170,1160,1150,1140,1130,1120,1110,1100,
1090,1080,1070,1060,1050,1040,1030,1020,1010,1000

Backward CPS
-27555,-27538,-27503,-27368,-26971,-25685,-24599,-24093,-23610,-23161,-22669,
-22213,-21733,-21267,-20755,-20240,-19714,-19212,-18693,-18131,-17532,
-16966,-16408,-15839,-15274,-14694,-14105,-13469,-12874,-12271,-11665,
-11039,-10445,-9817,-9231,-8592,-7984,-7384,-6771,-6106,-5472,
-4875,-4233,-3585,-2977,-2337,-1691,-1025,-333,-27,0

Backward PWM
2000,1990,1980,1970,1960,1950,1940,1930,1920,1910,1900,
1890,1880,1870,1860,1850,1840,1830,1820,1810,1800,
1790,1780,1770,1760,1750,1740,1730,1720,1710,1700,
1690,1680,1670,1660,1650,1640,1630,1620,1610,1600,
1590,1580,1570,1560,1550,1540,1530,1520,1510,1500
*/

static MOTOR_TYPE right_motor = {
    "right",
    RIGHT_PWM_STOP,
    0,
    0,
    0,
#ifdef OLD        
    {
        { /*  1     2     3     4     5     6     7     8     9     10  */
                0,    0,    0,    0,    0,  132,  679, 1344, 1990, 2558,
             3107, 3663, 4263, 4926, 5625, 6195, 6708, 7346, 8051, 8567,
            9202,  9828,10375,11009,11539,12171,12715,13365,13903,14546,
            15075,15691,16247,16783,17358,17884,18379,18873,19401,19936,
            20404,20881,21362,21848,22327,22795,23270,23744,24264,25274,
            26523
        },
        0,
        26523,
        CAL_SCALE_FACTOR,
        {            
            1500,1490,1480,1470,1460,1450,1440,1430,1420,1410,
            1400,1390,1380,1370,1360,1350,1340,1330,1320,1310,
            1300,1290,1280,1270,1260,1250,1240,1230,1220,1210,
            1200,1190,1180,1170,1160,1150,1140,1130,1120,1110,
            1100,1090,1080,1070,1060,1050,1040,1030,1020,1010,
            1000    
        }
    },
    {
        { /*   1      2      3      4      5      6      7      8      9     10  */
            -27555,-27538,-27503,-27368,-26971,-25685,-24599,-24093,-23610,-23161,
            -22669,-22213,-21733,-21267,-20755,-20240,-19714,-19212,-18693,-18131,
            -17532,-16966,-16408,-15839,-15274,-14694,-14105,-13469,-12874,-12271,
            -11665,-11039,-10445, -9817, -9231, -8592, -7984, -7384, -6771, -6106,
             -5472, -4875, -4233, -3585, -2977, -2337, -1691, -1025,  -333,   -27,
            0
        },
        -27555,
        0,
        CAL_SCALE_FACTOR,
        {
            2000,1990,1980,1970,1960,1950,1940,1930,1920,1910,
            1900,1890,1880,1870,1860,1850,1840,1830,1820,1810,
            1800,1790,1780,1770,1760,1750,1740,1730,1720,1710,
            1700,1690,1680,1670,1660,1650,1640,1630,1620,1610,
            1600,1590,1580,1570,1560,1550,1540,1530,1520,1510,
            1500
        }
    },
#endif    
    Right_HB25_Enable_Pin_Write,
    Right_HB25_PWM_Start,
    Right_HB25_PWM_Stop,
    Right_HB25_PWM_WriteCompare
};

#ifdef MOTOR_DUMP_ENABLED
static void DumpMotor(MOTOR_TYPE *motor)
{
    char cps_str[10];
    
    ftoa(motor->cps, cps_str, 3);
    
    DEBUG_PRINT_STR("%s: %s %d \r\n", motor->name, cps_str, motor->pwm);
}
#endif

static int16 Interpolate(int16 tgt_cps, int16 cps1, int16 cps2, uint16 pwm1, uint16 pwm2)
{
    /* Y = ( ( X - X1 )( Y2 - Y1) / ( X2 - X1) ) + Y1

        tgt_cps => X
           cps1 => X1
           cps2 => X2
           pwm1 => Y1
           pwm2 => Y2
        tgt_pwm => Y    
    
 */

    /* We are not guaranteed to not have duplicates in the array.
       Handle the cases where the values may be equal so we don't divide by zero and so we return a reasonble pwm value.
     */
    if (cps1 == cps2)
    {
        if (pwm2 == pwm1)
        {
            return pwm1;
        }
        else
        {
            return (pwm2 - pwm1)/2 + pwm1;
        }
    }
    
    return ((tgt_cps - cps1)*((int16) pwm2 - (int16) pwm1))/(cps2 - cps1) + (int16) pwm1;
}

static uint16 calculate_pwm(int32 cps, int32 *cps_data, uint16 *pwm_data, uint8 data_size)
{   
    uint16 tgt_pwm = PWM_STOP;
    uint8 lower = 0;
    uint8 upper = 0;

    if (cps > 0 || cps < 0)
    {
        BinaryRangeSearch( cps, cps_data, data_size, &lower, &upper);
        
        tgt_pwm = Interpolate(cps, cps_data[lower], cps_data[upper], pwm_data[lower], pwm_data[upper]);

        return constrain(tgt_pwm, MIN_PWM_VALUE, MAX_PWM_VALUE);
    }

    return tgt_pwm;
}

static void calc_motor_output_from_cps(float cps, MOTOR_TYPE *motor)
{
    int32 tgt_cps = 0;

    if (cps > 0)
    {
        tgt_cps = constrain((int32)(cps * motor->p_fwd_cal_data->cps_scale), motor->p_fwd_cal_data->cps_min, motor->p_fwd_cal_data->cps_max);
        motor->pwm = calculate_pwm(tgt_cps, &motor->p_fwd_cal_data->cps_data[0], &motor->p_fwd_cal_data->pwm_data[0], CAL_DATA_SIZE);
        motor->cps = tgt_cps;
    }
    else if (cps < 0)
    {   
        tgt_cps = constrain((int32)(cps * motor->p_bwd_cal_data->cps_scale), motor->p_bwd_cal_data->cps_min, motor->p_bwd_cal_data->cps_max);
        motor->pwm = calculate_pwm(tgt_cps, &motor->p_bwd_cal_data->cps_data[0], &motor->p_bwd_cal_data->pwm_data[0], CAL_DATA_SIZE);
        motor->cps = tgt_cps;
    }
    else
    {
        motor->pwm = PWM_STOP;
        motor->cps = 0;
    }

    MOTOR_DUMP(motor);
}

static void PrintFormatDataInt32(char *label, uint8 data_size, int32 *data)
{
    uint8 ii;
    char buf[32];
    char label_str[20];
    
    sprintf(label_str, "%s\r\n", label);
    Ser_PutString(label_str);
    for ( ii = 0; ii < data_size - 1; ++ii)
    {
        if ((ii > 0) && (ii % 10 == 0))
        {
            sprintf(buf, "%ld,\r\n", data[ii]);
        }
        else
        {            
            sprintf(buf, "%ld,", data[ii]);
        }
        Ser_PutString(buf);
    }
    sprintf(buf, "%ld\r\n", data[ii]);
    Ser_PutString(buf);
}

static void PrintFormatDataUint16(char *label, uint8 data_size, uint16 *data)
{
    uint8 ii;
    char buf[32];
    char label_str[20];
    
    sprintf(label_str, "%s\r\n", label);
    Ser_PutString(label_str);
    for ( ii = 0; ii < data_size - 1; ++ii)
    {
        if ((ii > 0) && (ii % 10 == 0))
        {
            sprintf(buf, "%d,\r\n", data[ii]);
        }
        else
        {            
            sprintf(buf, "%d,", data[ii]);
        }
        Ser_PutString(buf);
    }
    sprintf(buf, "%d\r\n", data[ii]);
    Ser_PutString(buf);
}

static void DumpMotorCal(char *name, MOTOR_TYPE *motor)
{

    Ser_PutString(name);
    
    PrintFormatDataInt32("Forward CPS", CAL_DATA_SIZE, motor->p_bwd_cal_data->cps_data);
    PrintFormatDataUint16("Forward PWM\r\n", CAL_DATA_SIZE, motor->p_fwd_cal_data->pwm_data);
  
    PrintFormatDataInt32("Backward CPS", CAL_DATA_SIZE, motor->p_bwd_cal_data->cps_data);
    PrintFormatDataUint16("Backward PWM\r\n", CAL_DATA_SIZE, motor->p_bwd_cal_data->pwm_data);
    
}

void Motor_Init()
{
    left_motor.enable(HB25_DISABLE);
    right_motor.enable(HB25_DISABLE);
}

void Motor_Start()
{
    Cal_LeftGetCalData(&left_motor.p_fwd_cal_data, &left_motor.p_bwd_cal_data);
    Cal_RightGetCalData(&right_motor.p_fwd_cal_data, &right_motor.p_bwd_cal_data);
    
    
    
    /* Enable the power on the HB-25 motor 
       Note: The HB-25 has a specific initialization sequence that is handled in hardware (see HB-25 reference).  
       All that is necessary in software is to enable power to the HB-25 motor controller and start the PWM.  
       The PWM will be enabled on to the HB-25 signal pin when the HB-25 indicates initialization is complete.
     */
    left_motor.enable(HB25_ENABLE);    
    left_motor.start();
    left_motor.set_pwm(PWM_STOP);
    right_motor.enable(HB25_ENABLE);
    right_motor.start();
    right_motor.set_pwm(PWM_STOP);    
}

void Motor_LeftSetPwm(uint16 pwm)
{
    left_motor.pwm = constrain(pwm, MIN_PWM_VALUE, MAX_PWM_VALUE);
    left_motor.set_pwm(left_motor.pwm);
}

void Motor_RightSetPwm(uint16 pwm)
{
    right_motor.pwm = constrain(pwm, MIN_PWM_VALUE, MAX_PWM_VALUE);
    right_motor.set_pwm(right_motor.pwm);
}
    
void Motor_SetPwm(uint16 left_pwm, uint16 right_pwm)
{
    Motor_LeftSetPwm(left_pwm);
    Motor_RightSetPwm(right_pwm);
}
        
void Motor_LeftSetCntsPerSec(float cps)
{
    calc_motor_output_from_cps(cps, &left_motor);
        
    left_motor.set_pwm(left_motor.pwm);
}
        
void Motor_RightSetCntsPerSec(float cps)
{
    calc_motor_output_from_cps(cps, &right_motor);
          
    right_motor.set_pwm(right_motor.pwm);
}
          
void Motor_LeftSetMeterPerSec(float mps)
{
    /* convert mps to cps and call Motor_LeftSetCntsPerSec */
    int16 cps = mps / METER_PER_COUNT;
    Motor_LeftSetCntsPerSec(cps);
}
          
void Motor_RightSetMeterPerSec(float mps)
{
    /* convert mmps to cps and call Motor_RightSetCntsPerSec */
    int16 cps = mps / METER_PER_COUNT;
    Motor_RightSetCntsPerSec(cps);
}
          
uint16 Motor_LeftGetPwm()
{
    return left_motor.pwm;
}
           
uint16 Motor_RightGetPwm()
{
    return right_motor.pwm;
}

void Motor_Stop()
{
    left_motor.stop();
    right_motor.stop();
    left_motor.enable(HB25_DISABLE);
    right_motor.enable(HB25_DISABLE);
}

static void WriteTwoValues(uint16 pwm, float cps)
{
    char cps_str[10];
    
    ftoa(cps, cps_str, 3);
    
    DEBUG_PRINT("%d %s \r\n", pwm, cps_str);
}
    
static int32 CollectCpsPwmSamples(MOTOR_TYPE *motor, GET_ENCODER_TYPE encoder, uint8 num_avg_iter)
{
    uint8 ii;
    float cnts_per_sec_sum;
    float enc_cnts_per_sec;
    uint16 iterations;
    
    cnts_per_sec_sum = 0;
    num_avg_iter *= 5;
    iterations = num_avg_iter + 5;
    motor->set_pwm(motor->pwm);
    for ( ii = 0; ii < iterations; ++ii)
    {
        Encoder_Update();
        if (ii >= 5)
        {
            enc_cnts_per_sec = encoder();        
            cnts_per_sec_sum += enc_cnts_per_sec * CAL_SCALE_FACTOR;
        }
        CyDelay(10);
    }
    
    int32 avg_cps = (int32) cnts_per_sec_sum / num_avg_iter;
    WriteTwoValues(motor->pwm, avg_cps);
    return avg_cps;
}

static void RampSpeed(MOTOR_TYPE *motor, uint32 time_ms, uint16 pwm)
{
    uint16 speed_change = abs(motor->pwm - pwm);    
    int32 speed_step = (int32) speed_change / time_ms;
    uint16 ii;
    uint16 ramp_pwm = motor->pwm;
    
    for (ii = 0; ii < time_ms; ++ii)
    {
        ramp_pwm -= speed_step; 
        motor->set_pwm(ramp_pwm);
        CyDelay(1);
    }
}

void Motor_CollectPwmCpsSamples(WHEEL_TYPE wheel, uint8 reverse_pwm, uint8 num_avg_iter, uint16 *pwm_samples, int32 *cps_samples)
{
    uint8 index;
    MOTOR_TYPE *motor;
    GET_ENCODER_TYPE encoder;

    motor = (wheel == LEFT_WHEEL) ? &left_motor : &right_motor;
    encoder = (wheel == LEFT_WHEEL) ? Encoder_LeftGetCntsPerSec : Encoder_RightGetCntsPerSec;
    motor->pwm = PWM_STOP;
    motor->set_pwm(motor->pwm);
    CyDelay(500);
    Encoder_Reset();

    /* Note: The pwm samples are loaded in ascending order wrt to the pwm value, e.g., 1500 .. 200 forward, 1000 .. 1500
       This is so the interpolation will work without any special handling.  However, when calculating the CPS, it is 
       necessary to apply the pwm values in reverse order when collecting CPS values in the 1000 to 1500 range so that
       the motor moves in from stop to full speed.
    
       This range is in the backward direction on the left wheel and the forward direction on the right wheel.
     */
    memset(cps_samples, 0, CAL_NUM_SAMPLES * sizeof(int32));
    
    if (reverse_pwm)
{
        for ( index = 0; index < CAL_NUM_SAMPLES; ++index)
        {
            uint8 offset = CAL_NUM_SAMPLES - 1 - index;
            motor->pwm = pwm_samples[offset];
            cps_samples[offset] = CollectCpsPwmSamples(motor, encoder, num_avg_iter);
        }
}
    else
    {
        for ( index = 0; index < CAL_NUM_SAMPLES; ++index)
        {
            motor->pwm = pwm_samples[index];
            cps_samples[index] = CollectCpsPwmSamples(motor, encoder, num_avg_iter);
        }
    }
    
    RampSpeed(motor, 500, PWM_STOP);
    motor->pwm = PWM_STOP;
    motor->set_pwm(motor->pwm);
}

void Motor_LeftSetCalibration(CAL_DATA_TYPE *fwd_cal_data, CAL_DATA_TYPE *bwd_cal_data)
{
    left_motor.p_fwd_cal_data = fwd_cal_data;
    left_motor.p_bwd_cal_data = bwd_cal_data;
    
    DumpMotorCal("Left\r\n", &left_motor);
}

void Motor_RightSetCalibration(CAL_DATA_TYPE *fwd_cal_data, CAL_DATA_TYPE *bwd_cal_data)
{
    right_motor.p_fwd_cal_data = fwd_cal_data;
    right_motor.p_bwd_cal_data = bwd_cal_data;
    
    DumpMotorCal("Right\r\n", &right_motor);
}


/* [] END OF FILE */
