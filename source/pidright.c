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
   Description: This module provides the implementation of the right wheel PID.
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdio.h>
#include "config.h"
#include "time.h"
#include "control.h"
#include "encoder.h"
#include "motor.h"
#include "odom.h"
#include "pid_controller.h"
#include "pid.h"
#include "pidright.h"
#include "utils.h"
#include "debug.h"
#include "diag.h"
#include "consts.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef RIGHT_PID_DUMP_ENABLED
#define RIGHTPID_DUMP()  DumpPid(pid.name, pid.debug_bit, &pid.pid)
#else
#define RIGHTPID_DUMP()
#endif

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/

/* The following PID values were determined experimentally and show good tracking behavior.
*/

#define RIGHT_KP (2.900)
#define RIGHT_KI (2.720)
#define RIGHT_KD (0.530)

#define RIGHT_PID_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(PID_SAMPLE_RATE)    

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Prototypes
 *-------------------------------------------------------------------------------------------------*/
static FLOAT GetCmdVelocity();
static FLOAT EncoderInput();
static FLOAT PidUpdate(FLOAT target, FLOAT input);

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

static PID_TYPE pid = { 
    /* name */          "right",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, /*Kf*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC, NULL},
    /* sign */          1.0,
    /* debug bit */     DEBUG_RIGHT_PID_ENABLE_BIT,
    /* get_target */    GetCmdVelocity,
    /* get_input */     EncoderInput,
    /* update */        PidUpdate,
};

static BOOL pid_enabled;

static GET_TARGET_FUNC_TYPE old_target_source;
static GET_TARGET_FUNC_TYPE target_source;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

static FLOAT GetCmdVelocity()
{
    FLOAT cps = target_source();
    Motor_RightSetPwm(Cal_CpsToPwm(WHEEL_RIGHT, cps));
    
    pid.sign = cps >= 0.0 ? 1.0 : -1.0;
    return cps;
}

static FLOAT EncoderInput()
{
    return abs(Encoder_RightGetCntsPerSec());
}

static FLOAT PidUpdate(FLOAT target, FLOAT input)
{
    PWM_TYPE pwm;
    
    PIDSetpointSet(&pid.pid, abs(target));
    PIDInputSet(&pid.pid, input);
    
    /* Note: PIDCompute returns TRUE when in AUTOMATIC mode and FALSE when in MANUAL mode */
    if (PIDCompute(&pid.pid))
    {
        pwm = Cal_CpsToPwm(WHEEL_RIGHT, pid.pid.output * pid.sign);
    }
    else
    {
        pwm = Cal_CpsToPwm(WHEEL_RIGHT, target);
    }

    Motor_RightSetPwm(pwm);
    
    /* Note: The PID update return value is not used. */ 
    return 0.0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_Init
 * Description: Initializes module variables to default values.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_Init()
{
    pid_enabled = FALSE;
    
    target_source = Control_RightGetCmdVelocityCps;
    old_target_source = NULL;

    PIDInit(&pid.pid, 0, 0, 0, 0, RIGHT_PID_SAMPLE_TIME_SEC, RIGHTPID_MIN, RIGHTPID_MAX, AUTOMATIC, DIRECT, NULL);        
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_Start
 * Description: Obtains the right PID gains and sets them into the right PID controller.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_Start()
{
    pid_enabled = Pid_SetGains(&pid.pid, Cal_GetPidGains(PID_TYPE_RIGHT));
    pid_enabled = TRUE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_Process
 * Description: 
 * Parameters: 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_Process()
{
    FLOAT target;
    FLOAT input;
    
    /* Note: If PID processing is disabled, then skip all PID-related work */
    if (pid_enabled)
    {
        target = pid.get_target();
        input = pid.get_input();
        pid.update(target, input);
        RIGHTPID_DUMP();
    }
}


/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_SetTarget
 * Description: Sets the right target source to the specified function.
 *              Note: This is done during calibration to allow internal control of the right
 *              wheel speed.
 * Parameters: target - the function that will be the source of the right target speed.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_SetTarget(GET_TARGET_FUNC_TYPE target)
{
    old_target_source = target_source;
    target_source = target;
}

/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_RestoreTarget
 * Description: Restores the right target source to the default function.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_RestoreTarget()
{
    target_source = old_target_source;
}

/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_Reset
 * Description: Resets the right PID fields.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_Reset()
{
    pid.pid.input = 0;
    pid.pid.iTerm = 0;
    pid.pid.lastInput = 0;
    pid.pid.setpoint = 0;
    pid.pid.output = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_Enable
 * Description: Enables/Disables PID processing (see RightPid_Process).  There are times when the PID
 *              needs to be completely disabled but still callable from the main loop.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_Enable(BOOL value)
{
    pid_enabled = value;
    if (value)
    {
        PIDModeSet(&pid.pid, AUTOMATIC);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_Bypass
 * Description: Bypassing the PID calculation by setting the PID mode to either MANUAL or AUTOMATIC.
 *              MANUAL mode bypasses the PID control calculation and uses the unmodified target value.
 *              AUTOMATIC mode performs the PID control calculation and uses the PID output value.
 * Parameters: value - TRUE if the PID calculation is to be bypassed; otherwise, FALSE.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_Bypass(BOOL value)
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
 * Name: RightPid_SetGains
 * Description: Sets the right PID gains.
 * Parameters: kp - the proportional gain
 *             ki - the integral gain
 *             kd - the derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_SetGains(FLOAT kp, FLOAT ki, FLOAT kd, FLOAT kf)
{
    PIDTuningsSet(&pid.pid, kp, ki, kd, kf);
}

/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_GetGains
 * Description: Returns the right PID gains
 * Parameters: kp - the proportional gain
 *             ki - the integral gain
 *             kd - the derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_GetGains(FLOAT* const kp, FLOAT* const ki, FLOAT* const kd, FLOAT* const kf)
{
    *kp = pid.pid.dispKp;
    *ki = pid.pid.dispKi;
    *kd = pid.pid.dispKd;
    *kf = pid.pid.dispKf;
}

/* [] END OF FILE */
