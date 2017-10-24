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

#include <math.h>
#include "control.h"
#include "callin.h"
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
#define LINEAR_DISTANCE (1.0)
#define LINEAR_VELOCITY (0.2)
#define LINEAR_MAX_TIME (20000)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static uint32 start_time;
static uint32 end_time;

static float linear_cmd_velocity;
static float angular_cmd_velocity;

static CALVAL_LIN_PARAMS linear_params = {DIR_FORWARD, 
                                       LINEAR_MAX_TIME, 
                                       LINEAR_DISTANCE,
                                       0.0,
                                       0.0};

static uint8 Init();
static uint8 Start();
static uint8 Update();
static uint8 Stop();
static uint8 Results();

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
 static void GetCommandVelocity(float *linear, float *angular, uint32 *timeout)
 {
     *linear = linear_cmd_velocity;
     *angular = angular_cmd_velocity;
     *timeout = 0;
 }
 
 
static uint8 IsMoveFinished(float * distance)
{
    float x;
    float y;
    float dist_so_far;

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
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Init()
{
    Debug_Store();
        
    p_lin_params->distance = p_lin_params->direction == DIR_FORWARD ? LINEAR_DISTANCE : -LINEAR_DISTANCE;
    p_lin_params->linear = p_lin_params->direction == DIR_FORWARD ? LINEAR_VELOCITY : -LINEAR_VELOCITY;
    p_lin_params->angular = 0.0;
    Control_SetCommandVelocityFunc(GetCommandVelocity);
    
    /* This should look just like the validation except after moving forward, we need to prompt the user to 
        enter the actual distance moved.
        Also, we need to add a linear scaler to nvram storage, initialize the scaler on startup and apply the
        scalar in the encoder.
        */
    /* Note: Do we want to support both forward and backward calibration?  How would the
        bias be different?  How would it be applied?
        */
    Ser_PutString("\r\nLinear Calibration\r\n");
    Ser_PutString("\r\nPlace a meter stick along side the robot starting centered");
    Ser_PutString("\r\non the wheel and extending toward the front of the robot\r\n");
                
    Debug_Enable(DEBUG_ODOM_ENABLE_BIT);

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Linear Calibration/Validation.
 * Parameters: None 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Start()
{
    int32 left_count;
    int32 right_count;
    float x_pos;
    float y_pos;

    Pid_Enable(TRUE, TRUE, FALSE);            
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();

    Ser_PutString("Linear Calibration Start\r\n");         
    Ser_PutString("\r\nCalibrating\r\n");
    
    /* Note: The linear bit is used to by the Encoder module to select the default linear bias or the value
        stored in EEPROM.  Therefore, because linear calibration can be an iterative process, we need to clear 
        the bit after we've reset the Encoder; otherwise, we'll pick up the default and never see the results.
        */
    Cal_ClearCalibrationStatusBit(CAL_LINEAR_BIT);

    left_count = Encoder_LeftGetCount();
    right_count = Encoder_RightGetCount();
    Odom_GetXYPosition(&x_pos, &y_pos);
    Ser_PutStringFormat("LC: %d RC: %d XP: %.3f YP: %.3f\r\n", left_count, right_count, x_pos, y_pos);

    /* Because the encoder is reset above, the left/right distances will be 0.0 and the target is just the distance */
    linear_cmd_velocity = p_lin_params->linear;
    angular_cmd_velocity = p_lin_params->angular;
    start_time = millis();

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: None 
 * Return: uint8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Update()
{
    if (millis() - start_time < p_lin_params->run_time)
    {
        if( IsMoveFinished(&p_lin_params->distance) )
        {
            end_time = millis();
            linear_cmd_velocity = 0.0;
            angular_cmd_velocity = 0.0;
            Motor_SetPwm(PWM_STOP, PWM_STOP);
            return CAL_COMPLETE;
        }
        return CAL_OK;
    }
    end_time = millis();
    Ser_PutString("\r\nRun time expired\r\n");

    return CAL_COMPLETE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Stop
 * Description: Calibration/Validation interface Stop function.  Called to stop calibration/validation.
 * Parameters: None 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Stop()
{
    Control_RestoreCommandVelocityFunc();
    Debug_Restore();    
    
    Ser_PutString("\r\nLinear Calibration complete\r\n");

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Results
 * Description: Calibration/Validation interface Results function.  Called to display validation 
 *              results. 
 * Parameters: None 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Results()
{
    float heading;
    float linear_bias;
    float x;
    float y;
    float distance;
    int32 left_count;
    int32 right_count;
    
    Odom_GetXYPosition(&x, &y);    
    heading = Odom_GetHeading();
    linear_bias = Cal_GetLinearBias();

    left_count = Encoder_LeftGetCount();
    right_count = Encoder_RightGetCount();
    Ser_PutStringFormat("X: %.6f\r\nY: %.6f\r\nDistance: %.6f\r\nLC: %d RC: %d\r\n", x, y, p_lin_params->distance, left_count, right_count);
    Ser_PutStringFormat("Heading: %.6f\r\n", heading);
    Ser_PutStringFormat("Elapsed Time: %ld\r\n", end_time - start_time);
    Ser_PutStringFormat("Linear Bias: %.6f\r\n", Cal_GetLinearBias());
    
    Ser_PutString("\r\nMeasure the distance traveled by the robot.");
    Ser_PutString("\r\nEnter the distance (0.5 to 1.5): ");
    distance = Cal_ReadResponse();
    Ser_PutString("\r\n");

    if (distance < CAL_LINEAR_BIAS_MIN || distance > CAL_LINEAR_BIAS_MAX)
    {
        Ser_PutStringFormat("The distance entered %.2f is out of the allowed range.  No change will be made.\r\n", distance);
        distance = Cal_GetLinearBias();
    }
    distance = constrain(distance, CAL_LINEAR_BIAS_MIN, CAL_LINEAR_BIAS_MAX);

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
    linear_cmd_velocity = 0.0;
    angular_cmd_velocity = 0.0;        
}

CALVAL_INTERFACE_TYPE* CalLin_Start()
{
    linear_calibration.state = CAL_INIT_STATE;
    linear_calibration.stage = CAL_CALIBRATE_STAGE;
    p_lin_params = linear_calibration.params;
    return &linear_calibration;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
