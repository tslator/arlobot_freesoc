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
   Description: This module provides the implementation of the left wheel PID.
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
#include "pidleft.h"
#include "utils.h"
#include "debug.h"
#include "diag.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef LEFT_PID_DUMP_ENABLED
#define LEFTPID_DUMP()  DumpPid(pid.name, &pid.pid)
#else
#define LEFTPID_DUMP()
#endif

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/

/* The following PID values were determined experimentally and show good tracking behavior.
*/
#define LEFT_KP (2.950)
#define LEFT_KI (2.800)
#define LEFT_KD (0.525)

#define LEFT_PID_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(PID_SAMPLE_RATE)
    

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
    /* name */          "left",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, /*Kf*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC, NULL},
    /* sign */          1.0,
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
    pid.sign = cps >= 0.0 ? 1.0 : -1.0;
    return cps;
}

static FLOAT EncoderInput()
{
    return abs(Encoder_LeftGetCntsPerSec());
}

static FLOAT PidUpdate(FLOAT target, FLOAT input)
{
    PWM_TYPE pwm;
    
    PIDSetpointSet(&pid.pid, abs(target));
    PIDInputSet(&pid.pid, input);
    
    /* Note: PIDCompute returns TRUE when in AUTOMATIC mode and FALSE when in MANUAL mode */
    if (PIDCompute(&pid.pid))
    {
        pwm = Cal_CpsToPwm(WHEEL_LEFT, pid.pid.output * pid.sign);
    }
    else
    {
        pwm = Cal_CpsToPwm(WHEEL_LEFT, target);
    }

    Motor_LeftSetPwm(pwm);
    
    /* Note: The PID update return value is not used. */ 
    return 0.0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: LeftPid_Init
 * Description: Initializes module variables to default values.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_Init()
{
    pid_enabled = FALSE;
    
    target_source = Control_LeftGetCmdVelocityCps;
    old_target_source = NULL;

    PIDInit(&pid.pid, 0, 0, 0, 0, LEFT_PID_SAMPLE_TIME_SEC, LEFTPID_MIN, LEFTPID_MAX, AUTOMATIC, DIRECT, NULL);        
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: LeftPid_Start
 * Description: Obtains the left PID gains and sets them into the left PID controller.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_Start()
{
    pid_enabled = Pid_SetGains(&pid.pid, Cal_GetPidGains(PID_TYPE_LEFT));
    pid_enabled = TRUE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: LeftPid_Process
 * Description: 
 * Parameters: 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_Process()
{
    FLOAT target;
    FLOAT input;
    
    /* Note: If PID processing is disabled, then skip all PID-related work */
    if (pid_enabled)
    {
        target = pid.get_target();
        input = pid.get_input();
        pid.update(target, input);
        LEFTPID_DUMP();
    }
}


/*---------------------------------------------------------------------------------------------------
 * Name: LeftPid_SetTarget
 * Description: Sets the left target source to the specified function.
 *              Note: This is done during calibration to allow internal control of the left
 *              wheel speed.
 * Parameters: target - the function that will be the source of the left target speed.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_SetTarget(GET_TARGET_FUNC_TYPE target)
{
    old_target_source = target_source;
    target_source = target;
}

/*---------------------------------------------------------------------------------------------------
 * Name: LeftPid_RestoreTarget
 * Description: Restores the left target source to the default function.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_RestoreTarget()
{
    target_source = old_target_source;
}

/*---------------------------------------------------------------------------------------------------
 * Name: LeftPid_Reset
 * Description: Resets the left PID fields.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_Reset()
{
    pid.pid.input = 0;
    pid.pid.iTerm = 0;
    pid.pid.lastInput = 0;
    pid.pid.setpoint = 0;
    pid.pid.output = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: LeftPid_Enable
 * Description: Enables/Disables PID processing (see LeftPid_Process).  There are times when the PID
 *              needs to be completely disabled but still callable from the main loop.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_Enable(BOOL value)
{
    pid_enabled = value;
    if (value)
    {
        PIDModeSet(&pid.pid, AUTOMATIC);        
    }

}

/*---------------------------------------------------------------------------------------------------
 * Name: LeftPid_Bypass
 * Description: Bypassing the PID calculation by setting the PID mode to either MANUAL or AUTOMATIC.
 *              MANUAL mode bypasses the PID control calculation and uses the unmodified target value.
 *              AUTOMATIC mode performs the PID control calculation and uses the PID output value.
 * Parameters: value - TRUE if the PID calculation is to be bypassed; otherwise, FALSE.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_Bypass(BOOL value)
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
 * Name: LeftPid_SetGains
 * Description: Sets the right PID gains.
 * Parameters: kp - the proportional gain
 *             ki - the integral gain
 *             kd - the derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_SetGains(FLOAT kp, FLOAT ki, FLOAT kd, FLOAT kf)
{
    PIDTuningsSet(&pid.pid, kp, ki, kd, kf);
}

/*---------------------------------------------------------------------------------------------------
 * Name: LeftPid_GetGains
 * Description: Returns the right PID gains
 * Parameters: kp - the proportional gain
 *             ki - the integral gain
 *             kd - the derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void LeftPid_GetGains(FLOAT* const kp, FLOAT* const ki, FLOAT* const kd, FLOAT* const kf)
{
    *kp = pid.pid.dispKp;
    *ki = pid.pid.dispKi;
    *kd = pid.pid.dispKd;
    *kf = pid.pid.dispKf;
}

/* [] END OF FILE */
