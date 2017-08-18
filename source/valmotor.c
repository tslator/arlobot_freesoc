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
#include "valmotor.h"
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
#define VAL_LOWER_BOUND_PERCENTAGE (0.2)
#define VAL_UPPER_BOUND_PERCENTAGE (0.8)
#define VAL_NUM_PROFILE_DATA_POINTS (11)
#define VALIDATION_INTERATION_DONE (255)
#define MAX_CPS_ARRAY (51)
#define NUM_MOTOR_VAL_PARAMS (1)


/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef struct 
{
    char label[30]; 
    DIR_TYPE direction;
    WHEEL_TYPE wheel;
    uint16 run_time;
    float *p_cps;
    SET_MOTOR_PWM_FUNC_TYPE set_pwm;
    GET_MOTOR_PWM_FUNC_TYPE get_pwm;
    RAMP_DOWN_PWM_FUNC_TYPE ramp_down;
    GET_ENCODER_FUNC_TYPE   get_mps;
    GET_ENCODER_FUNC_TYPE   get_cps;
    SET_LEFT_RIGHT_VELOCITY_TYPE set_velocity;
}VAL_MOTOR_PARAMS;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/

static float val_fwd_cps[VAL_NUM_PROFILE_DATA_POINTS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static float val_bwd_cps[VAL_NUM_PROFILE_DATA_POINTS] = {-0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0};

/* Provides an implementation of the Calibration interface */
static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static CALIBRATION_TYPE motor_validation = {CAL_INIT_STATE, 
                                            CAL_VALIDATE_STAGE,
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
//PWM_PARAMS_TYPE pwm_params[2][2] = {{{LEFT_PWM_STOP, LEFT_PWM_FULL_FORWARD, LEFT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1)}, 
//                                     {LEFT_PWM_FULL_BACKWARD, LEFT_PWM_STOP, LEFT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1)}
//                                    }, 
//                                    {
//                                     {RIGHT_PWM_STOP, RIGHT_PWM_FULL_FORWARD, -(RIGHT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1))}, 
//                                     {RIGHT_PWM_FULL_BACKWARD, RIGHT_PWM_STOP, -(RIGHT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1))}
//                                    }};

VAL_MOTOR_PARAMS motor_val_params[4] = 
    {
        {
            "left-forward", 
            DIR_FORWARD,
            WHEEL_LEFT,
            2000,
            &val_fwd_cps[0],
            Motor_LeftSetPwm,
            Motor_LeftGetPwm,
            Motor_LeftRampDown,
            Encoder_LeftGetMeterPerSec,
            Encoder_LeftGetCntsPerSec,
            Cal_SetLeftRightVelocity,
        }, 
        {
            "right-forward",
            DIR_FORWARD,
            WHEEL_RIGHT,
            2000,
            &val_fwd_cps[0],
            Motor_RightSetPwm,
            Motor_RightGetPwm,
            Motor_RightRampDown,
            Encoder_RightGetMeterPerSec,
            Encoder_RightGetCntsPerSec, 
            Cal_SetLeftRightVelocity
        },
        {
            "left-backward",
            DIR_BACKWARD,
            WHEEL_LEFT,
            2000,
            &val_bwd_cps[0],
            Motor_LeftSetPwm,
            Motor_LeftGetPwm,
            Motor_LeftRampDown,
            Encoder_LeftGetMeterPerSec,
            Encoder_LeftGetCntsPerSec,
            Cal_SetLeftRightVelocity,
        }, 
        {
            "right-backward",
            DIR_BACKWARD,
            WHEEL_RIGHT,
            2000,
            &val_bwd_cps[0],
            Motor_RightSetPwm,
            Motor_RightGetPwm,
            Motor_RightRampDown,
            Encoder_RightGetMeterPerSec,
            Encoder_RightGetCntsPerSec,
            Cal_SetLeftRightVelocity
        }
    };

static uint8 motor_val_index;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Print Functions
 *-------------------------------------------------------------------------------------------------*/

void PrintWheelVelocity(VAL_MOTOR_PARAMS *val_params, float cps)
{
    Ser_PutStringFormat("CPS: %.3f %.3f\r\n", cps, val_params->get_cps());
}

/*---------------------------------------------------------------------------------------------------
 * Name: PrintMotorValidationResults
 * Description: Prints the left/right, forward/backward count/sec and pwm calibration values
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void PrintMotorValidationResults()
{
}

static uint8 GetNextCps(VAL_MOTOR_PARAMS *params, float *cps)
/* Return the next value in the array and increment the index
   Return 0 if the index is in range 0 to N-1
   Return 1 if the index rolls over
   Note: Allow rollover so that the index auto-initializes
 */
{
    static uint8 index = 0;
    
    if (index == VAL_NUM_PROFILE_DATA_POINTS)
    {
        index = 0;
        return 1;
    }

    switch (params->direction)
    {
        case DIR_FORWARD:
            *cps = params->p_cps[index];
            break;
            
        case DIR_BACKWARD:
            *cps = params->p_cps[index];
            break;
            
        default:
            *cps = 0.0;
            break;
    }
    
    index++;
    
    return 0;
}

static void SetNextVelocity(VAL_MOTOR_PARAMS *params, float cps)
{
    float mps = cps * WHEEL_METER_PER_COUNT;
    Ser_PutStringFormat("SetNextVelocity: cps: %.3f, mps: %.3f\r\n", cps, mps);
    
    if (params->wheel == WHEEL_LEFT)
    {
        params->set_velocity(mps, 0);
    }
    if (params->wheel == WHEEL_RIGHT)
    {
        params->set_velocity(0, mps);
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
static void InitValidationParams(VAL_MOTOR_PARAMS *params)
{
    //uint8 ii;
    //uint16 pwm;
    //uint16 pwm_start;
    //int16 pwm_step;
    //WHEEL_TYPE wheel;
    //DIR_TYPE dir;
     
    params = params;
    
    //wheel = params->wheel;
    //dir = params->direction;
        
    //pwm_start = pwm_params[wheel][dir].start;
    //pwm_step = pwm_params[wheel][dir].step;
     
    /* The pwm samples are filled in from lowest value to highest PWM value.  This ensures that the
       corresponding count/sec values (stored in a different array) are ordered from lowest value to
       highest value, i.e., forward: 0 to max cps, reverse: -max cps to 0.
    */
    //for (ii =0, pwm = pwm_start; ii < CAL_NUM_SAMPLES; ++ii, pwm += pwm_step)
    //{
    //}
  
    /* The first pwm entry is always PWM_STOP and it must correspond to CPS value 0 in order to stop the motor.  So,
       So, set pwm_index to start at 1.
    */
}

static uint8 PerformMotorValidation(VAL_MOTOR_PARAMS *val_params)
{
    static uint8 running = FALSE;
    static uint32 start_time = 0;
    static float cps;
    uint8 result;

    if( !running )
    {
        val_params->set_velocity(0, 0);
        result = GetNextCps(val_params, &cps);
        if( result )
        {
            val_params->set_pwm(PWM_STOP);
            return CAL_COMPLETE;
        }
        Ser_PutStringFormat("Setting velocity: %.3f\r\n", cps);
        SetNextVelocity(val_params, cps);
        start_time = millis();
        running = TRUE;
    }

    if( running )
    {
        if ( millis() - start_time < val_params->run_time )
        {
            return CAL_OK;
        }
        PrintWheelVelocity(val_params, cps);
        
        /* Note: Make GetNextCps reset the index back to 0 when it reaches the end of the array
           so we don't have to explicitly reset the index.
         
           Consider doing the same for the PID -- auto reset, it's better that way
         
         */
        result = GetNextCps(val_params, &cps);
        if( result )
        {
            running = FALSE;
            val_params->set_velocity(0, 0);
            return VALIDATION_INTERATION_DONE;
        }
        SetNextVelocity(val_params, cps);
        start_time = millis();
        return CAL_OK;
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
    //uint8 ii;
    params = params;    

    Ser_PutString("\r\nInitialize motor validation\r\n");

    Cal_CalcTriangularProfile(VAL_NUM_PROFILE_DATA_POINTS, 
                              VAL_LOWER_BOUND_PERCENTAGE, 
                              VAL_UPPER_BOUND_PERCENTAGE, 
                              val_fwd_cps, 
                              val_bwd_cps);

    /* Left/Right wheel validation uses the main loop so the PID must be enabled */
    Pid_Enable(TRUE);
    Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);
    
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
    
    Ser_PutString("\r\nPerforming motor validation\r\n");
    Cal_SetLeftRightVelocity(0, 0);            
    Debug_Store();
    Debug_Enable(DEBUG_LEFT_ENCODER_ENABLE_BIT);// | DEBUG_LEFT_MOTOR_ENABLE_BIT | DEBUG_RIGHT_ENCODER_ENABLE_BIT);
    
    
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
    
    
    VAL_MOTOR_PARAMS *val_params = &motor_val_params[motor_val_index];

    uint8 result = PerformMotorValidation(val_params);
    if( result == VALIDATION_INTERATION_DONE )
    {
        motor_val_index++;
        if( motor_val_index == NUM_MOTOR_VAL_PARAMS )
        {
            val_params->set_velocity(0, 0);            
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
    
    Ser_PutString("Motor validation complete\r\n");
    Cal_SetLeftRightVelocity(0, 0);
    Pid_RestoreLeftRightTarget();
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

    Ser_PutString("\r\nPrinting motor validation results\r\n");

    return CAL_OK;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Module Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/
void ValMotor_Init()
{    
    ValMotor_Validation = &motor_validation;
}

/* [] END OF FILE */
