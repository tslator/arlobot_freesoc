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

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef LEFT_PID_DUMP_ENABLED
#define LEFT_DUMP_PID()  if (debug_control_enabled & DEBUG_LEFT_PID_ENABLE_BIT) DumpPid(left_pid.name, &left_pid.pid, left_pid.get_pwm())
#else
#define  LEFT_DUMP_PID()
#endif

#ifdef RIGHT_PID_DUMP_ENABLED
#define RIGHT_DUMP_PID()  if (debug_control_enabled & DEBUG_RIGHT_PID_ENABLE_BIT) DumpPid(right_pid.name, &right_pid.pid, right_pid.get_pwm())
#else
#define RIGHT_DUMP_PID()
#endif

#ifdef PID_UPDATE_DELTA_ENABLED
#define PID_DEBUG_DELTA(delta)  DEBUG_DELTA_TIME("pid", delta)
#else
#define PID_DEBUG_DELTA(delta)
#endif

#define PID_SAMPLE_TIME_MS  SAMPLE_TIME_MS(PID_SAMPLE_RATE)
#define PID_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(PID_SAMPLE_RATE)

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define PID_MIN (0)     // count/sec
#define PID_MAX (5000)  // count/sec

#define left_kp (3.55)
#define left_ki (1.955)
#define left_kd (0.57)

#define right_kp (6.0)
#define right_ki (1.95)
#define right_kd (0.65)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/    
typedef struct _pid_tag
{
    char name[6];
    WHEEL_TYPE wheel;
    PIDControl pid;
    GET_TARGET_FUNC_TYPE get_target;
    GET_ENCODER_FUNC_TYPE get_encoder;
    SET_MOTOR_PWM_FUNC_TYPE set_motor;
    GET_MOTOR_PWM_FUNC_TYPE get_pwm;
} PID_TYPE;


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

static PID_TYPE left_pid = { 
    /* name */          "left",
    /* wheel */         WHEEL_LEFT,
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
    /* get_target */    0,
    /* get_encoder */   Encoder_LeftGetCntsPerSec,
    /* set_motor */     Motor_LeftSetPwm,
    /* get_pwm */       Motor_LeftGetPwm
};

static PID_TYPE right_pid = { 
    /* name */          "right",
    /* wheel */         WHEEL_RIGHT,
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
    /* get_target */    0,    
    /* get_encoder */   Encoder_RightGetCntsPerSec,
    /* set_motor */     Motor_RightSetPwm,
    /* get_pwm */       Motor_RightGetPwm
};

