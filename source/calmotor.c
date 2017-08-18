/* 
MIT License

Copyright (c) 2017 Tim Slator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


/*---------------------------------------------------------------------------------------------------
   Description: This module provides the implementation for calibrating the motors.
 *-------------------------------------------------------------------------------------------------*/    

/* 
    Explanation of Motor Calibration using Count/Sec to PWM mapping:

    Calibration involves mapping count/sec values to PWM values.  Each motor is run (forward and backward) collecting
    an average count/sec for each PWM sample.  The results are stored in an structure (CAL_DATA_TYPE) that holds the
    count/sec averages, PWM samples, min/max count/sec and a scale factor.  There are four CAL_DATA_TYPE instances: one
    for left motor forward, left motor backward, right motor forward and right motor backward.  The calibration data is
    stored in NVRAM on the Psoc and pointers to the calibration data are passed to the motor module.
    
    Upon startup, the calibration data is made available to the motors via pointers to NVRAM (Note: the EEPROM component
    maps NVRAM to memory so there is no appreciable overhead in reading from NVRAM (or so I believe until proven otherwise)

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

#include "leftpid.h"
#include "rightpid.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define MAX_MOTOR_CAL_ITERATION (1)
#define CALIBRATION_ITERATION_DONE (255)
#define MAX_CPS_ARRAY (51)
#define NUM_MOTOR_CAL_PARAMS (4)
#define PWM_TEST_TIME (250)
#define CPS_SAMPLE_TIME (25)
#define MOTOR_RAMP_DOWN_TIME (1000) // millisecond

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef struct _cal_motor_params
{
    char        label[30]; 
    WHEEL_TYPE  wheel;
    DIR_TYPE    direction;
    uint8       iterations;
    uint32      pwm_time;
    uint8       pwm_index;
    PWM_TYPE    *p_pwm_samples;
    uint32      sample_time;
    uint8       cps_index;
    int32       *p_cps_samples;
    int32       *p_cps_avg;
    SET_MOTOR_PWM_FUNC_TYPE set_pwm;
    RAMP_DOWN_PWM_FUNC_TYPE ramp_down;
    GET_RAW_COUNT_FUNC_TYPE get_count;
    RESET_COUNT_FUNC_TYPE reset;
} CAL_MOTOR_PARAMS;


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static int32         cal_cps_samples[CAL_NUM_SAMPLES];
static uint16        cal_pwm_samples[CAL_NUM_SAMPLES];
static int32         cal_cps_avg[CAL_NUM_SAMPLES];
static CAL_DATA_TYPE cal_data;

/* Provides an implementation of the Calibration interface */
static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static CALIBRATION_TYPE motor_calibration = { CAL_INIT_STATE, 
                                              CAL_CALIBRATE_STAGE,
                                              NULL,
                                              Init, 
                                              Start, 
                                              Update, 
                                              Stop, 
                                              Results };

/* The PWM params: start, end, step are defined for each motor (left/right) and each direction (forward/backward)
   In general, the servo interface to the motors is defined as follows:
        1500 - stop
        2000 - full forward
        1000 - full reverse
 
   The orientation of the motors is such that the left more moves as above, but the right motor is rotated 180 degree
   such that:
        1500 - stop
        2000 - full reverse
        1000 - full forward
 
   The following is organized so that the count/sec samples are ordered numerically increase, i.e., lowest to highest.
   For the forward direction, the count/sec values range from 0 to Max Forward (a positive number).
   For the reverse direction, the count/sec values range from Max Revers (a negative number) to 0.
 
   Note: The ordering is a constraint of the binary search algorithm.
 
   Consequently, the organization below is:
       Left Wheel, Forward
            PWM runs from 1500 to 2000, Lowest CPS corresponds to 1500
            PWM array index runs from 0 to Max
            CPS array index runs from 0 to Max
    
       Left Wheel, Backward
            PWM runs from 1000 to 1500, Lowest CPS corresponds to 1000
            PWM array index runs from Max to 0
            CPS array index runs from Max to 0
    
       Right Wheel, Forward
            PWM runs from 1500 to 1000, Lowest CPS corresponds to 1500
            PWM array index runs from 0 to Max
            CPS array index runs from 0 to Max
    
       Right Wheel, Backward
            PWM runs from 2000 to 1500, Lowest CPS corresponds to 2000
            PWM array index runs from Max to 0
            CPS array index runs from Max to 0
 
   The above ensures that the pwm array and cps array is matched and ensures that the cps values are in ascending order
   so that linear interpolation works.
 */
PWM_PARAMS_TYPE pwm_params[2][2] = {{{LEFT_PWM_STOP, LEFT_PWM_FULL_FORWARD, LEFT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1)}, 
                                     {LEFT_PWM_FULL_BACKWARD, LEFT_PWM_STOP, LEFT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1)}
                                    }, 
                                    {
                                     {RIGHT_PWM_STOP, RIGHT_PWM_FULL_FORWARD, -(RIGHT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1))}, 
                                     {RIGHT_PWM_FULL_BACKWARD, RIGHT_PWM_STOP, -(RIGHT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1))}
                                    }};


