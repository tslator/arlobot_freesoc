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
#include <stdio.h>
#include <math.h>
#include "config.h"
#include "time.h"
#include "control.h"
#include "encoder.h"
#include "motor.h"
#include "odom.h"
#include "cal.h"
#include "pid.h"
#include "unipid.h"
#include "pid_controller.h"
#include "utils.h"
#include "debug.h"
#include "diag.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef UNIPID_DUMP_ENABLED
#define UNIPID_DUMP() DumpPid(pid.name, pid.debug_bit, &pid.pid)
#else
#define UNIPID_DUMP()
#endif

#define UNI_PID_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(PID_SAMPLE_RATE)

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define THETAPID_MIN (-PI/4.0)  // radian/sec
#define THETAPID_MAX (PI/4.0)   // radian/sec

#define THETA_KP    (0.0)
#define THETA_KF    (3.0)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Prototypes
 *-------------------------------------------------------------------------------------------------*/
static FLOAT GetControlVelocity();
static FLOAT GetOdomVelocity();
static FLOAT ThetaPidUpdate(FLOAT target, FLOAT input);
static FLOAT Calc_Angle_Error(FLOAT desired, FLOAT measured);

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

static PID_TYPE pid = { 
    /* name */          "theta",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, /*Kf*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC, Calc_Angle_Error},
    /* sign */          1.0,
    /* debug bit */     DEBUG_UNIPID_ENABLE_BIT,
    /* get_target */    GetControlVelocity,
    /* get_input */     GetOdomVelocity,
    /* update */        ThetaPidUpdate,
};

static BOOL pid_enabled;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
 
static FLOAT Calc_Angle(FLOAT linear, FLOAT angular)
{
    /* Calculate a theta between linear and angular.
      w
        |____
        |   /|
        |  / |
        | /th|
        |/___|____________ v

        Theta is the angle between v and w and represents the control variable
        Tracking theta between the desired velocity, vd/wd, and the measured
        velocity, vo/wo, couples the wheels and improves straight line motion
    */
    FLOAT pow_2_linear;
    FLOAT pow_2_angular;
    FLOAT hyp;
    FLOAT asin_result;
     
    pow_2_linear = linear * linear;
    pow_2_angular = angular * angular;
    hyp = sqrt(pow_2_linear + pow_2_angular);
     
    asin_result = asin(angular / hyp);
    /* For some reason, asin returns Nan when angular == 0 which messes up the remaining calculations
       The range of possible angles between linear and angular velocity is -pi/4 .. pi/4
       0 is certainly a legitimate value so its not clear why Nan is returned.
       Possibly sqrt is returning Nan, but the debug when asin return Nan showed hyp as non-Nan
    */
    if (isnan(asin_result))
    {
        asin_result = 0.0;
    }
     
    return asin_result;    
}
 
static FLOAT Calc_Angle_Error(FLOAT desired, FLOAT measured)
{
    return atan2(sin(desired - measured), cos(desired - measured));
}
 
static FLOAT GetControlVelocity()
{
    FLOAT linear;
    FLOAT angular;
    FLOAT value;

    Control_GetCmdVelocity(&linear, &angular);
    value = Calc_Angle(linear, angular);
    pid.sign = value > 0.0 ? 1.0 : -1.0;
    
    return abs(value);
}

static FLOAT GetOdomVelocity()
{
    FLOAT linear;
    FLOAT angular;
    FLOAT value;

    Odom_GetMeasVelocity(&linear, &angular);

    value = Calc_Angle(linear, angular);
    return abs(value);
}

static FLOAT ThetaPidUpdate(FLOAT target, FLOAT input)
{
    FLOAT result;
    
    PIDSetpointSet(&pid.pid, target);
    PIDInputSet(&pid.pid, input);
    
    if (PIDCompute(&pid.pid))
    {
        result = pid.pid.output * pid.sign;
    }
    else
    {
        result = target;
    }
    
    return result;
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_Init
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_Init()
{
    pid_enabled = FALSE;
    PIDInit(&pid.pid, 0, 0, 0, 0, UNI_PID_SAMPLE_TIME_SEC, THETAPID_MIN, THETAPID_MAX, AUTOMATIC, DIRECT, Calc_Angle_Error);
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_Start
 * Description: Obtains the left/right PID gains and sets them into the left/right PID controller.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_Start()
{
    PIDTuningsSet(&pid.pid, 1.0, 0.0, 0.0, 0.0);
    pid_enabled = FALSE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_Process
 * Description: 
 * Parameters: 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_Process()
{
    FLOAT target;
    FLOAT input;
    FLOAT delta;
    FLOAT linear_cmd;
    FLOAT angular_cmd;
    
    Control_GetCmdVelocity(&linear_cmd, &angular_cmd);
    
    if (pid_enabled)
    {
        target = pid.get_target();
        input = pid.get_input();        
        delta = pid.update(target, input);
        
        angular_cmd += delta;
        //UNIPID_DUMP();
        //DumpPid(pid.name, &pid.pid);
    }
    
    Control_SetCmdVelocity(linear_cmd, angular_cmd);
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_Reset
 * Description: Resets the unicycle PID fields.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_Reset()
{
    pid.pid.input = 0;
    pid.pid.iTerm = 0;
    pid.pid.lastInput = 0;
    pid.pid.setpoint = 0;
    pid.pid.output = 0;

}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_Enable
 * Description: Enables/Disables the PID.  This is needed for motor and PID calibration.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_Enable(BOOL value)
{
    pid_enabled = value;
    if (value)
    {
        PIDModeSet(&pid.pid, AUTOMATIC);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_Bypass
 * Description: Bypassing the PID calculation by setting the PID mode to either MANUAL or AUTOMATIC.
 *              MANUAL mode bypasses the PID control calculation and uses the unmodified target value.
 *              AUTOMATIC mode performs the PID control calculation and uses the PID output value.
 * Parameters: value - TRUE if the PID calculation is to be bypassed; otherwise, FALSE.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 void UniPid_Bypass(BOOL value)
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
 * Name: UniPid_SetGains
 * Description: Sets the unicycle PID gains.
 * Parameters: kp - the linear proportional gain
 *             ki - the linear integral gain
 *             kd - the linear derivative gain
 *             kf - the feedforward gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_SetGains(FLOAT kp, FLOAT ki, FLOAT kd, FLOAT kf)
{
    PIDTuningsSet(&pid.pid, kp, ki, kd, kf);
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_GetGains
 * Description: Returns the unicycle PID gains
 * Parameters: kp - the linear proportional gain
 *             ki - the linear integral gain
 *             kd - the linear derivative gain
 *             kf - the feedforward gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_GetGains(FLOAT* const kp, FLOAT* const ki, FLOAT* const kd, FLOAT* const kf)
{
    *kp = pid.pid.dispKp;
    *ki = pid.pid.dispKi;
    *kd = pid.pid.dispKd;
    *kf = pid.pid.dispKf;
}

/* [] END OF FILE */
