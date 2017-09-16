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
#include "pidutil.h"
#include "debug.h"
#include "diag.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef UNIPID_DUMP_ENABLED
//#define UNIPID_DUMP()  DUMP_PID(DEBUG_UNIPID_ENABLE_BIT, pid.name, &pid.pid, GetControlVelocity)
#define UNIPID_DUMP()
#else
#define UNIPID_DUMP()
#endif

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define THETAPID_MIN (0.0)  // radian/sec
#define THETAPID_MAX (PI/4.0)   // radian/sec

#define THETA_KP    (0)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Prototypes
 *-------------------------------------------------------------------------------------------------*/
static float GetControlVelocity();
static float GetOdomVelocity();
static float ThetaPidUpdate(float target, float input);
static float Calc_Angle_Error(float desired, float measured);

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

static PID_TYPE pid = { 
    /* name */          "theta",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC, Calc_Angle_Error},
    /* sign */          1.0,
    /* get_target */    GetControlVelocity,
    /* get_input */     GetOdomVelocity,
    /* update */        ThetaPidUpdate,
};

static uint8 pid_enabled;
float linear_input_velocity;
float angular_input_velocity;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
 
static float Calc_Angle(float linear, float angular)
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
    float pow_2_linear;
    float pow_2_angular;
    float hyp;
    float asin_result;
     
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
 
static float Calc_Angle_Error(float desired, float measured)
{
    return atan2(sin(desired - measured), cos(desired - measured));    
}
 
static float GetControlVelocity()
{
    float linear;
    float angular;
    float value;

    Control_GetCmdVelocity(&linear, &angular);
    value = Calc_Angle(linear, angular);
    pid.sign = value > 0.0 ? 1.0 : -1.0;
    return abs(value);
}

static float GetOdomVelocity()
{
    float linear;
    float angular;
    float value;

    Odom_GetMeasVelocity(&linear, &angular);

    value = Calc_Angle(linear, angular);
    return abs(value);
}

static float ThetaPidUpdate(float target, float input)
{
    float result;
    
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
    
    PIDInit(&pid.pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, THETAPID_MIN, THETAPID_MAX, AUTOMATIC, DIRECT, Calc_Angle_Error);
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
    PIDTuningsSet(&pid.pid, THETA_KP, 0.0, 0.0);
    //pid_enabled = TRUE;
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
    float target;
    float input;
    float delta;
    float linear_desired;
    float angular_desired;
    
    Control_GetCmdVelocity(&linear_desired, &angular_desired);
    if (pid_enabled)
    {
        target = pid.get_target();
        input = pid.get_input();        
        delta = pid.update(target, input);
        angular_desired = target + delta;
        UNIPID_DUMP();
    }
    Control_SetCmdVelocity(linear_desired, angular_desired);
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
void UniPid_Enable(uint8 value)
{
    pid_enabled = value;
    if (value)
    {
        PIDModeSet(&pid.pid, AUTOMATIC);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_SetGains
 * Description: Sets the unicycle PID gains.
 * Parameters: linear_kp - the linear proportional gain
 *             linear_ki - the linear integral gain
 *             linear_kd - the linear derivative gain
 *             angular_kp - the angular proportional gain
 *             angularki - the angular integral gain
 *             angularkd - the angular derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_SetGains(float kp, float ki, float kd)
{
    PIDTuningsSet(&pid.pid, kp, ki, kd);
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_GetGains
 * Description: Returns the unicycle PID gains
 * Parameters: linear_kp - the linear proportional gain
 *             linear_ki - the linear integral gain
 *             linear_kd - the linear derivative gain
 *             angular_kp - the angular proportional gain
 *             angularki - the angular integral gain
 *             angularkd - the angular derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_GetGains(float *kp, float *ki, float *kd)
{
    *kp = pid.pid.dispKp;
    *ki = pid.pid.dispKi;
    *kd = pid.pid.dispKd;
}

/* [] END OF FILE */
