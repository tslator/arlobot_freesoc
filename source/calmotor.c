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


/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <limits.h>
#include "calmotor.h"
#include "motor.h"
#include "pwm.h"
#include "nvstore.h"
#include "serial.h"
#include "encoder.h"
#include "time.h"
#include "utils.h"
#include "debug.h"
#include "pid.h"


/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define MAX_MOTOR_CAL_ITERATION (3)
#define MOTOR_VALIDATION_LOWER_BOUND_PERCENTAGE (0.2)
#define MOTOR_VALIDATION_UPPER_BOUND_PERCENTAGE (0.8)

/* For motor validation, 11 points were determined to be sufficient to validate the range 
 * When selecting a different range, choose an odd number so that the middle value is the
 * peak of the triangle
 */
#define MID_SAMPLE_OFFSET (5)
#define NEXT_SAMPLE_OFFSET (MID_SAMPLE_OFFSET + 1)
#define PREV_SAMPLE_OFFSET (MID_SAMPLE_OFFSET - 1)
#define NUM_FORWARD_SAMPLES (MID_SAMPLE_OFFSET)
#define NUM_BACKWARD_SAMPLES (MID_SAMPLE_OFFSET)
    

/*
    Explanation of Count/Sec to PWM Calibration:

    Calibration involves mapping count/sec values to PWM values.  Each motor is run (forward and backward) collecting
    an average count/sec for each PWM sample.  The results are stored in an structure (CAL_DATA_TYPE) that holds the
    count/sec averages, PWM samples, min/max count/sec and a scale factor.  There are four CAL_DATA_TYPE instances: one
    for left motor forward, left motor backward, right motor forward and right motor backward.  The calibration data is
    stored in NVRAM on the Psoc and pointers to the calibration data are passed to the motor module.
    
    Upon startup, the calibration data is made available to the motors via pointers to NVRAM (Note: the EEPROM component
    maps NVRAM to memory so there is no appreciable overhead in reading from NVRAM (or so I believe until proven otherwise)

 */

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static int32 cps_samples[CAL_NUM_SAMPLES];
static uint16 pwm_samples[CAL_NUM_SAMPLES];
static int32 cps_sum[CAL_NUM_SAMPLES];
static CAL_DATA_TYPE cal_data;

static float fwd_cps_validate[11] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static float bwd_cps_validate[11] = {-0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0};

static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static CALIBRATION_TYPE motor_calibration = {CAL_INIT_STATE, 
                                             CAL_CALIBRATE_STAGE,
                                             0,
                                             Init, 
                                             Start, 
                                             Update, 
                                             Stop, 
                                             Results };

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Print Functions
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Name: PrintAllMotorParams
 * Description: Prints the left/right, forward/backward count/sec and pwm calibration values
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void PrintAllMotorParams()
{
    Ser_PutString("\r\n");
    Cal_PrintSamples("Left-Backward", (int32 *) p_cal_eeprom->left_motor_bwd.cps_data, (uint16 *) p_cal_eeprom->left_motor_bwd.pwm_data);
    Cal_PrintSamples("Left-Forward", (int32 *) p_cal_eeprom->left_motor_fwd.cps_data, (uint16 *) p_cal_eeprom->left_motor_fwd.pwm_data);
    Cal_PrintSamples("Right-Backward", (int32 *) p_cal_eeprom->right_motor_bwd.cps_data, (uint16 *) p_cal_eeprom->right_motor_bwd.pwm_data);
    Cal_PrintSamples("Right-Forward", (int32 *) p_cal_eeprom->right_motor_fwd.cps_data, (uint16 *) p_cal_eeprom->right_motor_fwd.pwm_data);
}

