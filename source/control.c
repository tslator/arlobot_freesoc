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
#include "ccif.h"
#include "diag.h"
#include "consts.h"

/*---------------------------------------------------------------------------------------------------
 * Defines
 *-------------------------------------------------------------------------------------------------*/
//#define ENABLE_VELOCITY_REPEAT    
//#define OVERRIDE_VELOCITY

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define MAX_CMD_VELOCITY_TIMEOUT (2000)

#define LINEAR_RESPONSE_TIME (0.1)
#define ANGULAR_RESPONSE_TIME (0.1)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static COMMAND_FUNC_TYPE control_cmd_velocity;
static FLOAT linear_velocity_mps;
static FLOAT angular_velocity_rps;
static FLOAT left_velocity_cps;
static FLOAT right_velocity_cps;
static FLOAT max_robot_forward_linear_velocity_mps;     // m/s
static FLOAT max_robot_backward_linear_velocity_mps;    // m/s

static FLOAT max_linear;  // m/s
static FLOAT max_angular; // r/s

static BOOL debug_override;
static UINT8 left_right_cmd_velocity_override;

static FLOAT linear_gain;
static FLOAT linear_trim;


/*---------------------------------------------------------------------------------------------------
 * Name: Update_Debug
 * Description: Enables/Disables debug based on the specified bits.
 * Parameters: bits - contains bits corresponding to the supported debug, e.g., encoder, pid, motor, etc
 *                    1 indicates debug is enabled, 0 indicates debug is disabled
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Update_Debug(UINT16 bits)
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
    linear_gain = 1.0;
    linear_trim = 0.0;
    left_right_cmd_velocity_override = FALSE;
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
    FLOAT max_robot_linear_velocity;
    FLOAT dont_care;
    
    /* Calculate the min/max forward/backward differential velocities */
    DiffToUni(MAX_WHEEL_METER_PER_SECOND, MAX_WHEEL_METER_PER_SECOND, &max_linear, &dont_care);
    DiffToUni(MAX_WHEEL_METER_PER_SECOND, -MAX_WHEEL_METER_PER_SECOND, &dont_care, &max_angular);
    
    UniToDiff(MAX_WHEEL_FORWARD_LINEAR_VELOCITY, 0, &max_robot_linear_velocity, &dont_care);
    
    max_robot_forward_linear_velocity_mps = max_robot_linear_velocity;
    max_robot_backward_linear_velocity_mps = -max_robot_linear_velocity;

}

#ifdef ENABLE_VELOCITY_REPEAT
static void RepeatVelocity(FLOAT* const linear, FLOAT* const angular, UINT32* const timeout)
{
    static UINT8 select = 0;
    static UINT32 last_time = 0;
    
    *timeout = 0;
    if (millis() - last_time < 5000)
    {
    }
    else
    {
        last_time = millis();
        select ^= 1;
    }
    
    *linear = (select == 0) ? 0.3 : -0.3;
    *angular = (select == 0) ? 0.0 : -0.0;
}
#endif

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
    UINT32 timeout;
    UINT16 device_control;
    UINT16 debug_control;
    FLOAT left_velocity_rps;
    FLOAT right_velocity_rps;
        
    
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
    
    control_cmd_velocity(&linear_velocity_mps, &angular_velocity_rps, &timeout);

    //EnsureAngularVelocity(&linear_cmd_velocity, &angular_cmd_velocity);    
    
    //Debug_Enable(DEBUG_ODOM_ENABLE_BIT);
    //Debug_Enable(DEBUG_LEFT_ENCODER_ENABLE_BIT);
    //Debug_Enable(DEBUG_RIGHT_ENCODER_ENABLE_BIT);
    //Debug_Enable(DEBUG_LEFT_PID_ENABLE_BIT);
    //Debug_Enable(DEBUG_RIGHT_PID_ENABLE_BIT);
    
#ifdef ENABLE_VELOCITY_REPEAT
    RepeatVelocity(&linear_velocity_mps, &angular_cmd_velocity_rps, &timeout);
#endif
#ifdef OVERRIDE_VELOCITY
    linear_velocity_mps = 0.0;
    angular_velocity_rps = 0.0;
    timeout = 0;
#endif

#ifdef ENABLE_ACCEL_LIMIT
    linear_velocity_mps = LimitLinearAccel(linear_velocity_mps, max_linear, LINEAR_RESPONSE_TIME);
    angular_velocity_rps = LimitAngularAccel(angular_velocity_rps, max_angular, ANGULAR_RESPONSE_TIME);
