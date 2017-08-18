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
#include "config.h"
#include "time.h"
#include "control.h"
#include "encoder.h"
#include "motor.h"
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
#ifdef UNI_PID_DUMP_ENABLED
//#define LINEARPID_DUMP()  DUMP_PID(DEBUG_UNI_PID_ENABLE_BIT, pid, Control_LinearGetCmdVelocity)
//#define ANGULARPID_DUMP()  DUMP_PID(DEBUG_UNI_PID_ENABLE_BIT, pid, Control_AngularGetCmdVelocity)
#else
#define LINEARPID_DUMP()
#define ANGULARPID_DUMP()
#endif

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define LINEARPID_MIN (0)                                   // meter/sec
#define LINEARPID_MAX (MAX_WHEEL_FORWARD_LINEAR_VELOCITY)   // meter/sec
#define ANGULARPID_MIN (0)                                  // radian/sec
#define ANGULARPID_MAX (MAX_ROBOT_RADIAN_PER_SECOND)        // radian/sec


#define LINEAR_KP (0.5)
#define LINEAR_KI (0)
#define LINEAR_KD (0)

#define ANGULAR_KP (0.5)
#define ANGULAR_KI (0)
#define ANGULAR_KD (0)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Prototypes
 *-------------------------------------------------------------------------------------------------*/
static float GetLinearCmdVelocity();
static float GetAngularCmdVelocity();
static float EncoderLinearInput();
static float EncoderAngularInput();
static float LinearPidUpdate(float target, float input);
static float AngularPidUpdate(float target, float input);

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

static PID_TYPE linear_pid = { 
    /* name */          "linear",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
    /* sign */          1.0,
    /* get_target */    GetLinearCmdVelocity,
    /* get_input */     EncoderLinearInput,
    /* update */        LinearPidUpdate,
};

static PID_TYPE angular_pid = { 
    /* name */          "angular",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
    /* sign */          1.0,
    /* get_target */    GetAngularCmdVelocity,
    /* get_input */     EncoderAngularInput,
    /* update */        AngularPidUpdate,
};

static uint8 pid_enabled;
float linear_input_velocity;
float angular_input_velocity;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

static float GetLinearCmdVelocity()
{
    float value = Control_LinearGetCmdVelocity();
    linear_pid.sign = value >= 0.0 ? 1.0 : -1.0;
    return value;
}

static float GetAngularCmdVelocity()
{
    float value = Control_AngularGetCmdVelocity();
    angular_pid.sign = value >= 0.0 ? 1.0 : -1.0;
    return value;
}

static float EncoderLinearInput()
{
    return Encoder_LinearGetVelocity();
}

static float EncoderAngularInput()
{
    return Encoder_AngularGetVelocity();
}

static float LinearPidUpdate(float target, float input)
{
    PIDSetpointSet(&linear_pid.pid, abs(target));
    PIDInputSet(&linear_pid.pid, abs(input));
    
    if (PIDCompute(&linear_pid.pid))
    {
        return linear_pid.pid.output * linear_pid.sign;        
    }
    
    return input;
}

