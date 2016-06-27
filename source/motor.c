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

static MOTOR_TYPE left_motor = {
    "left",
    LEFT_PWM_STOP,
    0,
    0,
    0,
    Left_HB25_Enable_Pin_Write,
    Left_HB25_PWM_Start,
    Left_HB25_PWM_Stop,
    Left_HB25_PWM_WriteCompare
};

static MOTOR_TYPE right_motor = {
    "right",
    RIGHT_PWM_STOP,
    0,
    0,
    0,
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
    
    if (MOTOR_DEBUG_CONTROL_ENABLED)
    {
        DEBUG_PRINT_ARG("%s: %s %d \r\n", motor->name, cps_str, motor->pwm);
    }
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

void Motor_Init()
{
    left_motor.enable(HB25_DISABLE);
    right_motor.enable(HB25_DISABLE);
}

void Motor_Start()
{
    Cal_LeftGetMotorCalData(&left_motor.p_fwd_cal_data, &left_motor.p_bwd_cal_data);
    Cal_RightGetMotorCalData(&right_motor.p_fwd_cal_data, &right_motor.p_bwd_cal_data);
    
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
    int16 cps = mps / METER_PER_COUNT;
    Motor_LeftSetCntsPerSec(cps);
}
          
void Motor_RightSetMeterPerSec(float mps)
{
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
    
    return (int32) cnts_per_sec_sum / num_avg_iter;
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
}

void Motor_RightSetCalibration(CAL_DATA_TYPE *fwd_cal_data, CAL_DATA_TYPE *bwd_cal_data)
{
    right_motor.p_fwd_cal_data = fwd_cal_data;
    right_motor.p_bwd_cal_data = bwd_cal_data;
}


/* [] END OF FILE */