#endif    

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

    if (!left_right_cmd_velocity_override)
    {
        UniToDiff(linear_velocity_mps, angular_velocity_rps, &left_velocity_rps, &right_velocity_rps);
        
        left_velocity_cps = left_velocity_rps * WHEEL_COUNT_PER_RADIAN;
        right_velocity_cps = right_velocity_rps * WHEEL_COUNT_PER_RADIAN;
    
        //Ser_PutStringFormat("LCV: %.3f, RCV: %.3f\r\n", left_cmd_velocity, right_cmd_velocity);
        
        /* The motors have physical limits.  Do not allow the robot to be command beyond reasonable those limits.
           Configuration limits can be found in config.h.
         */
        //linear_cmd_velocity = constrain(max_robot_backward_linear_velocity, linear_cmd_velocity, max_robot_forward_linear_velocity);
        //angular_cmd_velocity = constrain(MAX_ROBOT_CCW_RADIAN_PER_SECOND, angular_cmd_velocity, MAX_ROBOT_CW_RADIAN_PER_SECOND);
        
        if (timeout > MAX_CMD_VELOCITY_TIMEOUT)
        {
            left_velocity_cps = 0;
            right_velocity_cps = 0;
        }
    }
    
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
 * Name: Control_SetLeftRightVelocityOverride
 * Description: Sets override flag to isolate control of left/right_velocity_cps.
 * Parameters: (in) enable
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Control_SetLeftRightVelocityOverride(BOOL enable)
{
    left_right_cmd_velocity_override = enable;
    /* Clear out the linear/angular velocities so they don't interfere */
    linear_velocity_mps = 0.0;
    angular_velocity_rps = 0.0;
    left_velocity_cps = 0.0;
    right_velocity_cps = 0.0;    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_SetLeftRightVelocity
 * Description: Directly sets the left/right velocity (in count/sec).  Note: has affect only if
 *              override is enabled.
 * Parameters: (in) left - left velocity (cps)
 *             (in) right - right velocity (cps)
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Control_SetLeftRightVelocity(FLOAT left, FLOAT right)
{
    if (left_right_cmd_velocity_override)
    {
        left_velocity_cps = left;
        right_velocity_cps = right;
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_LeftGetCmdVelocity
 * Description: Accessor function used to return the left commanded velocity (in count/sec).  
 * Parameters: None
 * Return: FLOAT
 * 
 *-------------------------------------------------------------------------------------------------*/ 
FLOAT Control_LeftGetCmdVelocity()
{    
    FLOAT velocity = 0.0;

    velocity = (linear_gain - linear_trim) * left_velocity_cps;
    
    //Ser_PutStringFormat("Control_LeftGetCmdVelocity: %.3f %.3f\r\n", left_velocity_cps, velocity);
    return velocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_RightGetCmdVelocity
 * Description: Accessor function used to return the right commanded velocity (in count/sec).  
 * Parameters: None
 * Return: FLOAT
 * 
 *-------------------------------------------------------------------------------------------------*/ 
FLOAT Control_RightGetCmdVelocity()
{
    FLOAT velocity = 0.0;

    velocity = (linear_gain + linear_trim) * right_velocity_cps;
    
    //Ser_PutStringFormat("Control_RightGetCmdVelocity: %.3f %.3f\r\n", right_velocity_cps, velocity);
    return velocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_LinearGetCmdVelocity
 * Description: Accessor function used to return the linear commanded velocity.  
 * Parameters: None
 * Return: FLOAT (meter/sec)
 * 
 *-------------------------------------------------------------------------------------------------*/ 
FLOAT Control_LinearGetCmdVelocity()
{
    return linear_velocity_mps;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_AngularGetCmdVelocity
 * Description: Accessor function used to return the angular commanded velocity.  
 * Parameters: None
 * Return: FLOAT (rad/sec)
 * 
 *-------------------------------------------------------------------------------------------------*/ 
FLOAT Control_AngularGetCmdVelocity()
{
    return angular_velocity_rps;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_GetCmdVelocity
 * Description: Accessor function used to return the linear/angular commanded velocities.
 * Parameters: (out) linear - linear velocity (meter/sec)
 *             (out) angular - angular velocity (rad/sec)
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
 void Control_GetCmdVelocity(FLOAT* const linear, FLOAT* const angular)
{
    *linear = linear_velocity_mps;
    *angular = angular_velocity_rps;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_SetCmdVelocity
 * Description: Accessor function used to set the linear/angular commanded velocities.
 * Parameters: (in) linear - linear velocity (meter/sec)
 *             (in) angular - angular velocity (rad/sec)
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
 void Control_SetCmdVelocity(FLOAT linear, FLOAT angular)
{
    FLOAT left_velocity_rps;
    FLOAT right_velocity_rps;
    
    if (!left_right_cmd_velocity_override)
    {    
        /* Note: This function is called from the unipid to track angular velocity.  Basically, we are taking the adjustment
           to linear/angular velocity and converting to left/right velocity (in rad/s).  The left/right pids will pick
           up left/right velocity (converted to count/s) and track linear velocity.
        */
        UniToDiff(linear, angular, &left_velocity_rps, &right_velocity_rps);
        
        left_velocity_cps = left_velocity_rps * WHEEL_COUNT_PER_RADIAN;
        right_velocity_cps = right_velocity_rps * WHEEL_COUNT_PER_RADIAN;
        
        //left_cmd_velocity = constrain(MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, left_cmd_velocity, MAX_WHEEL_FORWARD_LINEAR_VELOCITY);
        //right_cmd_velocity = constrain(MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, right_cmd_velocity,MAX_WHEEL_FORWARD_LINEAR_VELOCITY);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Control_OverrideDebug
 * Description: Function used to override the debug mask.  Used primarily during calibration.
 * Parameters: override : boolean - used to disable the debug mask settings specified via I2C.
 * Return: FLOAT
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Control_OverrideDebug(BOOL override)
{
    debug_override = override;
}

void Control_SetDeviceStatusBit(UINT16 bit)
{
    SetDeviceStatusBit(bit);
}

void Control_ClearDeviceStatusBit(UINT16 bit)
{
    ClearDeviceStatusBit(bit);
}

void Control_SetCalibrationStatus(UINT16 status)
{
    SetCalibrationStatus(status);
}

void Control_SetCalibrationStatusBit(UINT16 bit)
{
    SetCalibrationStatusBit(bit);
}

void Control_ClearCalibrationStatusBit(UINT16 bit)
{
    ClearCalibrationStatusBit(bit);
}

void Control_WriteOdom(FLOAT linear, 
                       FLOAT angular, 
                       FLOAT x_position, 
                       FLOAT y_position, 
                       FLOAT heading)
{
    WriteSpeed(linear, angular);
    WritePosition(x_position, y_position);
    WriteHeading(heading);
}

void Control_UpdateHeartbeat(UINT32 heartbeat)
{
    UpdateHeartbeat(heartbeat);
}

/* [] END OF FILE */
