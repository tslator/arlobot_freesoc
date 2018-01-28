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
   Description: This module provides the implementation for linear calibration.
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * The purpose of linear validation will be move the robot in the forward/backward direction,
 * capture the odometry, and compare the results with the expected distance traveled.  The
 * goal is to see the robot moving in a straight line.
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/

#include "control.h"
#include "callin.h"
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

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define LINEAR_DISTANCE (1.0)
#define LINEAR_VELOCITY (0.2)
#define LINEAR_MAX_TIME (20000)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static UINT32 start_time;
static UINT32 end_time;

static CALVAL_LIN_PARAMS linear_params = {DIR_FORWARD, 
                                       LINEAR_MAX_TIME, 
                                       LINEAR_DISTANCE,
                                       0.0,
                                       0.0};

static UINT8 Init();
static UINT8 Start();
static UINT8 Update();
static UINT8 Stop();
static UINT8 Results();

static CALVAL_INTERFACE_TYPE linear_calibration = {CAL_INIT_STATE,
                                              CAL_CALIBRATE_STAGE,
                                              &linear_params,
                                              Init,
                                              Start,
                                              Update,
                                              Stop,
                                              Results};

static CALVAL_LIN_PARAMS *p_lin_params;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static void GetCommandVelocity(FLOAT* const linear, FLOAT* const angular, UINT32 *timeout)
{
    *linear = p_lin_params->linear;
    *angular = p_lin_params->angular;
    *timeout = 0;
}
 
 
static UINT8 IsMoveFinished(FLOAT* const distance)
{
    FLOAT x;
    FLOAT y;
    FLOAT dist_so_far;

    Odom_GetXYPosition(&x, &y);

    dist_so_far = sqrt(pow(x, 2) + pow(y, 2));
    
    if ( dist_so_far < *distance )
    {
        return FALSE;
    }

    *distance = dist_so_far;
    return TRUE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Init
 * Description: Calibration/Validation interface Init function.  Performs initialization for Linear 
 *              Calibration/Validation.
 * Parameters: None 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Init()
{
    Control_OverrideDebug(TRUE);
    
    Debug_Store();
        
    p_lin_params->distance = p_lin_params->direction == DIR_FORWARD ? LINEAR_DISTANCE : -LINEAR_DISTANCE;
    p_lin_params->linear = p_lin_params->direction == DIR_FORWARD ? LINEAR_VELOCITY : -LINEAR_VELOCITY;
    p_lin_params->angular = 0.0;
    Control_SetCommandVelocityFunc(GetCommandVelocity);

    /* Clear the calibration bit before starting calibration so that we use the default value */
    Cal_ClearCalibrationStatusBit(CAL_ANGULAR_BIT);
        
    /* This should look just like the validation except after moving forward, we need to prompt the user to 
        enter the actual distance moved.
        Also, we need to add a linear scaler to nvram storage, initialize the scaler on startup and apply the
        scalar in the encoder.
        */
    /* Note: Do we want to support both forward and backward calibration?  How would the
        bias be different?  How would it be applied?
        */
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Linear Calibration");
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Place a meter stick along side the robot starting centered");
    ConSer_WriteLine(TRUE, "on the wheel and extending toward the front of the robot");
                
    Debug_Enable(DEBUG_ODOM_ENABLE_BIT);

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Linear Calibration/Validation.
 * Parameters: None 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Start()
{
    INT32 left_count;
    INT32 right_count;
    FLOAT x_pos;
    FLOAT y_pos;

    Pid_Enable(TRUE, TRUE, FALSE);            
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();

    ConSer_WriteLine(TRUE, "Linear Calibration Start");
    ConSer_WriteLine(TRUE, "Calibrating");
    
    left_count = Encoder_LeftGetCount();
    right_count = Encoder_RightGetCount();
    Odom_GetXYPosition(&x_pos, &y_pos);
    ConSer_WriteLine(TRUE, "LC: %d RC: %d XP: %.3f YP: %.3f", left_count, right_count, x_pos, y_pos);

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
    if (millis() - start_time < p_lin_params->run_time)
    {
        if( IsMoveFinished(&p_lin_params->distance) )
        {
            end_time = millis();
            Motor_SetPwm(PWM_STOP, PWM_STOP);
            p_lin_params->linear = 0.0;
            p_lin_params->angular = 0.0;
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
    Control_RestoreCommandVelocityFunc();
    Debug_Restore();    
    
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Linear Calibration complete");

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Results
 * Description: Calibration/Validation interface Results function.  Called to display validation 
 *              results. 
 * Parameters: None 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Results()
{
    FLOAT heading;
    FLOAT linear_bias;
    FLOAT x;
    FLOAT y;
    FLOAT distance;
    INT32 left_count;
    INT32 right_count;
    
    Odom_GetXYPosition(&x, &y);    
    heading = Odom_GetHeading();
    linear_bias = Cal_GetLinearBias();

    left_count = Encoder_LeftGetCount();
    right_count = Encoder_RightGetCount();
    ConSer_WriteLine(TRUE, "X: %.6f", x);
    ConSer_WriteLine(TRUE, "Y: %.6f", y);
    ConSer_WriteLine(TRUE, "Distance: %.6f", p_lin_params->distance);
    ConSer_WriteLine(TRUE, "LC: %d RC: %d", left_count, right_count);
    ConSer_WriteLine(TRUE, "Heading: %.6f", heading);
    ConSer_WriteLine(TRUE, "Elapsed Time: %ld", end_time - start_time);
    ConSer_WriteLine(TRUE, "Linear Bias: %.6f", linear_bias);
    
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Measure the distance traveled by the robot.");
    ConSer_WriteLine(TRUE, "Enter the distance (0.5 to 1.5): ");
    //distance = Cal_ReadResponse();

    if (distance < CAL_LINEAR_BIAS_MIN || distance > CAL_LINEAR_BIAS_MAX)
    {
        ConSer_WriteLine(TRUE, "The distance entered %.2f is out of the allowed range.  No change will be made.", distance);
        distance = linear_bias;
    }

    Cal_ClearCalibrationStatusBit(CAL_LINEAR_BIT);    
    Cal_SetLinearBias(distance);
    Cal_SetCalibrationStatusBit(CAL_LINEAR_BIT);
        
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalLin_Init
 * Description: Initializes the Linear calibration module 
 * Parameters: None 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void CalLin_Init()
{
}

CALVAL_INTERFACE_TYPE* const CalLin_Start()
{
    linear_calibration.state = CAL_INIT_STATE;
    linear_calibration.stage = CAL_CALIBRATE_STAGE;
    p_lin_params = linear_calibration.params;
    return (CALVAL_INTERFACE_TYPE* const) &linear_calibration;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
