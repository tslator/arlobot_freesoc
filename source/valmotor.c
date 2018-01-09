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

    Calibration involves mapping count/sec values to PWM values.  Each motor is run (forward and 
    backward) collecting an average count/sec for each PWM sample.  The results are stored in a 
    structure (CAL_DATA_TYPE) that holds the count/sec averages, PWM samples, min/max count/sec.  
    There are four CAL_DATA_TYPE instances: one for left motor forward, left motor backward, right 
    motor forward and right motor backward.  The calibration data is stored in NVRAM on the Psoc.
    
    Upon startup, the calibration data is made available to the motors (Note: the EEPROM component 
    maps NVRAM to memory so there is no appreciable overhead in reading from NVRAM.

 */

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <limits.h>
#include "control.h"
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

#include "pidleft.h"
#include "pidright.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define VAL_NUM_PROFILE_DATA_POINTS (13)
#define NUM_MOTOR_VAL_PARAMS (4)

#define LEFT_FORWARD_INDEX (0)
#define RIGHT_FORWARD_INDEX (1)
#define LEFT_BACKWARD_INDEX (2)
#define RIGHT_BACKWARD_INDEX (3)

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
    UINT16 run_time;
    FLOAT *p_cps;
    SET_MOTOR_PWM_FUNC_TYPE set_pwm;
    GET_MOTOR_PWM_FUNC_TYPE get_pwm;
    RAMP_DOWN_PWM_FUNC_TYPE ramp_down;
    GET_ENCODER_FUNC_TYPE   get_mps;
    GET_ENCODER_FUNC_TYPE   get_cps;
    SET_LEFT_RIGHT_VELOCITY_TYPE set_velocity;
} VAL_MOTOR_PARAMS;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/

static FLOAT val_fwd_cps[VAL_NUM_PROFILE_DATA_POINTS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static FLOAT val_bwd_cps[VAL_NUM_PROFILE_DATA_POINTS] = {-0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0};


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

VAL_MOTOR_PARAMS motor_val_params[NUM_MOTOR_VAL_PARAMS] = 
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
            Control_SetLeftRightVelocityCps
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
            Control_SetLeftRightVelocityCps
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
            Control_SetLeftRightVelocityCps
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
            Control_SetLeftRightVelocityCps
        }
    };

static UINT8 motor_val_index;
static VAL_MOTOR_PARAMS *val_params;

static UINT8 motor_val_sequence_index;
static UINT8 motor_val_sequence_end;
static UINT8 motor_val_sequence[NUM_MOTOR_VAL_PARAMS];
static UINT8 num_profile_data_points;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Print Functions
 *-------------------------------------------------------------------------------------------------*/