/*---------------------------------------------------------------------------------------------------
 * Name: PrintVelocityParams
 * Description: Prints the specified count/sec, meter/sec, and pwm values
 * Parameters: label - string identifying the wheel 
 *             cps - count/sec value
 *             mps - meter/sec value
 *             pwm - pulse-width modulation value
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void PrintVelocityParams(char *label, float cps, float mps, uint16 pwm)
{
    char cps_str[10];
    char mps_str[10];
    char output[64];
    
    ftoa(cps, cps_str, 3);
    ftoa(mps, mps_str, 3);
    
    sprintf(output, "%s - cps: %s, mps: %s, pwm: %d\r\n", label, cps_str, mps_str, pwm);
    Ser_PutString(output);
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalculateValidationValues
 * Description: Calculates an array of values using a triangle profile based on calibration motor 
 *              values. 
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalculateValidationValues()
/* This routine calculates a series of count/second values in a triangle profile (slow, fast, slow).  It uses the motor
   calibration data to determine a range  of forward and reverse values for each wheel.  The routine is called from
   motor validation to confirm that motor calibration conversion from count/second to pwm is reasonably accurate.
 */
{
    uint8 ii;
    
    if (p_cal_eeprom->status & CAL_MOTOR_BIT)
    {    
        float left_forward_cps_max = p_cal_eeprom->left_motor_fwd.cps_max / p_cal_eeprom->left_motor_fwd.cps_scale;
        float right_forward_cps_max = p_cal_eeprom->right_motor_fwd.cps_max / p_cal_eeprom->right_motor_fwd.cps_scale;
        float left_backward_cps_min = p_cal_eeprom->left_motor_bwd.cps_min / p_cal_eeprom->left_motor_bwd.cps_scale;
        float right_backward_cps_min = p_cal_eeprom->right_motor_bwd.cps_min / p_cal_eeprom->right_motor_bwd.cps_scale;

        float forward_cps_max = min(left_forward_cps_max, right_forward_cps_max);
        /* Note: Backward values are negative */
        float backward_cps_max = max(left_backward_cps_min, right_backward_cps_min);
        
        float fwd_cps_start = MOTOR_VALIDATION_LOWER_BOUND_PERCENTAGE * forward_cps_max;
        float fwd_cps_end = MOTOR_VALIDATION_UPPER_BOUND_PERCENTAGE * forward_cps_max;
        float bwd_cps_start = MOTOR_VALIDATION_LOWER_BOUND_PERCENTAGE * backward_cps_max;
        float bwd_cps_end = MOTOR_VALIDATION_UPPER_BOUND_PERCENTAGE * backward_cps_max;
        
        float fwd_cps_delta = (fwd_cps_end - fwd_cps_start)/NUM_FORWARD_SAMPLES;
        float bwd_cps_delta = (bwd_cps_end - bwd_cps_start)/NUM_BACKWARD_SAMPLES;
        
        fwd_cps_validate[MID_SAMPLE_OFFSET] = forward_cps_max;
        bwd_cps_validate[MID_SAMPLE_OFFSET] = backward_cps_max;
        
        for (ii = 0; ii < MID_SAMPLE_OFFSET; ++ii)
        {
            fwd_cps_validate[ii] = (fwd_cps_start + fwd_cps_delta * ii);
            bwd_cps_validate[ii] = (bwd_cps_start + bwd_cps_delta * ii);
        }
        
        for (ii = 0; ii < MID_SAMPLE_OFFSET; ++ii)
        {
            fwd_cps_validate[NEXT_SAMPLE_OFFSET + ii] = fwd_cps_validate[PREV_SAMPLE_OFFSET - ii];
            bwd_cps_validate[NEXT_SAMPLE_OFFSET + ii] = bwd_cps_validate[PREV_SAMPLE_OFFSET - ii];
        }
    }
}

typedef struct _pwm_params
{
    uint16 start;
    uint16 end;
    uint16 step;
    uint8  reverse;
} PWM_PARAMS_TYPE;

PWM_PARAMS_TYPE pwm_params[2][2] = {{{LEFT_PWM_STOP, LEFT_PWM_FULL_FORWARD, LEFT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1), 0}, 
                                     {LEFT_PWM_FULL_BACKWARD, LEFT_PWM_STOP, LEFT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1), 1}
                                    }, 
                                    {
                                     {RIGHT_PWM_STOP, RIGHT_PWM_FULL_FORWARD, RIGHT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1), 0}, 
                                     {RIGHT_PWM_FULL_BACKWARD, RIGHT_PWM_STOP, RIGHT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1), 1}
                                    }};

