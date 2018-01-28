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
//#include <math.h>
#include "calang.h"
#include "odom.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "time.h"
#include "utils.h"
#include "conserial.h"
#include "nvstore.h"
#include "debug.h"
#include "pwm.h"
#include "control.h"
#include "consts.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define ANGULAR_BIAS_ANGLE (2*PI)   // rad
#define ANGULAR_MAX_TIME (15000)
#define ANGULAR_BIAS_DIR (DIR_CW)
#define ANGULAR_BIAS_VELOCITY (0.5) // rad/s

#define TOLERANCE (0.01)

/* Calibration/Validation Interface routines */ 
static UINT8 Init();
static UINT8 Start();
static UINT8 Update();
static UINT8 Stop();
static UINT8 Results();


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static UINT32 start_time;
static UINT32 end_time;

static CALVAL_ANG_PARAMS angular_params = {ANGULAR_BIAS_DIR,
                                           ANGULAR_MAX_TIME, 
                                           0.0,
                                           ANGULAR_BIAS_ANGLE,
                                           // There is no linear velocity because we are rotating in place
                                           0.0,                
                                           ANGULAR_BIAS_DIR == DIR_CW ? -ANGULAR_BIAS_VELOCITY : ANGULAR_BIAS_VELOCITY};


static CALVAL_INTERFACE_TYPE angular_calibration = {CAL_INIT_STATE,
                                               CAL_CALIBRATE_STAGE,
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

/*---------------------------------------------------------------------------------------------------
 * Name: IsMoveFinished
 * Description: Determines if the move is finished
 * Parameters: distance - the target distance for the move. 
 *             direction - the direction of the move, i.e., CW or CCW 
 * Return: BOOL - TRUE if the move is complete; otherwise, FALSE
 *-------------------------------------------------------------------------------------------------*/
static BOOL IsMoveFinished(DIR_TYPE direction, FLOAT heading, FLOAT distance)
{
    /* Note: We are taking advantage of the fact that heading starts at zero due to Odometry reset
       prior to running the calibration.  Therefore, we only need to test when we get close to
       0 again.
    */
    
    FLOAT curr_heading;
    
    /* Unused */
    heading = heading;
    distance = distance;
    
    curr_heading = Odom_GetHeading();
    
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
 * Parameters: None 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Init()
{
    Control_OverrideDebug(TRUE);
    
    Cal_SetLeftRightVelocity(0, 0);
    Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);

    /* Clear the calibration bit before starting calibration so that we use the default value */
    Cal_ClearCalibrationStatusBit(CAL_ANGULAR_BIT);
    
    Debug_Store();
    Debug_Enable(DEBUG_ODOM_ENABLE_BIT);

    /* Note: Do we want to support both clockwise and counter clockwise calibration?  How would the
        bias be different?  How would it be applied?
        
        What if we averaged the cw and ccw biases?
        */

    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Angular Calibration");
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Place a mark on the floor corresponding to the center of one of the wheels");

    p_ang_params->distance = ANGULAR_BIAS_ANGLE;
    p_ang_params->angular = p_ang_params->direction == DIR_CW ? -ANGULAR_BIAS_VELOCITY : ANGULAR_BIAS_VELOCITY;
    
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Angular Calibration/Validation.
 * Parameters: None 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Start()
{
    FLOAT left;
    FLOAT right;

    Pid_Enable(TRUE, TRUE, FALSE);            
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();

    UniToDiff(p_ang_params->linear, p_ang_params->angular, &left, &right);    
    p_ang_params->heading = Odom_GetHeading();

    ConSer_WriteLine(TRUE, "Current Heading: %.6f", Odom_GetHeading());
    ConSer_WriteLine(TRUE, "Angular Calibration Start");            
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "\r\nCalibrating");
            
    
    Cal_SetLeftRightVelocity(left * WHEEL_COUNT_PER_RADIAN, right * WHEEL_COUNT_PER_RADIAN);            
    start_time = millis();

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: None 
 * Return: UINT8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Update()
{
    /* Note: Wait 1 second before testing, so that the heading gets a chance to change.
        We're simplifying the angle calculation by taking advantage of the fact that the 
        heading will start and end at 0.0
    */
    if (millis() - start_time < 1000)
    {
        return CAL_OK;
    }
    
    else if (millis() - start_time < p_ang_params->run_time)
    {
        if (IsMoveFinished(p_ang_params->direction, p_ang_params->heading, p_ang_params->distance))
        {
            end_time = millis();
            return CAL_COMPLETE;
        }
        
        return CAL_OK;
        
    }
    end_time = millis();
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Run time expired");

    return CAL_COMPLETE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Stop
 * Description: Calibration/Validation interface Stop function.  Called to stop calibration/validation.
 * Parameters: None 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Stop()
{
    Cal_SetLeftRightVelocity(0, 0);
    Motor_SetPwm(PWM_STOP, PWM_STOP);

    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Angular Calibration complete");

    Debug_Restore();    

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Results
 * Description: Calibration/Validation interface Results function.  Called to display calibration/validation 
 *              results. 
 * Parameters: None 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Results()
{
    FLOAT x;
    FLOAT y;
    FLOAT heading;
    FLOAT angular_bias;

    Odom_GetXYPosition(&x, &y);
    heading = Odom_GetHeading();
    angular_bias = Cal_GetAngularBias();

    ConSer_WriteLine(TRUE, "X: %.6f", x);
    ConSer_WriteLine(TRUE, "Y: %.6f", y);
    ConSer_WriteLine(TRUE, "Heading: %.6f", heading);
    ConSer_WriteLine(TRUE, "Elapsed Time: %ld", end_time - start_time);
    ConSer_WriteLine(TRUE, "Angular Bias: %.6f", angular_bias);

    ConSer_WriteLine(TRUE, "Degrees Travelled: %.6f", 360 + RADIANS_TO_DEGREES(abs(heading)));
        
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Measure the rotate traveled by the robot.");
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Enter the rotation (in degrees): ");
    FLOAT rot_in_degrees; // = Cal_ReadResponse();
    ConSer_WriteLine(TRUE, "");
    
    /* If the actual rotation is less than 360.0 then each delta is too small, i.e., lengthen delta by 360/rotation
        If the actual rotation is greater than 360.0 then each delta is too big, i.e., shorten delta by rotation/360
        */
    FLOAT bias = rot_in_degrees >= 360.0 ? 360.0 / rot_in_degrees : rot_in_degrees / 360.0;

    ConSer_WriteLine(TRUE, "New Angular Bias: %.6f", bias);
    
    Cal_SetAngularBias(bias);
    Cal_SetCalibrationStatusBit(CAL_ANGULAR_BIT);
            
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalLin_Init
 * Description: Initializes the Angular calibration module 
 * Parameters: None 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void CalAng_Init()
{
}

CALVAL_INTERFACE_TYPE* const CalAng_Start()
{
    angular_calibration.state = CAL_INIT_STATE;
    angular_calibration.stage = CAL_CALIBRATE_STAGE;
    p_ang_params = angular_calibration.params;
    return (CALVAL_INTERFACE_TYPE * const) &angular_calibration;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