static uint8 pid_enabled;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Init
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Init()
{
    pid_enabled = 0;
    left_pid.get_target = Control_LeftGetCmdVelocity;
    right_pid.get_target = Control_RightGetCmdVelocity;
    
    PIDInit(&left_pid.pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, PID_MIN, PID_MAX, AUTOMATIC, DIRECT); 
    PIDInit(&right_pid.pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, PID_MIN, PID_MAX, AUTOMATIC, DIRECT); 
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Start
 * Description: Obtains the left/right PID gains and sets them into the left/right PID controller.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Start()
{
    CAL_PID_TYPE *p_gains;
    
    // Note: the PID gains are stored in EEPROM.  The EEPROM cannot be accessed until the EEPROM
    // component is started which is handled in the Nvstore module.  
    // Pid_Start is called after Nvstore_Start.
    
    p_gains = Cal_LeftGetPidGains();    
    PIDTuningsSet(&left_pid.pid, p_gains->kp, p_gains->ki, p_gains->kd);
    
    p_gains = Cal_RightGetPidGains();
    PIDTuningsSet(&right_pid.pid, p_gains->kp, p_gains->ki, p_gains->kd);
}

/*---------------------------------------------------------------------------------------------------
 * Name: ProcessPid
 * Description: Prepares the values to be passed to the PID controller (which handles the actual
 *              PID calculations) and applies the results to the relevant motor.  This function is
 *              called at the PID sampling rate
 * Parameters: pid - the relevant PID, left or right.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void ProcessPid(PID_TYPE *pid)
{
    float tgt_speed;
    int8 dir;
    float enc_speed;
    
    tgt_speed = pid->get_target();
    dir = tgt_speed > 0 ? 1 : -1;
    
    enc_speed = pid->get_encoder();
    
    PIDSetpointSet(&pid->pid, abs(tgt_speed / METER_PER_COUNT));
    PIDInputSet(&pid->pid, abs(enc_speed));
    
    if (PIDCompute(&pid->pid))
    {
        pid->set_motor(Cal_CpsToPwm(pid->wheel, pid->pid.output * dir));
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Update
 * Description: Updates the left/right PID fields.  This function is called from the main loop and
 *              enforces the PID sampling rate.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Update()
{
    static uint32 last_update_time = 0;
    static uint8 pid_sched_offset_applied = 0;
    uint32 delta_time;
    
    delta_time = millis() - last_update_time;
    PID_DEBUG_DELTA(delta_time);
    if (delta_time >= PID_SAMPLE_TIME_MS)
    {    
        last_update_time = millis();
        
        APPLY_SCHED_OFFSET(PID_SCHED_OFFSET, pid_sched_offset_applied);
        
        ProcessPid(&left_pid);
        ProcessPid(&right_pid);
        LEFT_DUMP_PID();
        RIGHT_DUMP_PID();
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_SetLeftRightTarget
 * Description: Sets the left/right get_target fields to a different function.
 *              Note: This is done during calibration to allow internal control of the left/right
 *              wheel speed.
 * Parameters: left_target - the function that will be used to get the left target speed
 *             right_target - the function that will be used to get the right target speed
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_SetLeftRightTarget(GET_TARGET_FUNC_TYPE left_target, GET_TARGET_FUNC_TYPE right_target)
{
    left_pid.get_target = left_target;
    right_pid.get_target = right_target;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_RestoreLeftRightTarget
 * Description: Restores the left/right get_target fields to the default function.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_RestoreLeftRightTarget()
{
    left_pid.get_target = Control_LeftGetCmdVelocity;
    right_pid.get_target = Control_RightGetCmdVelocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Reset
 * Description: Resets the left/right PID fields.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Reset()
{
    left_pid.pid.input = 0;
    left_pid.pid.iTerm = 0;
    left_pid.pid.lastInput = 0;
    left_pid.pid.setpoint = 0;
    left_pid.pid.output = 0;

    right_pid.pid.input = 0;
    right_pid.pid.iTerm = 0;
    right_pid.pid.lastInput = 0;
    right_pid.pid.setpoint = 0;
    right_pid.pid.output = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Enable
 * Description: Enables/Disables the PID.  This is needed for motor and PID calibration.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Enable(uint8 enable)
{
    PIDMode mode = MANUAL;
    
    if (enable)
    {
        mode = AUTOMATIC;
    }
    
    PIDModeSet(&left_pid.pid, mode);
    PIDModeSet(&right_pid.pid, mode);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_LeftSetGains
 * Description: Sets the right PID gains.
 * Parameters: kp - the proportional gain
 *             ki - the integral gain
 *             kd - the derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_LeftSetGains(float kp, float ki, float kd)
{
    PIDTuningsSet(&left_pid.pid, kp, ki, kd);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_RightSetGains
 * Description: Sets the right PID gains.
 * Parameters: kp - the proportional gain
 *             ki - the integral gain
 *             kd - the derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_RightSetGains(float kp, float ki, float kd)
{
    PIDTuningsSet(&right_pid.pid, kp, ki, kd);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_LeftGetGains
 * Description: Returns the right PID gains
 * Parameters: kp - the proportional gain
 *             ki - the integral gain
 *             kd - the derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_LeftGetGains(float *kp, float *ki, float *kd)
{
    *kp = left_pid.pid.dispKp;
    *ki = left_pid.pid.dispKi;
    *kd = left_pid.pid.dispKd;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_RightGetGains
 * Description: Returns the right PID gains
 * Parameters: kp - the proportional gain
 *             ki - the integral gain
 *             kd - the derivative gain
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_RightGetGains(float *kp, float *ki, float *kd)
{
    *kp = right_pid.pid.dispKp;
    *ki = right_pid.pid.dispKi;
    *kd = right_pid.pid.dispKd;
}


/* [] END OF FILE */
