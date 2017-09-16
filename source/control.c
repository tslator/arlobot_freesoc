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
   Description: This module provides functionality for controlling the robot state.  The primary
   purpose is to receive control and speed commands and apply them.  Additionally, debugging levels
   are also handled here.
 *-------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include "control.h"
#include "motor.h"
#include "cal.h"
#include "time.h"
#include "utils.h"
#include "odom.h"
#include "debug.h"
#include "config.h"
#include "ccif.h"
#include "diag.h"


/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define MAX_CMD_VELOCITY_TIMEOUT (2000)


/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static COMMAND_FUNC_TYPE control_cmd_velocity;
static float left_cmd_velocity;
static float right_cmd_velocity;
static float linear_cmd_velocity;
static float angular_cmd_velocity;
static uint8 debug_override;

static float max_robot_forward_linear_velocity;
static float max_robot_backward_linear_velocity;

static float linear_bias;
static float angular_bias;
static float gain;
static float trim;

static float linear_odom_velocity;
static float angular_odom_velocity;
static float linear_gain;
static float angular_gain;


void Update_Debug(uint16 bits)
{
    if (bits & ENCODER_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_LEFT_ENCODER_ENABLE_BIT);
        Debug_Enable(DEBUG_RIGHT_ENCODER_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_LEFT_ENCODER_ENABLE_BIT);
        Debug_Disable(DEBUG_RIGHT_ENCODER_ENABLE_BIT);
    }
    
    if (bits & PID_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_LEFT_PID_ENABLE_BIT);
        Debug_Enable(DEBUG_RIGHT_PID_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_LEFT_PID_ENABLE_BIT);
        Debug_Disable(DEBUG_RIGHT_PID_ENABLE_BIT);
    }
    
    if (bits & MOTOR_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_LEFT_MOTOR_ENABLE_BIT);
        Debug_Enable(DEBUG_RIGHT_MOTOR_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_LEFT_MOTOR_ENABLE_BIT);
        Debug_Disable(DEBUG_RIGHT_MOTOR_ENABLE_BIT);
    }

    if (bits & ODOM_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_ODOM_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_ODOM_ENABLE_BIT);
    }
    
    if (bits & SAMPLE_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_SAMPLE_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_SAMPLE_ENABLE_BIT);
    }
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: Control_Init
 * Description: Performs initialization of module variables.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Control_Init()
{
    control_cmd_velocity = ReadCmdVelocity;
    gain = 1.0;
    trim = 0.0;
    
    linear_odom_velocity = 0.0;
    angular_odom_velocity = 0.0;
    linear_gain = 0.5;
    angular_gain = 0.5;
    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_Start
 * Description: Performs actions to activate objects that operate independently of this module.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Control_Start()
{   
    float max_robot_linear_velocity;
    float dont_care;
    
    /* Calculate the min/max forward/backward differential velocities */
    
    UniToDiff(MAX_WHEEL_FORWARD_LINEAR_VELOCITY, 0, &max_robot_linear_velocity, &dont_care);
    
    max_robot_forward_linear_velocity = max_robot_linear_velocity;
    max_robot_backward_linear_velocity = -max_robot_linear_velocity;

    linear_bias = Cal_GetLinearBias();
    angular_bias = Cal_GetAngularBias();    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_Update
 * Description: Reads and evaluates the current control state as received from the I2C module.
 *              Operations include stopping the motors, reseting odometry, clearing calibration,
 *              enabling/disabling debug and monitoring safety timer.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Control_Update()
{
    uint32 timeout;
    uint16 device_control;
    uint16 debug_control;
    
    
    CONTROL_UPDATE_START();
    
    device_control = ReadDeviceControl();
    if (device_control & CONTROL_DISABLE_MOTOR_BIT)
    {
        Motor_Stop();
    }

    /* Default debug settings are enabled/defined in config.h where the settings determine if a particular debug
       is enabled at compile time.  At runtime there are two control points for debug:
    
        1. The control module (this module) where the control interface has a facility to dynamically turning on/off
           debug.
        2. The calibration module where each calibration type controls what debug is enabled.
    
      The debug_override flag defaults to False which allows the control interface to determine what debug is enabled.
      By default the control interface disables all debug.
    
      When calibration is performed it calls Control_OverrideDebug to disable control interface control of debug.
      Control is restored when calibration terminates.
    */
    if (!debug_override)
    {
        debug_control = ReadDebugControl();
        Update_Debug(debug_control);
    }
    
    if (device_control & CONTROL_CLEAR_ODOMETRY_BIT)
    {
        Odom_Reset();
    }
    
    if (device_control & CONTROL_CLEAR_CALIBRATION)
    {
        Cal_Clear();
    }
    
    control_cmd_velocity(&linear_cmd_velocity, &angular_cmd_velocity, &timeout);
    
    //linear_cmd_velocity = 0.05;
    //angular_cmd_velocity = 0.0;
    //timeout = 0;
    Debug_Enable(DEBUG_ODOM_ENABLE_BIT);
    
    

    /* Here seems like a reasonable place to evaluate safety, e.g., can we execute the requested speed change safely
       without running into something or falling into a hole (or down stairs).
    
       The only thing in consideration at the moment is cliff sensors which are IR sensors which point at a 45 degree
       angle downward.  These will be connected to a voltage comparator and will issue an interrupt if there is a trigger.
    
       The main loop typically runs no longer than about 40ms, so if that is sufficient to detect and stop the robot
       then checking safety here seems like a good solution.  Something like:
    
       if (IsSafe())
       {
           UpdateSpeed(); 
       }

       Safety should also be proactive and stop the robot when an unsafe condition occurs.  The motors will stop after
       2 seconds but it might be better to immediately zero out the robot velocity (in the i2c module)
    
       Along these lines, hardware may be good way to handle safety concerns.  Hardware can be used to monitor actual
       inputs and generate an interrupt that can then make software calls, e.g., stop the motor and update i2c interface.
    
       It was suggested that a voltage comparator could be used to monitor voltage on a cliff sensor and directly 
       disable the PWM if the distance (voltage) exceeds a threshold - no software required.
    
    */    
    
    if (timeout > MAX_CMD_VELOCITY_TIMEOUT)
    {
        linear_cmd_velocity = 0;
        angular_cmd_velocity = 0;
    }
    
    linear_cmd_velocity = constrain(max_robot_backward_linear_velocity, linear_cmd_velocity, max_robot_forward_linear_velocity);
    angular_cmd_velocity = constrain(MAX_ROBOT_CCW_RADIAN_PER_SECOND, angular_cmd_velocity, MAX_ROBOT_CW_RADIAN_PER_SECOND);

    CONTROL_UPDATE_END();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_SetCommandVelocityFunc
 * Description: Method for setting the function from which left/right velocity values are obtained.
 *              The purpose of this function is to allow calibration to override the setting so that
 *              left/right velocity can be injected from the calibration modules.  The default method 
 *              is I2c_ReadCmdVelocity.  
 * Parameters: cmd - a pointer to a command function type
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Control_SetCommandVelocityFunc(COMMAND_FUNC_TYPE cmd)
{
    control_cmd_velocity = cmd;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_RestoreCommandVelocityFunc
 * Description: Method for restoring the default function for obtaining left/right velocity values  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Control_RestoreCommandVelocityFunc()
{
    control_cmd_velocity = ReadCmdVelocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_LeftGetCmdVelocity
 * Description: Accessor function used to return the left commanded velocity.  
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/ 
float Control_LeftGetCmdVelocity()
{
    return left_cmd_velocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_RightGetCmdVelocity
 * Description: Accessor function used to return the right commanded velocity.  
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/ 
float Control_RightGetCmdVelocity()
{
    return right_cmd_velocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_LinearGetCmdVelocity
 * Description: Accessor function used to return the linear commanded velocity.  
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/ 
float Control_LinearGetCmdVelocity()
{
    return linear_cmd_velocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_AngularGetCmdVelocity
 * Description: Accessor function used to return the angular commanded velocity.  
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/ 
float Control_AngularGetCmdVelocity()
{
    return angular_cmd_velocity;
}

void Control_GetCmdVelocity(float *linear, float *angular)
{
    *linear = linear_cmd_velocity;
    *angular = angular_cmd_velocity;
}

void Control_SetCmdVelocity(float linear, float angular)
{
    linear_cmd_velocity = linear;
    angular_cmd_velocity = angular;

    UniToDiff(linear, angular, &left_cmd_velocity, &right_cmd_velocity);
    
    left_cmd_velocity = constrain(MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, left_cmd_velocity, MAX_WHEEL_FORWARD_LINEAR_VELOCITY);
    right_cmd_velocity = constrain(MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, right_cmd_velocity,MAX_WHEEL_FORWARD_LINEAR_VELOCITY);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_OverrideDebug
 * Description: Function used to override the debug mask.  Used primarily during calibration.
 * Parameters: override : boolean - used to disable the debug mask settings specified via I2C.
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Control_OverrideDebug(uint8 override)
{
    debug_override = override;
}

void Control_SetDeviceStatusBit(uint16 bit)
{
    SetDeviceStatusBit(bit);
}

void Control_ClearDeviceStatusBit(uint16 bit)
{
    ClearDeviceStatusBit(bit);
}

void Control_SetCalibrationStatus(uint16 status)
{
    SetCalibrationStatus(status);
}

void Control_SetCalibrationStatusBit(uint16 bit)
{
    SetCalibrationStatusBit(bit);
}

void Control_ClearCalibrationStatusBit(uint16 bit)
{
    ClearCalibrationStatusBit(bit);
}

void Control_WriteOdom(float linear, 
                       float angular, 
                       float x_position, 
                       float y_position, 
                       float heading)
{
    linear_odom_velocity = linear;
    angular_odom_velocity = angular;
    
    WriteSpeed(linear, angular);
    WritePosition(x_position, y_position);
    WriteHeading(heading);
}

void Control_UpdateHeartbeat(uint32 heartbeat)
{
    UpdateHeartbeat(heartbeat);
}

/* [] END OF FILE */
