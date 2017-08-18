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
#include "leftpid.h"
#include "utils.h"
#include "pidutil.h"
#include "debug.h"
#include "diag.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef LEFT_PID_DUMP_ENABLED
#define LEFTPID_DUMP()  DUMP_PID(DEBUG_LEFT_PID_ENABLE_BIT, pid, Motor_LeftGetPwm())
#else
#define LEFTPID_DUMP()
#endif

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/

/* The following PID values were determined experimentally and show good tracking behavior.
*/
#define LEFT_KP (1.000)
#define LEFT_KI (4.000)
#define LEFT_KD (0.000)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Prototypes
 *-------------------------------------------------------------------------------------------------*/
static float GetCmdVelocity();
static float EncoderInput();
static float PidUpdate(float target, float input);

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

static PID_TYPE pid = { 
    /* name */          "left",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
    /* sign */          1.0,
    /* get_target */    GetCmdVelocity,
    /* get_input */     EncoderInput,
    /* update */        PidUpdate,
};

static uint8 pid_enabled;

static GET_TARGET_FUNC_TYPE old_target_source;
static GET_TARGET_FUNC_TYPE target_source;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

static float GetCmdVelocity()
{
    float value = target_source();
    pid.sign = value >= 0.0 ? 1.0 : -1.0;
    //Ser_PutStringFormat("getcmdvelocity: %f\r\n", value);
    
    return abs(value / WHEEL_METER_PER_COUNT);
}

static float EncoderInput()
{
    return abs(Encoder_LeftGetCntsPerSec());
}

static float PidUpdate(float target, float input)
{
    PWM_TYPE pwm;
    
    PIDSetpointSet(&pid.pid, target);
    PIDInputSet(&pid.pid, input);
    
    /* Note: PIDCompute returns TRUE when in AUTOMATIC mode and FALSE when in MANUAL mode */
    if (PIDCompute(&pid.pid))
    {
        //Ser_PutStringFormat("pid auto\r\n");
        pwm = Cal_CpsToPwm(WHEEL_LEFT, pid.pid.output * pid.sign);
    }
    else
    {
        //Ser_PutStringFormat("pid manual\r\n");
        pwm = Cal_CpsToPwm(WHEEL_LEFT, target / WHEEL_METER_PER_COUNT);
    }

    //Ser_PutStringFormat("pidupdate %f %f %f %d\r\n", target, input, pid.pid.output * pid.sign, pwm);
    
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
    pid_enabled = 0;
    
    target_source = Control_LeftGetCmdVelocity;
    old_target_source = NULL;
    PIDInit(&pid.pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, LEFTPID_MIN, LEFTPID_MAX, AUTOMATIC, DIRECT);        
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
    CAL_PID_TYPE *p_gains;
    
    // Note: the PID gains are stored in EEPROM.  The EEPROM cannot be accessed until the EEPROM
    // component is started which is handled in the Nvstore module.  
    // Pid_Start is called after Nvstore_Start.
    
    if (Cal_GetCalibrationStatusBit(CAL_PID_BIT))
    {
        p_gains = Cal_LeftGetPidGains();  
        PIDTuningsSet(&pid.pid, p_gains->kp, p_gains->ki, p_gains->kd);
    }
    else
    {
        Ser_PutString("No valid PID calibration\r\n");
    }
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
    float target;
    float input;
    
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
void LeftPid_Enable(uint8 value)
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
void LeftPid_Bypass(uint8 value)
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
void LeftPid_SetGains(float kp, float ki, float kd)
{
    PIDTuningsSet(&pid.pid, kp, ki, kd);
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
void LeftPid_GetGains(float *kp, float *ki, float *kd)
{
    *kp = pid.pid.dispKp;
    *ki = pid.pid.dispKi;
    *kd = pid.pid.dispKd;
}

/* [] END OF FILE */
