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

#ifdef LEFT_PID_DUMP_ENABLED
#define LEFT_DUMP_PID(pid)  DumpPid(pid)
#else
#define LEFT_DUMP_PID(pid)
#endif

#ifdef RIGHT_PID_DUMP_ENABLED
#define RIGHT_DUMP_PID(pid)  DumpPid(pid)
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

#define DEFAULT_KP (5)
#define DEFAULT_KI (2.2)
#define DEFAULT_KD (0.67)


#define TUNE_KP (6.0)
#define TUNE_KI (2.2)
#define TUNE_KD (0.65)

#define left_kp (TUNE_KP)
#define left_ki (TUNE_KI)
#define left_kd (TUNE_KD)

#define right_kp (TUNE_KP)
#define right_ki (TUNE_KI)
#define right_kd (TUNE_KD)

static PID_TYPE left_pid = { 
    /* name */          "left",
    /* pid */           {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
    /* get_target */    0,
    /* get_encoder */   Encoder_LeftGetCntsPerSec,
    /* set_motor */     Motor_LeftSetCntsPerSec,
    /* get_pwm */       Motor_LeftGetPwm
};

static PID_TYPE right_pid = { 
    /* name */          "right",
    /* pid */           {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC},
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

    DEBUG_PRINT_STR("%s pid: %s %s %s %s %s %s %d \r\n", pid->name, set_point_str, input_str, error_str, last_input_str, iterm_str, output_str, pid->get_pwm());
}
#endif

void Pid_Init(GET_TARGET_TYPE left_target, GET_TARGET_TYPE right_target)
{
    left_pid.get_target = left_target;
    right_pid.get_target = right_target;
    
    PIDInit(&left_pid.pid, left_kp, left_ki, left_kd, PID_SAMPLE_TIME_SEC, PID_MIN, PID_MAX, AUTOMATIC, DIRECT); 
    PIDInit(&right_pid.pid, right_kp, right_ki, right_kd, PID_SAMPLE_TIME_SEC, PID_MIN, PID_MAX, AUTOMATIC, DIRECT); 
}
    
void Pid_Start()
{
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
        
/* [] END OF FILE */
