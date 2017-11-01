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
   Description: This module provides the implementation for angular calibration.
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <math.h>
#include "control.h"
#include "valang.h"
#include "odom.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "time.h"
#include "utils.h"
#include "serial.h"
#include "nvstore.h"
#include "debug.h"
#include "pwm.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define ANGULAR_BIAS_ANGLE (2*PI)   // rad
#define ANGULAR_MAX_TIME (15000)
#define ANGULAR_BIAS_DIR (DIR_CW)
#define ANGULAR_BIAS_VELOCITY (0.5) // rad/s


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static uint32 start_time;
static uint32 end_time;

static CALVAL_ANG_PARAMS angular_params = {ANGULAR_BIAS_DIR,
                                        ANGULAR_MAX_TIME, 
                                        0.0,
                                        ANGULAR_BIAS_ANGLE,
                                        // There is no linear velocity because we are rotating in place
                                        0.0,                
                                        ANGULAR_BIAS_DIR == DIR_CW ? -ANGULAR_BIAS_VELOCITY : ANGULAR_BIAS_VELOCITY};

static uint8 Init();
static uint8 Start();
static uint8 Update();
static uint8 Stop();
static uint8 Results();

static CALVAL_INTERFACE_TYPE angular_validation = {CAL_INIT_STATE,
                                               CAL_VALIDATE_STAGE,
                                               &angular_params,
                                               Init,
                                               Start,
                                               Update,
                                               Stop,
                                               Results};



static CALVAL_ANG_PARAMS *p_ang_params;
                                               
/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static void GetCommandVelocity(float *linear, float *angular, uint32 *timeout)
{
    *linear = p_ang_params->linear;
    *angular = p_ang_params->angular;
    *timeout = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: IsMoveFinished
 * Description: Determines if the move is finished
 * Parameters: direction - the direction of the move, i.e., CW or CCW 
 * Return: uint8 - TRUE if the move is complete; otherwise, FALSE
 *-------------------------------------------------------------------------------------------------*/
static uint8 IsMoveFinished(DIR_TYPE direction)
{
    #define TOLERANCE (0.01)

    /* Note: We are taking advantage of the fact that heading starts at zero due to Odometry reset
       prior to running the calibration.  Therefore, we only need to test when we get close to
       0 again.
    */
    
    float curr_heading = Odom_GetHeading();
    
    if (direction == DIR_CCW)
    {
        if (curr_heading < 0.0)
        {
            return FALSE;
        }
        else
        {
            if (curr_heading >= -TOLERANCE && curr_heading <= TOLERANCE)
            {
                return TRUE;
            }

            return FALSE;
        }
    }

    else if (direction == DIR_CW)
    {
        if (curr_heading > 0.0)
        {
            return FALSE;
        }
        else
        {
            if (curr_heading >= -TOLERANCE && curr_heading <= TOLERANCE)
            {
                return TRUE;
            }

            return FALSE;
        }                
    }

    return FALSE;
}


/*---------------------------------------------------------------------------------------------------
 * Name: Init
 * Description: Calibration/Validation interface Init function.  Performs initialization for Angular 
 *              Calibration/Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Init()
{
    Debug_Store();
    
    Control_SetCommandVelocityFunc(GetCommandVelocity);    
    p_ang_params->distance = ANGULAR_BIAS_ANGLE;
    p_ang_params->linear = 0.0;
    p_ang_params->angular = p_ang_params->direction == DIR_CW ? -ANGULAR_BIAS_VELOCITY : ANGULAR_BIAS_VELOCITY;

    Ser_PutStringFormat("\r\n%s Angular validation\r\n", 
                        p_ang_params->direction == DIR_CW ? "Clockwise" : "Counter Clockwise");
    
    Debug_Enable(DEBUG_ODOM_ENABLE_BIT);

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Angular Calibration/Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Start()
{
    Pid_Enable(TRUE, TRUE, FALSE);            
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();

    Ser_PutString("Angular Validation Start\r\n");            
    Ser_PutString("\r\nValidating\r\n");

    start_time = millis();

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: stage - the calibration/validation stage 
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Update()
{
    static uint8 delay_completed = FALSE;
    
    if (!delay_completed)
    {
        /* Note: Wait 1 second before testing, so that the heading gets a chance to change; otherwise,
           heading will still be 0.0.  We're simplifying the angle calculation by taking advantage of the 
           fact that the heading will start and end at 0.0
        */
        if (millis() - start_time < 1000)
        {
            return CAL_OK;
        }
        
        delay_completed = TRUE;
        start_time = millis();
    }
    
    if (millis() - start_time < p_ang_params->run_time)
    {
        if (IsMoveFinished(p_ang_params->direction))
        {
            end_time = millis();
            Motor_SetPwm(PWM_STOP, PWM_STOP);
            p_ang_params->linear = 0.0;
            p_ang_params->angular = 0.0;            
            delay_completed = FALSE;
            return CAL_COMPLETE;
        }        
        return CAL_OK;        
    }
    end_time = millis();
    delay_completed = FALSE;
    Ser_PutString("\r\nRun time expired\r\n");
    
    return CAL_COMPLETE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Stop
 * Description: Calibration/Validation interface Stop function.  Called to stop calibration/validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Stop()
{
    Control_RestoreCommandVelocityFunc();
    Debug_Restore();    

    Ser_PutStringFormat("\r\n%s Angular Validation complete\r\n", 
                        p_ang_params->direction == DIR_CW ? "clockwise" : "counter clockwise");

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Results
 * Description: Calibration/Validation interface Results function.  Called to display calibration/validation 
 *              results. 
 * Parameters: stage - the calibration/validation stage (validation only) 
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Results()
{
    float x;
    float y;
    float heading;
    float angular_bias;

    Odom_GetXYPosition(&x, &y);
    heading = Odom_GetHeading();
    angular_bias = Cal_GetAngularBias();

    Ser_PutStringFormat("X: %.6f\r\nY: %.6f\r\n", x, y);
    Ser_PutStringFormat("Heading: %.6f\r\n", heading);
    Ser_PutStringFormat("Elapsed Time: %ld\r\n", end_time - start_time);
    Ser_PutStringFormat("Angular Bias: %.6f\r\n", angular_bias);
        
    Ser_PutString("\r\nPrinting Angular validation results\r\n");

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalLin_Init
 * Description: Initializes the Angular calibration module 
 * Parameters: None 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void ValAng_Init()
{
}

CALVAL_INTERFACE_TYPE *ValAng_Start(DIR_TYPE dir)
{
    angular_validation.state = CAL_INIT_STATE;
    angular_validation.stage = CAL_VALIDATE_STAGE;
    p_ang_params = angular_validation.params;
    p_ang_params->direction = dir;
    return (CALVAL_INTERFACE_TYPE *) &angular_validation;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