/*---------------------------------------------------------------------------------------------------
 * Name: CalculatePwmSamples
 * Description: Calculates an array of pwm values based on the specified wheel, and direction. 
 * Parameters: wheel - left/right wheel 
 *             dir - forward/backward direction
 *             pwm_samples - array of pwm values
 *             reverse_pwm - indicates the ordering within the pwm samples
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalculatePwmSamples(WHEEL_TYPE wheel, DIR_TYPE dir, uint16 *pwm_samples, uint8 *pwm_reverse)
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
    
    #define CONDITION(wheel)   (wheel == WHEEL_LEFT ? ++ii, pwm <= pwm_end : ++ii, pwm >= pwm_end)
    #define STEP(wheel)        (wheel == WHEEL_LEFT ? ++ii, pwm += pwm_step : ++ii, pwm -= pwm_step)
    
    uint8 ii;
    uint16 pwm;
    uint16 pwm_start;
    uint16 pwm_end;
    uint16 pwm_step;

    pwm_start = pwm_params[wheel][dir].start;
    pwm_end = pwm_params[wheel][dir].end;
    pwm_step = pwm_params[wheel][dir].step;
    *pwm_reverse = pwm_params[wheel][dir].reverse;
    for (ii = 0, pwm = pwm_start; CONDITION(wheel); STEP(wheel))
    {
        pwm_samples[ii] = pwm;
    }
#ifdef XXX

    if (wheel == WHEEL_LEFT)
    {
        if (dir == DIR_FORWARD)
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
        if (dir == DIR_FORWARD)
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
#endif
}

/*---------------------------------------------------------------------------------------------------
 * Name: AddSamplesToSum
 * Description: Adds an array of values to a sum 
 * Parameters: cap_samples - array of count/sec samples
 *             cps_sum - resulting sum
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void AddSamplesToSum(int32 *cps_samples, int32 *cps_sum)
{
    uint8 ii;
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        cps_sum[ii] += cps_samples[ii];
    }
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: CalculateAvgCpsSamples
 * Description: Calculates an average for each array entry
 * Parameters: cps_sum - array of count/sec sums
 *             num_runs - number of runs used to collect the sum
 *             avg_cps_samples - array of averages
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalculateAvgCpsSamples(int32 *cps_sum, uint8 num_runs, int32 *avg_cps_samples)
{
    uint8 ii;
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        avg_cps_samples[ii] = cps_sum[ii]/num_runs;
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalculateMinMaxCpsSample
 * Description: Calculates the min/max from an array of values
 * Parameters: samples - array of values
 *             num_samples - the number of samples in the array
 *             min - the minimum value of the array
 *             max - the maximum value of the array
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalculateMinMaxCpsSample(int32 *samples, uint8 num_samples, int32 *min, int32 *max)
{
    uint8 ii;
    *min = INT_MAX;
    *max = INT_MIN;    
    
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

/*---------------------------------------------------------------------------------------------------
 * Name: CollectCpsPwmSamples
 * Description: Collects count/sec values at a given pwm value
 * Parameters: set_pwm - motor set pwm function
 *             cnts_per_sec - encoder get count/sec function
 *             pwm - the pwm to be driven
 *             num_avg_iter - 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static int32 CollectCpsPwmSamples(SET_MOTOR_PWM_FUNC_TYPE set_pwm, GET_ENCODER_FUNC_TYPE cnts_per_sec, uint16 pwm, uint8 num_avg_iter)
{
    uint8 ii;
    float cnts_per_sec_sum;
    float enc_cnts_per_sec;
    uint16 iterations;
    
    cnts_per_sec_sum = 0;
    num_avg_iter *= 5;
    iterations = num_avg_iter + 5;
    set_pwm(pwm);
    for ( ii = 0; ii < iterations; ++ii)
    {
        Encoder_Update();
        if (ii >= 5)
        {
            enc_cnts_per_sec = cnts_per_sec();        
            cnts_per_sec_sum += enc_cnts_per_sec * CAL_SCALE_FACTOR;
        }
        CyDelay(10);
    }
    
    return (int32) cnts_per_sec_sum / num_avg_iter;
}

/*---------------------------------------------------------------------------------------------------
 * Name: CollectPwmCpsSamples
 * Description: 
 * Parameters: wheel - 
 *             reverse_pwm -
 *             num_avg_iter -
 *             pwm_samples -
 *             cps_samples - 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CollectPwmCpsSamples(WHEEL_TYPE wheel, uint8 reverse_pwm, uint8 num_avg_iter, uint16 *pwm_samples, int32 *cps_samples)
{
    uint8 index;
    SET_MOTOR_PWM_FUNC_TYPE set_pwm;
    RAMP_DOWN_PWM_FUNC_TYPE ramp_down;
    GET_ENCODER_FUNC_TYPE get_cps;    

    set_pwm   = (wheel == WHEEL_LEFT) ? Motor_LeftSetPwm : Motor_RightSetPwm;
    ramp_down = (wheel == WHEEL_LEFT) ? Motor_LeftRampDown : Motor_RightRampDown;
    get_cps   = (wheel == WHEEL_LEFT) ? Encoder_LeftGetCntsPerSec : Encoder_RightGetCntsPerSec;
    
    set_pwm(PWM_STOP);
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
            cps_samples[offset] = CollectCpsPwmSamples(set_pwm, get_cps, pwm_samples[offset], num_avg_iter);
        }
    }
    else
    {
        for ( index = 0; index < CAL_NUM_SAMPLES; ++index)
        {
            cps_samples[index] = CollectCpsPwmSamples(set_pwm, get_cps, pwm_samples[index], num_avg_iter);
        }
    }
    
    ramp_down(1000);
    set_pwm(PWM_STOP);
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalibrateWheelSpeed
 * Description: 
 * Parameters: wheel - 
 *             dir -
 *             num_runs -
 *             cps_samples -
 *             pwm_samples - 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalibrateWheelSpeed(WHEEL_TYPE wheel, DIR_TYPE dir, int32 *cps_samples, uint16 *pwm_samples)
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
    
    for (run = 0; run < MAX_MOTOR_CAL_ITERATION; ++run)
    {
        CollectPwmCpsSamples(wheel, reverse_pwm, num_avg_iter, pwm_samples, cps_samples);
        AddSamplesToSum(cps_samples, cps_sum);
    }
    
    CalculateAvgCpsSamples(cps_sum, MAX_MOTOR_CAL_ITERATION, cps_samples);
}

/*---------------------------------------------------------------------------------------------------
 * Name: StoreWheelSpeedSamples
 * Description: 
 * Parameters: wheel - 
 *             dir -
 *             cps_samples -
 *             pwm_samples - 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void StoreWheelSpeedSamples(WHEEL_TYPE wheel, DIR_TYPE dir, int32 *cps_samples, uint16 *pwm_samples)
{
    CalculateMinMaxCpsSample(cps_samples, CAL_NUM_SAMPLES, &cal_data.cps_min, &cal_data.cps_max);
    cal_data.cps_scale = CAL_SCALE_FACTOR;
    memcpy(&cal_data.cps_data[0], cps_samples, sizeof(cps_samples[0]) * CAL_NUM_SAMPLES);
    memcpy(&cal_data.pwm_data[0], pwm_samples, sizeof(pwm_samples[0]) * CAL_NUM_SAMPLES);

    CAL_DATA_TYPE *p_cal_data = WHEEL_DIR_TO_CAL_DATA[wheel][dir];
    
    uint16 offset = NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(p_cal_data);
    Nvstore_WriteBytes((uint8 *) &cal_data, sizeof(cal_data), offset);
}

/*---------------------------------------------------------------------------------------------------
 * Name: DoVelocityValidation
 * Description: 
 * Parameters: label - 
 *             cps -
 *             num_cps -
 *             wheel - 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void DoVelocityValidation(char *label, float *cps, uint8 num_cps, WHEEL_TYPE wheel)
{
    uint8 ii;
    uint16 pwm;
    uint32 start_time;
    float mps = 0.0;
    SET_MOTOR_PWM_FUNC_TYPE set_pwm;
    GET_MOTOR_PWM_FUNC_TYPE get_pwm;
    GET_ENCODER_FUNC_TYPE get_mps;
    
    switch (wheel)
    {
        case WHEEL_LEFT:
            set_pwm = Motor_LeftSetPwm;
            get_pwm = Motor_LeftGetPwm;
            get_mps = Encoder_LeftGetMeterPerSec;
            break;

        case WHEEL_RIGHT:
            set_pwm = Motor_RightSetPwm;
            get_pwm = Motor_RightGetPwm;
            get_mps = Encoder_RightGetMeterPerSec;
            break;
        
        case WHEEL_BOTH:
        default:
            return;
    }
    
    set_pwm(PWM_STOP);
    
    for (ii = 0; ii < num_cps; ++ii)
    {
        pwm = Cal_CpsToPwm(wheel, cps[ii]);
        set_pwm(pwm);
        start_time = millis();
        while (millis() - start_time < 2000)
        {
            Encoder_Update();
            mps = get_mps();
        }
        
        PrintVelocityParams(label, cps[ii], mps, get_pwm());
    }
    
    set_pwm(PWM_STOP);
}

/*----------------------------------------------------------------------------------------------------------------------
 * Calibration Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: Init
 * Description: Calibration/Validation interface Init function.  Performs initialization for Linear 
 *              Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - PID calibration/validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Init(CAL_STAGE_TYPE stage, void *params)
{
    params = params;

    Pid_Enable(FALSE);

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nInitialize motor calibration\r\n");            
            break;
            
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nInitialize motor validation\r\n");
            CalculateValidationValues();
            break;
    }
    
    return CAL_OK;    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Linear Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - motor validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Start(CAL_STAGE_TYPE stage, void *params)
{
    params = params;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nPerforming motor calibration\r\n");
            break;
        
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nPerforming motor validation\r\n");
            break;
    }
    
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: stage - the calibration/validation stage 
 *             params - motor validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Update(CAL_STAGE_TYPE stage, void *params)
{
    uint16 last_debug_control_enabled;

    params = params;
    
    last_debug_control_enabled = debug_control_enabled;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("Left-Forward Calibration\r\n");
            debug_control_enabled = DEBUG_LEFT_ENCODER_ENABLE_BIT;

            CalibrateWheelSpeed(WHEEL_LEFT, DIR_FORWARD, cps_samples, pwm_samples);
            StoreWheelSpeedSamples(WHEEL_LEFT, DIR_FORWARD, cps_samples, pwm_samples);
            
            Ser_PutString("Left-Backward Calibration\r\n");
            CalibrateWheelSpeed(WHEEL_LEFT, DIR_BACKWARD, cps_samples, pwm_samples);
            StoreWheelSpeedSamples(WHEEL_LEFT, DIR_BACKWARD, cps_samples, pwm_samples);
            
            Ser_PutString("Right-Forward Calibration\r\n");
            debug_control_enabled = DEBUG_RIGHT_ENCODER_ENABLE_BIT;                    
            CalibrateWheelSpeed(WHEEL_RIGHT, DIR_FORWARD, cps_samples, pwm_samples);
            StoreWheelSpeedSamples(WHEEL_RIGHT, DIR_FORWARD, cps_samples, pwm_samples);
            
            Ser_PutString("Right-Backward Calibration\r\n");
            CalibrateWheelSpeed(WHEEL_RIGHT, DIR_BACKWARD, cps_samples, pwm_samples);
            StoreWheelSpeedSamples(WHEEL_RIGHT, DIR_BACKWARD, cps_samples, pwm_samples);

            debug_control_enabled = last_debug_control_enabled;
            return CAL_COMPLETE;
            break;
            
        case CAL_VALIDATE_STAGE:
            DoVelocityValidation("left-forward", fwd_cps_validate, 11, WHEEL_LEFT);
            DoVelocityValidation("right-forward", fwd_cps_validate, 11, WHEEL_RIGHT);
            DoVelocityValidation("left-backward", bwd_cps_validate, 11, WHEEL_LEFT);
            DoVelocityValidation("right-backward", bwd_cps_validate, 11, WHEEL_RIGHT);
            return CAL_COMPLETE;
            break;
    }
    
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Stop
 * Description: Calibration/Validation interface Stop function.  Called to stop validation.
 * Parameters: stage - the calibration/validation stage
 *             params - motor validation parameters 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Stop(CAL_STAGE_TYPE stage, void *params)
{
    params = params;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("Motor calibration complete\r\n");
            SetCalibrationStatusBit(CAL_MOTOR_BIT);
            break;
        case CAL_VALIDATE_STAGE:
            Ser_PutString("Motor validation complete\r\n");
            break;
    }
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Results
 * Description: Calibration/Validation interface Results function.  Called to display calibration/ 
 *              validation results. 
 * Parameters: stage - the calibration/validation stage 
 *             params - motor calibration/validation parameters 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Results(CAL_STAGE_TYPE stage, void *params)
{
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nPrinting motor calibration results\r\n");
            PrintAllMotorParams();
            break;
        
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nPrinting motor validation results\r\n");
            /* I think what we want here is a kind of analysis that shows how close the count/sec and pwm values are */
            //Val_PrintAllMotorParams();
            break;
    }
    return CAL_OK;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Module Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/
void CalMotor_Init()
{
    WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_FORWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_fwd;
    WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_BACKWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_bwd;
    WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_FORWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_fwd;
    WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_BACKWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_bwd;
    
    CalMotor_Calibration = &motor_calibration;
    CalMotor_Validation = &motor_calibration;    
}

