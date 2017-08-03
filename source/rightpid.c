/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/*---------------------------------------------------------------------------------------------------
 * Description
 *-------------------------------------------------------------------------------------------------*/
// Add a description of the module

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdio.h>
#include "config.h"
#include "pid.h"
#include "rightpid.h"
#include "time.h"
#include "utils.h"
#include "encoder.h"
#include "motor.h"
#include "pid_controller.h"
#include "diag.h"
#include "debug.h"
#include "odom.h"
#include "control.h"
#include "pidutil.h"
#include "debug.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef RIGHT_PID_DUMP_ENABLED
#define RIGHTPID_DUMP()  DUMP_PID(DEBUG_RIGHT_PID_ENABLE_BIT, pid, Motor_RightGetPwm())
#else
#define RIGHTPID_DUMP()
#endif

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define RIGHTPID_MIN (0)     // count/sec
#define RIGHTPID_MAX (5000)  // count/sec


/* The following PID values were determined experimentally and show good tracking behavior.
    Right PID - P: 3.500, I: 1.450, D: 1.100
*/

#define right_kp (3.500)
#define right_ki (1.450)
#define right_kd (1.100)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Prototypes
 *-------------------------------------------------------------------------------------------------*/
static float GetCmdVelocity();
static float EncoderInput();
static void PidUpdate(float target, float input);

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

static PID_TYPE pid = { 
    /* name */          "right",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
    /* sign */          1.0,
    /* get_target */    GetCmdVelocity,
    /* get_input */     EncoderInput,
    /* update */        PidUpdate,
};

static uint8 pid_enabled;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

static float GetCmdVelocity()
{
    float value = Control_RightGetCmdVelocity();
    pid.sign = value >= 0.0 ? 1.0 : -1.0;
    return value;
}

static float EncoderInput()
{
    return Encoder_RightGetCntsPerSec();
}

static void PidUpdate(float target, float input)
{
    PIDSetpointSet(&pid.pid, abs(target / METER_PER_COUNT));
    PIDInputSet(&pid.pid, abs(input));
    
    if (PIDCompute(&pid.pid))
    {
        Motor_RightSetPwm(Cal_CpsToPwm(WHEEL_RIGHT, pid.pid.output * pid.sign));
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_Init
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_Init()
{
    pid_enabled = 0;
    
    PIDInit(&pid.pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, RIGHTPID_MIN, RIGHTPID_MAX, AUTOMATIC, DIRECT);        
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_Start
 * Description: Obtains the left/right PID gains and sets them into the left/right PID controller.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_Start()
{
    CAL_PID_TYPE *p_gains;
    
    // Note: the PID gains are stored in EEPROM.  The EEPROM cannot be accessed until the EEPROM
    // component is started which is handled in the Nvstore module.  
    // Pid_Start is called after Nvstore_Start.
    
    p_gains = Cal_RightGetPidGains();    
    PIDTuningsSet(&pid.pid, p_gains->kp, p_gains->ki, p_gains->kd);    
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
    float target;
    float input;
    
    target = pid.get_target();
    input = pid.get_input();
    pid.update(target, input);
    RIGHTPID_DUMP();    
}


/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_SetTarget
 * Description: Sets the left/right get_target fields to a different function.
 *              Note: This is done during calibration to allow internal control of the left/right
 *              wheel speed.
 * Parameters: left_target - the function that will be used to get the left target speed
 *             right_target - the function that will be used to get the right target speed
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_SetTarget(GET_TARGET_FUNC_TYPE target)
{
    pid.get_target = target;
}

/*---------------------------------------------------------------------------------------------------
 * Name: RightPid_RestoreTarget
 * Description: Restores the left/right get_target fields to the default function.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_RestoreTarget()
{
    pid.get_target = Control_RightGetCmdVelocity;
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
 * Description: Enables/Disables the PID.  This is needed for motor and PID calibration.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RightPid_Enable(uint8 enable)
{
    PIDMode mode = MANUAL;
    
    if (enable)
    {
        mode = AUTOMATIC;
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
void RightPid_SetGains(float kp, float ki, float kd)
{
    PIDTuningsSet(&pid.pid, kp, ki, kd);
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
void RightPid_GetGains(float *kp, float *ki, float *kd)
{
    *kp = pid.pid.dispKp;
    *ki = pid.pid.dispKi;
    *kd = pid.pid.dispKd;
}

/* [] END OF FILE */