static CAL_MOTOR_PARAMS motor_cal_params[NUM_MOTOR_CAL_PARAMS] = 
{
    {
        "left-forward", 
        WHEEL_LEFT,
        DIR_FORWARD,
        /* iterations */ 3,
        /* pwm_time */ PWM_TEST_TIME,
        /* pwm_index */ 0,
        /* p_pwm_samples */ &cal_pwm_samples[0],
        /* sample_time */ CPS_SAMPLE_TIME,
        /* sample_index */ 0,
        /* p_cps_samples */ &cal_cps_samples[0],
        /* cal_cps_avg */ &cal_cps_avg[0],
        Motor_LeftSetPwm,
        Motor_LeftRampDown,
        Encoder_LeftGetRawCount,
        Encoder_LeftReset
    }, 
    {
        "left-backward",
        WHEEL_LEFT,
        DIR_BACKWARD,
        /* iterations */ 3,
        /* pwm_time */ PWM_TEST_TIME,
        0,
        /* p_pwm_samples */ &cal_pwm_samples[0],
        /* sample_time */ CPS_SAMPLE_TIME,
        0,
        /* p_cps_samples */ &cal_cps_samples[0],
        /* cal_cps_avg */ &cal_cps_avg[0],
        Motor_LeftSetPwm,
        Motor_LeftRampDown,
        Encoder_LeftGetRawCount,
        Encoder_LeftReset        
    }, 
    {
        "right-forward",
        WHEEL_RIGHT,
        DIR_FORWARD,
        /* iterations */ 3,
        /* pwm_time */ PWM_TEST_TIME,
        0,
        /* p_pwm_samples */ &cal_pwm_samples[0],
        /* sample_time */ CPS_SAMPLE_TIME,
        0,
        /* p_cps_samples */ &cal_cps_samples[0],
        /* cal_cps_avg */ &cal_cps_avg[0],
        Motor_RightSetPwm,
        Motor_RightRampDown,
        Encoder_RightGetRawCount,
        Encoder_RightReset
    }, 
    {
        "right-backward",
        WHEEL_RIGHT,
        DIR_BACKWARD,
        /* iterations */ 3,
        /* pwm_time */ PWM_TEST_TIME,
        0,
        /* p_pwm_samples */ &cal_pwm_samples[0],
        /* sample_time */ CPS_SAMPLE_TIME,
        0,
        /* p_cps_samples */ &cal_cps_samples[0],
        /* cal_cps_avg */ &cal_cps_avg[0],
        Motor_RightSetPwm,
        Motor_RightRampDown,
        Encoder_RightGetRawCount,
        Encoder_RightReset
    }
};