static float AngularPidUpdate(float target, float input)
{
    PIDSetpointSet(&angular_pid.pid, abs(target));
    PIDInputSet(&angular_pid.pid, abs(input));
    
    if (PIDCompute(&angular_pid.pid))
    {
        return angular_pid.pid.output * angular_pid.sign;
    }
    
    return input;
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
    pid_enabled = 0;
    
    PIDInit(&linear_pid.pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, LINEARPID_MIN, LINEARPID_MAX, AUTOMATIC, DIRECT);        
    PIDInit(&angular_pid.pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, ANGULARPID_MIN, ANGULARPID_MAX, AUTOMATIC, DIRECT);        
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
    CAL_PID_TYPE *p_gains;
    
    // Note: the PID gains are stored in EEPROM.  The EEPROM cannot be accessed until the EEPROM
    // component is started which is handled in the Nvstore module.  
    // UniPid_Start is called after Nvstore_Start.
    
    p_gains = Cal_LinearGetPidGains();
    //PIDTuningsSet(&linear_pid.pid, p_gains->kp, p_gains->ki, p_gains->kd);    
    PIDTuningsSet(&linear_pid.pid, LINEAR_KP, 0.0, 0.0);

    p_gains = Cal_AngularGetPidGains();
    //PIDTuningsSet(&angular_pid.pid, p_gains->kp, p_gains->ki, p_gains->kd);    
    PIDTuningsSet(&angular_pid.pid, ANGULAR_KP, 0.0, 0.0);
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
    float linear_target;
    float angular_target;
    float linear_input;
    float angular_input;
    float linear_output;
    float angular_output;
    float left;
    float right;
    float left_cps;
    float right_cps;
    
    
    linear_target = linear_pid.get_target();
    linear_input = linear_pid.get_input();
    
    angular_target = angular_pid.get_target();
    angular_input = angular_pid.get_input();
    
    linear_output = linear_pid.update(linear_target, linear_input);
    angular_output = angular_pid.update(angular_target, angular_input);
    
    LINEARPID_DUMP();
    ANGULARPID_DUMP();
    
    UniToDiff(linear_output, angular_output, &left, &right);
    
    /* Convert mps to cps before setting motor pwm */
    left_cps = left * WHEEL_COUNT_PER_METER;
    right_cps = right * WHEEL_COUNT_PER_METER;
    
    Motor_LeftSetPwm(Cal_CpsToPwm(WHEEL_LEFT, left_cps));
    Motor_RightSetPwm(Cal_CpsToPwm(WHEEL_RIGHT, right_cps));
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_SetTarget
 * Description: Sets the unicycle get_target fields to a different function.
 *              Note: This is done during calibration to allow internal control of the linear/angular
 *              speed.
 * Parameters: linear_target - the function that will be used to get the linear target speed
 *             angular_target - the function that will be used to get the angular target speed
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_SetTarget(GET_TARGET_FUNC_TYPE linear_target, GET_TARGET_FUNC_TYPE angular_target)
{
    linear_pid.get_target = linear_target;
    angular_pid.get_target = angular_target;
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_RestoreTarget
 * Description: Restores the unicycle get_target fields to the default function.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_RestoreTarget()
{
    linear_pid.get_target = Control_LinearGetCmdVelocity;
    angular_pid.get_target = Control_AngularGetCmdVelocity;
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
    linear_pid.pid.input = 0;
    linear_pid.pid.iTerm = 0;
    linear_pid.pid.lastInput = 0;
    linear_pid.pid.setpoint = 0;
    linear_pid.pid.output = 0;

    angular_pid.pid.input = 0;
    angular_pid.pid.iTerm = 0;
    angular_pid.pid.lastInput = 0;
    angular_pid.pid.setpoint = 0;
    angular_pid.pid.output = 0;    
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniPid_Enable
 * Description: Enables/Disables the PID.  This is needed for motor and PID calibration.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniPid_Enable(uint8 enable)
{
    PIDMode mode = MANUAL;
    
    if (enable)
    {
        mode = AUTOMATIC;
    }
    
    PIDModeSet(&linear_pid.pid, mode);
    PIDModeSet(&angular_pid.pid, mode);
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
void UniPid_SetGains(float linear_kp, float linear_ki, float linear_kd, 
                     float angular_kp, float angular_ki, float angular_kd)
{
    PIDTuningsSet(&linear_pid.pid, linear_kp, linear_ki, linear_kd);
    PIDTuningsSet(&angular_pid.pid, angular_kp, angular_ki, angular_kd);
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
void UniPid_GetGains(float *linear_kp, float *linear_ki, float *linear_kd, 
                     float *angular_kp, float *angular_ki, float *angular_kd)
{
    *linear_kp = linear_pid.pid.dispKp;
    *linear_ki = linear_pid.pid.dispKi;
    *linear_kd = linear_pid.pid.dispKd;

    *angular_kp = angular_pid.pid.dispKp;
    *angular_ki = angular_pid.pid.dispKi;
    *angular_kd = angular_pid.pid.dispKd;
}

/* [] END OF FILE */
