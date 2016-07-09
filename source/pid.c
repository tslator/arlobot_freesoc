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

#ifdef LEFT_PID_DUMP_ENABLED
#define LEFT_DUMP_PID(pid)  if (debug_control_enabled & DEBUG_LEFT_PID_ENABLE_BIT) DumpPid(pid)
#else
#define  LEFT_DUMP_PID(pid)
#endif

#ifdef RIGHT_PID_DUMP_ENABLED
#define RIGHT_DUMP_PID(pid)  if (debug_control_enabled & DEBUG_RIGHT_PID_ENABLE_BIT) DumpPid(pid)
#else
#define RIGHT_DUMP_PID(pid)
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
    GET_TARGET_TYPE get_target;
    GET_ENCODER_TYPE get_encoder;
    SET_MOTOR_TYPE set_motor;
    GET_MOTOR_PWM_TYPE get_pwm;
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

#if defined (LEFT_PID_DUMP_ENABLED) || defined(RIGHT_PID_DUMP_ENABLED)
static void DumpPid(PID_TYPE *pid)
{
    char set_point_str[10];
    char input_str[10];
    char error_str[10];
    char last_input_str[10];
    char iterm_str[10];
    char output_str[10];

    ftoa(pid->pid.setpoint, set_point_str, 3);
    ftoa(pid->pid.input, input_str, 3);
    ftoa(pid->pid.setpoint - pid->pid.input, error_str, 6);
    ftoa(pid->pid.lastInput, last_input_str, 3);
    ftoa(pid->pid.iTerm, iterm_str, 6);
    ftoa(pid->pid.output, output_str, 6);

    if (PID_DEBUG_CONTROL_ENABLED)
    {
        DEBUG_PRINT_ARG("%s pid: %s %s %s %s %s %s %d \r\n", pid->name, set_point_str, input_str, error_str, last_input_str, iterm_str, output_str, pid->get_pwm());
    }
}
#endif

void Pid_Init(GET_TARGET_TYPE left_target, GET_TARGET_TYPE right_target)
{
    
    left_pid.get_target = left_target;
    right_pid.get_target = right_target;
    
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
        LEFT_DUMP_PID(&left_pid);
        RIGHT_DUMP_PID(&right_pid);
    }
}

void Pid_SetLeftRightTarget(GET_TARGET_TYPE left_target, GET_TARGET_TYPE right_target)
{
    left_pid.get_target = left_target;
    right_pid.get_target = right_target;
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