static uint8 motor_cal_index;

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
    Cal_PrintSamples("Left-Forward", (int32 *) p_cal_eeprom->left_motor_fwd.cps_data, (uint16 *) p_cal_eeprom->left_motor_fwd.pwm_data);
    Cal_PrintSamples("Left-Backward", (int32 *) p_cal_eeprom->left_motor_bwd.cps_data, (uint16 *) p_cal_eeprom->left_motor_bwd.pwm_data);
    Cal_PrintSamples("Right-Forward", (int32 *) p_cal_eeprom->right_motor_fwd.cps_data, (uint16 *) p_cal_eeprom->right_motor_fwd.pwm_data);
    Cal_PrintSamples("Right-Backward", (int32 *) p_cal_eeprom->right_motor_bwd.cps_data, (uint16 *) p_cal_eeprom->right_motor_bwd.pwm_data);
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
static void CalculateMinMaxCpsSample(int32 *samples, int32 *min, int32 *max)
{
    uint8 ii;
    *min = INT_MAX;
    *max = INT_MIN;    

    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        if (samples[ii] > *max)
        {
            *max = samples[ii];
        }
        
        if (samples[ii] < *min)
        {
            *min = samples[ii];
        }
        Ser_PutStringFormat("min/max: %d/%d\r\n", *min, *max);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: InitCalibrationParams
 * Description: Calculates an array of pwm values based on the specified wheel, and direction. 
 * Parameters: wheel - left/right wheel 
 *             dir - forward/backward direction
 *             pwm_samples - array of pwm values
 *             reverse_pwm - indicates the ordering within the pwm samples
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void InitCalibrationParams(CAL_MOTOR_PARAMS *params)
{
    uint8 ii;
    uint16 pwm;
    uint16 pwm_start;
    int16 pwm_step;
    WHEEL_TYPE wheel;
    DIR_TYPE dir;
     
    wheel = params->wheel;
    dir = params->direction;
        
    pwm_start = pwm_params[wheel][dir].start;
    pwm_step = pwm_params[wheel][dir].step;
     
    /* The pwm samples are filled in from lowest value to highest PWM value.  This ensures that the
       corresponding count/sec values (stored in a different array) are ordered from lowest value to
       highest value, i.e., forward: 0 to max cps, reverse: -max cps to 0.
    */
    for (ii =0, pwm = pwm_start; ii < CAL_NUM_SAMPLES; ++ii, pwm += pwm_step)
    {
        params->p_pwm_samples[ii] = pwm;
        params->p_cps_samples[ii] = 0;
        params->p_cps_avg[ii] = 0;
    }
  
    /* The first pwm entry is always PWM_STOP and it must correspond to CPS value 0 in order to stop the motor.  So,
       So, set pwm_index to start at 1.
    */
    params->pwm_index = 1;
    params->iterations = MAX_MOTOR_CAL_ITERATION;
 }
  
static uint8 GetNextPwm(CAL_MOTOR_PARAMS *params, PWM_TYPE *pwm)
{
    uint8 index;

    if (params->pwm_index < CAL_NUM_SAMPLES)
    {
        index = PWM_CALC_OFFSET(params->direction, params->pwm_index);
        *pwm = params->p_pwm_samples[index];
        params->cps_index = index;
        params->pwm_index++;
        return 0;
    }
    
    /* Reset the pwm_index for next run.  this is needed doing an average and multiple iterations 
       are run because there is no initialziation between iterations 
    */
    params->pwm_index = 1;            
    
    return 1;        
}

static uint8 PerformMotorCalibrationIteration(CAL_MOTOR_PARAMS *params)
{
    static uint8 pwm_running = FALSE;
    static uint32 pwm_start_time = 0;
    static uint32 sample_start_time = 0;
    static int32 last_count = 0;
    static uint8 num_cps_samples_collected;
    
    uint32 now;
    uint32 pwm_delta;
    PWM_TYPE pwm;
    uint32 sample;
    uint8 result;
    int32 count;
    int32 total_sample_time;
    int32 total_counts;

    if (!pwm_running)
    {        
        result = GetNextPwm(params, &pwm);
        if (result)
        {
            /* We've finished running a series of pwm values:
                1. Ramp down the motor speed to be nice to the motor
                2. Stop the motors (just to make sure)
                3. Reset the pwm index (we might not be the last iteration)
            */
            params->ramp_down(MOTOR_RAMP_DOWN_TIME);
            params->set_pwm(PWM_STOP);
            return CALIBRATION_ITERATION_DONE;
        }

        /* Start the calibration:
            1. Apply the pwm to move the motor 
            2. Set the running flag
            3. Grab the current time for pwm running and cps sampling
        */
        params->set_pwm(pwm);

        num_cps_samples_collected = 0;
        pwm_running = TRUE;
        params->reset();
        last_count = params->get_count();
        pwm_start_time = millis();
        sample_start_time = millis();
    }
    
    if (pwm_running)
    {   
        now = millis();
        pwm_delta = now - pwm_start_time;
        sample = now - sample_start_time;
        
            /* Is the pwm time up? */
        if (pwm_delta < params->pwm_time)
        {                
            /* Is is time to sample? */
            if (sample > params->sample_time)
            {
                /* Note: cps_index is set when select the pwm (see GetNextPwm) */
                sample_start_time = millis();
                count = params->get_count();
                params->p_cps_samples[params->cps_index] += (count - last_count);
                last_count = count;
                num_cps_samples_collected++;
            }
        }
        else
        {
            /* Pwm time is up */

            /* Calculate total sample time, count average and counts/second */
            total_sample_time = params->sample_time * num_cps_samples_collected;
            total_counts = params->p_cps_samples[params->cps_index];
            params->p_cps_samples[params->cps_index] = total_counts * MILLIS_PER_SECOND / total_sample_time;
            Ser_PutStringFormat("tst: %d, tc: %d, cps: %d\r\n", total_sample_time, total_counts, params->p_cps_samples[params->cps_index]);
            pwm_running = FALSE;
        } 
    }
    
    return CAL_OK;
}

static uint8 PerformMotorCalibrateAverage(CAL_MOTOR_PARAMS *params)
{
    uint8 ii;
    uint8 result;
    
    if (params->iterations > 0)
    {
        result = PerformMotorCalibrationIteration(params);
        if (result == CALIBRATION_ITERATION_DONE)
        {            
            params->iterations--;
            
            /* Sum the collected cps's */
            for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
            {
                params->p_cps_avg[ii] += params->p_cps_samples[ii];
                params->p_cps_samples[ii] = 0;
            }
        }
        return CAL_OK;
    }
    
    /* Calculate average cps's */
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        params->p_cps_avg[ii] = params->p_cps_avg[ii]/MAX_MOTOR_CAL_ITERATION;
        Ser_PutStringFormat("Avg CPS (%d): %d\r\n", params->p_pwm_samples[ii], params->p_cps_avg[ii]);
    }
    
    return CALIBRATION_ITERATION_DONE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: StoreMotorCalibration
 * Description: 
 * Parameters: wheel - 
 *             dir -
 *             cps_samples -
 *             pwm_samples - 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void StoreMotorCalibration(CAL_MOTOR_PARAMS *params)
{
    uint8 ii;

    /* Note: It was intended to add a pointer to the CAL_MOTOR_PARAMS structure that pointed to
    the cal_data variable similar to p_pwm_samples and p_cps_avg samples; however, when doing that
    the wrong values were being written to NVRAM.  Some investigation is warranted here to see
    what is happening.  As a workaround, I'm copying the pwm and cps data into cal_data in this
    function before writing to NVRAM which works.
    */

    /* Remove unwanted neg/pos values */
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        if (params->direction == DIR_FORWARD)
        {
            if (params->p_cps_avg[ii] < 0)
            {
                params->p_cps_avg[ii] = 0;
            }
        }

        if (params->direction == DIR_BACKWARD)
        {
            if (params->p_cps_avg[ii] > 0)
            {
                params->p_cps_avg[ii] = 0;
            }
        }
    }

    CalculateMinMaxCpsSample(params->p_cps_avg, &cal_data.cps_min, &cal_data.cps_max);
    
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        cal_data.pwm_data[ii] = params->p_pwm_samples[ii];
        cal_data.cps_data[ii] = params->p_cps_avg[ii];       
    }
    Ser_PutStringFormat("\r\nMin/Max: %d/%d\r\n", cal_data.cps_min, cal_data.cps_max);

    /* Retrieve the non-volatile storage offset for the specified wheel and direction */
    uint16 offset = NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(WHEEL_DIR_TO_CAL_DATA[params->wheel][params->direction]);
    
    /* Write the calibration to non-volatile storage */
    Nvstore_WriteBytes((uint8 *) &cal_data, sizeof(cal_data), offset);
}
  
