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

#ifdef LEFT_PID_DUMP_ENABLED
#define LEFT_DUMP_PID()  if (debug_control_enabled & DEBUG_LEFT_PID_ENABLE_BIT) DumpPid(left_pid.name, &left_pid.pid)
#else
#define  LEFT_DUMP_PID()
#endif

#ifdef RIGHT_PID_DUMP_ENABLED
#define RIGHT_DUMP_PID()  if (debug_control_enabled & DEBUG_RIGHT_PID_ENABLE_BIT) DumpPid(right_pid.name, &right_pid.pid)
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

#define PID_MIN (0)
#define PID_MAX (300)

typedef struct _pid_tag
{
    char name[6];
    PIDControl pid;
    GET_TARGET_FUNC_TYPE get_target;
    GET_ENCODER_FUNC_TYPE get_encoder;
    SET_MOTOR_FUNC_TYPE set_motor;
    GET_MOTOR_PWM_FUNC_TYPE get_pwm;
} PID_TYPE;


#define left_kp (3.55)
#define left_ki (1.955)
#define left_kd (0.57)

#define right_kp (6.0)
#define right_ki (1.95)
#define right_kd (0.65)

static PID_TYPE left_pid = { 
    /* name */          "left",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
    /* get_target */    0,
    /* get_encoder */   Encoder_LeftGetCntsPerSec,
    /* set_motor */     Motor_LeftSetCntsPerSec,
    /* get_pwm */       Motor_LeftGetPwm
};

static PID_TYPE right_pid = { 
    /* name */          "right",
    /* pid */           {0, 0, 0, /*Kp*/0, /*Ki*/0, /*Kd*/0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
    /* get_target */    0,    
    /* get_encoder */   Encoder_RightGetCntsPerSec,
    /* set_motor */     Motor_RightSetCntsPerSec,
    /* get_pwm */       Motor_RightGetPwm
};

static uint8 pid_enabled;


void Pid_Init()
{
    pid_enabled = 0;
    left_pid.get_target = Control_LeftGetCmdVelocity;
    right_pid.get_target = Control_RightGetCmdVelocity;
    
    PIDInit(&left_pid.pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, PID_MIN, PID_MAX, AUTOMATIC, DIRECT); 
    PIDInit(&right_pid.pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, PID_MIN, PID_MAX, AUTOMATIC, DIRECT); 
}
    
void Pid_Start()
{
    CAL_PID_TYPE *p_gains;
    
    // Note: the PID gains are stored in EEPROM, so we don't have access to EEPROM until the EEPROM component has been 
    // started.  Pid_Start is called after Nvstore_Start.
    
    p_gains = Cal_LeftGetPidGains();    
    PIDTuningsSet(&left_pid.pid, p_gains->kp, p_gains->ki, p_gains->kd);
    //PIDTuningsSet(&left_pid.pid, left_kp, left_ki, left_kd);
    
    p_gains = Cal_RightGetPidGains();
    PIDTuningsSet(&right_pid.pid, p_gains->kp, p_gains->ki, p_gains->kd);
    //PIDTuningsSet(&right_pid.pid, right_kp, right_ki, right_kd);
}

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
        pid->set_motor(pid->pid.output * dir);
    }
}

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

void Pid_SetLeftRightTarget(GET_TARGET_FUNC_TYPE left_target, GET_TARGET_FUNC_TYPE right_target)
{
    left_pid.get_target = left_target;
    right_pid.get_target = right_target;
}

void Pid_RestoreLeftRightTarget()
{
    left_pid.get_target = Control_LeftGetCmdVelocity;
    right_pid.get_target = Control_RightGetCmdVelocity;
}

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

void Pid_LeftSetGains(float kp, float ki, float kd)
{
    PIDTuningsSet(&left_pid.pid, kp, ki, kd);
}

void Pid_RightSetGains(float kp, float ki, float kd)
{
    PIDTuningsSet(&right_pid.pid, kp, ki, kd);
}

void Pid_LeftGetGains(float *kp, float *ki, float *kd)
{
    *kp = left_pid.pid.dispKp;
    *ki = left_pid.pid.dispKi;
    *kd = left_pid.pid.dispKd;
}

void Pid_RightGetGains(float *kp, float *ki, float *kd)
{
    *kp = right_pid.pid.dispKp;
    *ki = right_pid.pid.dispKi;
    *kd = right_pid.pid.dispKd;
}


/* [] END OF FILE */