void PrintWheelVelocity(FLOAT cps)
{    
    Ser_PutStringFormat("{\"calc cps\":%.3f,\"meas cps\":%.3f,\"diff\":%.3f, \"%% diff\":%.3f}\r\n", 
        cps, val_params->get_cps(), cps - val_params->get_cps(), 100.0 * (cps - val_params->get_cps())/cps);
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

static UINT8 GetNextCps(FLOAT *cps)
/* Return the next value in the array and increment the index
   Return 0 if the index is in range 0 to N-1
   Return 1 if the index rolls over
   Note: Allow rollover so that the index auto-initializes
 */
{
    static UINT8 index = 0;
    
    if (index == num_profile_data_points)
    {
        index = 0;
        return 1;
    }

    switch (val_params->direction)
    {
        case DIR_FORWARD:
            *cps = val_params->p_cps[index];
            break;
            
        case DIR_BACKWARD:
            *cps = val_params->p_cps[index];
            break;
            
        default:
            *cps = 0.0;
            break;
    }
    
    index++;
    
    return 0;
}

static void SetNextVelocity(FLOAT cps)
{
    if (val_params->wheel == WHEEL_LEFT)
    {
        val_params->set_velocity(cps, 0);
    }
    if (val_params->wheel == WHEEL_RIGHT)
    {
        val_params->set_velocity(0, cps);
    }
}

static UINT8 PerformMotorValidation()
{
    static UINT8 running = FALSE;
    static UINT32 start_time = 0;
    static FLOAT cps;
    UINT8 result;

    if( !running )
    {
        val_params->set_velocity(0, 0);
        result = GetNextCps(&cps);
        if( result )
        {
            val_params->set_pwm(PWM_STOP);
            return CAL_COMPLETE;
        }
        SetNextVelocity(cps);
        start_time = millis();
        running = TRUE;
    }

    if( running )
    {
        if ( millis() - start_time < val_params->run_time )
        {
            return CAL_OK;
        }
        PrintWheelVelocity(cps);
        
        /* Note: Make GetNextCps reset the index back to 0 when it reaches the end of the array
           so we don't have to explicitly reset the index.
         
           Consider doing the same for the PID -- auto reset, it's better that way
         
         */
        result = GetNextCps(&cps);
        if( result )
        {
            running = FALSE;
            val_params->set_velocity(0, 0);
            return INTERATION_DONE;
        }
        SetNextVelocity(cps);
        start_time = millis();
        return CAL_OK;
    }
    
    return CAL_OK;
}

static void InitValParams(WHEEL_TYPE wheel, DIR_TYPE direction)
{
    /* If wheel is left and direction is forward then choose index 0
       If wheel is right and direction is forward then choose index 1
       If wheel is left and direction is backward then choose index 2
       If wheel is right and direction is backward then choose index 3

       If wheel is both and direction is forward then choose indeces 0 and 1
       If wheel is both and direction is backward then choose indeces 2 and 3
       If wheel is left and direction is both then choose indeces 0 and 2
       If wheel is right and direction is both then choose indeces 1 and 3

       If wheel is both and direction is both then choose indeces 1 through 3
    */

    motor_val_sequence_index = 0;
    memset(motor_val_sequence, 0, sizeof motor_val_sequence);

    if (wheel == WHEEL_LEFT && direction == DIR_FORWARD)
    {
        motor_val_sequence[0] = LEFT_FORWARD_INDEX;
        motor_val_sequence_end = 1;
    }
    else if (wheel == WHEEL_RIGHT && direction == DIR_FORWARD)
    {
        motor_val_sequence[0] = RIGHT_FORWARD_INDEX;        
        motor_val_sequence_end = 1;
    }
    else if (wheel == WHEEL_LEFT && direction == DIR_BACKWARD)
    {
        motor_val_sequence[0] = LEFT_BACKWARD_INDEX;
        motor_val_sequence_end = 1;
    }
    else if (wheel == WHEEL_RIGHT && direction == DIR_BACKWARD)
    {
        motor_val_sequence[0] = RIGHT_BACKWARD_INDEX;
        motor_val_sequence_end = 1;
    }
    else if (wheel == WHEEL_BOTH && direction == DIR_FORWARD)
    {
        motor_val_sequence[0] = LEFT_FORWARD_INDEX;        
        motor_val_sequence[1] = RIGHT_FORWARD_INDEX;        
        motor_val_sequence_end = 2;
    }
    else if (wheel == WHEEL_BOTH && direction == DIR_BOTH)
    {
        motor_val_sequence[0] = LEFT_FORWARD_INDEX;        
        motor_val_sequence[1] = RIGHT_FORWARD_INDEX;        
        motor_val_sequence[2] = LEFT_BACKWARD_INDEX;        
        motor_val_sequence[3] = RIGHT_BACKWARD_INDEX;        
        motor_val_sequence_end = 4;
    }

    motor_val_index = motor_val_sequence[motor_val_sequence_index];
    val_params = (VAL_MOTOR_PARAMS *) &motor_val_params[motor_val_index];
}

static UINT8 NextValParams(void)
{
    motor_val_sequence_index++;
    motor_val_index = motor_val_sequence[motor_val_sequence_index];
    val_params = (VAL_MOTOR_PARAMS *) &motor_val_params[motor_val_index];

    if (motor_val_sequence_index == motor_val_sequence_end )
    {
        return VAL_COMPLETE;
    }

    return VAL_OK;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Module Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/
void ValMotor_Init(WHEEL_TYPE wheel, DIR_TYPE direction, FLOAT min_percent, FLOAT max_percent, UINT8 num_points)
{    
    InitValParams(wheel, direction);

    num_profile_data_points = num_points;

    Cal_CalcTriangularProfile(num_profile_data_points, 
                              min_percent, 
                              max_percent, 
                              val_fwd_cps,
                              val_bwd_cps);

    
    Motor_SetPwm(PWM_STOP, PWM_STOP);
    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: stage - the calibration/validation stage 
 *             params - motor validation parameters, e.g. direction, run time, etc. 
 * Return: UINT8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT8 ValMotor_Update()
{
    
    /* Here is where we can decide what validation to do.  The validation menu has one option for
       motor validation.  I think that is find, but it should run both motors forwards and backwards.
    
       Why isn't that happening?
    */

    UINT8 result = PerformMotorValidation();
    if( result == INTERATION_DONE )
    {
        result = NextValParams();
    }
    
    return result;
}

/* [] END OF FILE */