static uint8 PerformMotorCalibration(CAL_MOTOR_PARAMS *cal_params)
{
    static char *wheel_str[2] = {"left", "right"};
    static char *direction_str[2] = {"forward", "backward"};
    static uint8 running = FALSE;
    uint8 result;
    
    if ( !running )
    {
        Ser_PutStringFormat("%s-%s Calibration\r\n", wheel_str[cal_params->wheel], direction_str[cal_params->direction]);

        Motor_SetPwm(PWM_STOP, PWM_STOP);
        InitCalibrationParams(cal_params);
        
        running = TRUE;
    }
    
    if ( running )
    {
        result = PerformMotorCalibrateAverage(cal_params);
        if (result)
        {        
            Motor_SetPwm(PWM_STOP, PWM_STOP);
            StoreMotorCalibration(cal_params);
            Ser_PutString("Complete\r\n");
            running = FALSE;
            return CALIBRATION_ITERATION_DONE;
        }
    }
    
    return CAL_OK;
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

    Ser_PutString("\r\nInitialize motor calibration\r\n");
    motor_cal_index = 0;
    Cal_ClearCalibrationStatusBit(CAL_MOTOR_BIT);
    
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
    
    Ser_PutString("\r\nPerforming motor calibration\r\n");
    Debug_Store();
    Pid_Enable(FALSE);
    Motor_SetPwm(PWM_STOP, PWM_STOP);
    
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
    params = params;
    
    uint16 motor_bit;
    CAL_MOTOR_PARAMS *cal_params = (CAL_MOTOR_PARAMS *) &motor_cal_params[motor_cal_index];
    
    motor_bit = (cal_params->wheel == WHEEL_LEFT) ? DEBUG_LEFT_MOTOR_ENABLE_BIT : DEBUG_RIGHT_MOTOR_ENABLE_BIT;

    Debug_Enable(motor_bit);

    uint8 result = PerformMotorCalibration(cal_params);
    if ( result == CALIBRATION_ITERATION_DONE )
    {
        Debug_Disable(motor_bit);
        motor_cal_index++;

        if ( motor_cal_index == NUM_MOTOR_CAL_PARAMS )
        {
            return CAL_COMPLETE;
        }
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
    
    Ser_PutString("Motor calibration complete\r\n");
    Cal_SetCalibrationStatusBit(CAL_MOTOR_BIT);
    Debug_Restore();
    
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
    params = params;
    
    Ser_PutString("\r\nPrinting motor calibration results\r\n");
    PrintAllMotorParams();
        
    return CAL_OK;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Module Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/
void CalMotor_Init()
{
    CalMotor_Calibration = &motor_calibration;
}

/* [] END OF FILE */
