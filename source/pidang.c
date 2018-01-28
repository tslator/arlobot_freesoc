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
   Description: This module provides the implementation of the linear velocity PID.
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "control.h"
#include "odom.h"
#include "cal.h"
#include "pid.h"
#include "pidtypes.h"
#include "pidang.h"
#include "pid_controller.h"
#include "debug.h"
#include "utils.h"
#include "consts.h"
#include "diag.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef ANGPID_DUMP_ENABLED
#define ANGPID_DUMP() DumpPid(pid.name, pid.debug_bit, &pid.pid)
#else
#define ANGPID_DUMP()
#endif

#define ANG_PID_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(PID_SAMPLE_RATE)

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define ANGPID_MIN (0)                              // radian/sec
#define ANGPID_MAX (2*MAX_ROBOT_RADIAN_PER_SECOND)  // radian/sec

#define ANG_KP    (0.0)
#define ANG_KF    (1.0)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Prototypes
 *-------------------------------------------------------------------------------------------------*/
static FLOAT GetControlVelocity();
static FLOAT GetOdomVelocity();
static FLOAT AngularPidUpdate(FLOAT target, FLOAT input);

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

static PID_TYPE pid = { 
    /* name */          "ang",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, /*Kf*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC, NULL},
    /* sign */          1.0,
    /* debug bit */     DEBUG_ANGPID_ENABLE_BIT,
    /* get_target */    GetControlVelocity,
    /* get_input */     GetOdomVelocity,
    /* update */        AngularPidUpdate,
};

static BOOL pid_enabled;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
 
static FLOAT GetControlVelocity()
{
    FLOAT linear;
    FLOAT angular;

    Control_GetCmdVelocity(&linear, &angular);
    pid.sign = 1.0;
    
    return angular + MAX_ROBOT_RADIAN_PER_SECOND;
}

static FLOAT GetOdomVelocity()
{
    FLOAT linear;
    FLOAT angular;

    Odom_GetMeasVelocity(&linear, &angular);

    return angular + MAX_ROBOT_RADIAN_PER_SECOND;
}

static FLOAT AngularPidUpdate(FLOAT target, FLOAT input)
{
    FLOAT result;
    
    PIDSetpointSet(&pid.pid, target);
    PIDInputSet(&pid.pid, input);
    
    if (PIDCompute(&pid.pid))
    {
        result = pid.pid.output - MAX_ROBOT_RADIAN_PER_SECOND;
    }
    else
    {
        result = target;
    }
    
    return result;
}

/*---------------------------------------------------------------------------------------------------
 * Name: AngPid_Init
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void AngPid_Init()
{
    pid_enabled = FALSE;

    PIDInit(&pid.pid, 0, 0, 0, 0, ANG_PID_SAMPLE_TIME_SEC, ANGPID_MIN, ANGPID_MAX, AUTOMATIC, DIRECT, NULL);
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: AngPid_Start
 * Description: Obtains the left/right PID gains and sets them into the left/right PID controller.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void AngPid_Start()
{
    PIDTuningsSet(&pid.pid, 0.2, 0.0, 0.0, 1.0);
    pid_enabled = TRUE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: AngPid_Process
 * Description: 
 * Parameters: 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void AngPid_Process()
{
    FLOAT target;
    FLOAT input;
    FLOAT linear_cmd;
    FLOAT angular_cmd;
    
    if (pid_enabled)
    {
        Control_GetCmdVelocity(&linear_cmd, &angular_cmd);
        target = pid.get_target();
        input = pid.get_input();        
        angular_cmd += pid.update(target, input);
        Control_SetCmdVelocity(linear_cmd, angular_cmd);
        
        ANGPID_DUMP();
    }
    
}

/*---------------------------------------------------------------------------------------------------
 * Name: AngPid_Reset
 * Description: Resets the unicycle PID fields.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void AngPid_Reset()
{
    pid.pid.input = 0;
    pid.pid.iTerm = 0;
    pid.pid.lastInput = 0;
    pid.pid.setpoint = 0;
    pid.pid.output = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: AngPid_Enable
 * Description: Enables/Disables the PID.  This is needed for motor and PID calibration.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void AngPid_Enable(BOOL value)
{
    pid_enabled = value;
    if (value)
    {
        PIDModeSet(&pid.pid, AUTOMATIC);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: AngPid_Bypass
 * Description: Bypassing the PID calculation by setting the PID mode to either MANUAL or AUTOMATIC.
 *              MANUAL mode bypasses the PID control calculation and uses the unmodified target value.
 *              AUTOMATIC mode performs the PID control calculation and uses the PID output value.
 * Parameters: value - TRUE if the PID calculation is to be bypassed; otherwise, FALSE.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 void AngPid_Bypass(BOOL value)
 {
     PIDMode mode = AUTOMATIC;
     
     /* Note: To bypass the PID calculation, PID processing must be enabled. */
     if (value)
     {
         pid_enabled = TRUE;
         mode = MANUAL;
     }
     
     PIDModeSet(&pid.pid, mode);
 }
 

/*---------------------------------------------------------------------------------------------------
 * Name: AngPid_SetGains
 * Description: Sets the unicycle PID gains.
 * Parameters: kp - the linear proportional gain
 *             ki - the linear integral gain
 *             kd - the linear derivative gain
 *             kf - the feedforward gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void AngPid_SetGains(FLOAT kp, FLOAT ki, FLOAT kd, FLOAT kf)
{
    PIDTuningsSet(&pid.pid, kp, ki, kd, kf);
}

/*---------------------------------------------------------------------------------------------------
 * Name: AngPid_GetGains
 * Description: Returns the unicycle PID gains
 * Parameters: kp - the linear proportional gain
 *             ki - the linear integral gain
 *             kd - the linear derivative gain
 *             kf - the feedforward gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void AngPid_GetGains(FLOAT* const kp, FLOAT* const ki, FLOAT* const kd, FLOAT* const kf)
{
    *kp = pid.pid.dispKp;
    *ki = pid.pid.dispKi;
    *kd = pid.pid.dispKd;
    *kf = pid.pid.dispKf;
}

/* [] END OF FILE */
