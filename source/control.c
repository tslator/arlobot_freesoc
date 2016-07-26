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

#include "control.h"
#include "i2c.h"
#include "motor.h"
#include "cal.h"
#include "odom.h"
#include "time.h"
#include "utils.h"
#include "debug.h"
#include "pid_controller.h"
#include "pidutil.h"

/* The purpose of this module is to handle control changes to the system.
 */

#ifdef CTRL_LINEAR_PID_DUMP_ENABLED
#define LINEAR_DUMP_PID()  if (debug_control_enabled & DEBUG_CTRL_LINEAR_PID_ENABLE_BIT) DumpPid("clin", &linear_pid)
#else
#define LINEAR_DUMP_PID()
#endif

#ifdef CTRL_ANGULAR_PID_DUMP_ENABLED
#define ANGULAR_DUMP_PID()  if (debug_control_enabled & DEBUG_CTRL_ANGULAR_PID_ENABLE_BIT) DumpPid("cang", &angular_pid)
#else
#define ANGULAR_DUMP_PID()
#endif

#define CTRL_VELOCITY_SAMPLE_TIME_MS SAMPLE_TIME_MS(CTRL_VELOCITY_RATE)
#define CTRL_VELOCITY_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(CTRL_VELOCITY_RATE)

#ifdef CTRL_UPDATE_DELTA_ENABLED
#define CTRL_DEBUG_DELTA(delta)  DEBUG_DELTA_TIME("ctrl", delta)
#else
#define CTRL_DEBUG_DELTA(delta)
#endif

#define LINEAR_PID_MIN (-1.0)
#define LINEAR_PID_MAX (1.0)

#define ANGULAR_PID_MIN (-0.5)
#define ANGULAR_PID_MAX (0.5)

static COMMAND_FUNC_TYPE control_cmd_velocity;
static uint32 last_update_time;

static PIDControl linear_pid;
static PIDControl angular_pid;
static uint32 last_update_time;

static float left_cmd_velocity;
static float right_cmd_velocity;

static void ProcessPid(PIDControl *pid, float sample_time, float target, float meas)
{
    int8 dir;
    
    dir = target > 0 ? 1 : -1;
    
    PIDSampleTimeSet(pid, sample_time);
    PIDSetpointSet(pid, abs(target));
    PIDInputSet(pid, abs(meas));
    
    if (PIDCompute(pid))
    {
        pid->output *= dir;
    }
}

static void UpdateSpeed()
/* Calculate the left/right wheel speed from the commanded linear/angular velocity
 */
{
    static uint32 last_update_time = 0;
    float linear;
    float angular;
    float meas_linear;
    float meas_angular;
    uint32 delta_time;

    control_cmd_velocity(&linear, &angular);

    /* Implements a PID for linear and angular velocity using odometry as feedback
    
       This is needed to couple the left/right PIDs that control wheel velocity,
       necessary to keep the robot moving in a straight line.

     */
        
    delta_time = millis() - last_update_time;        
    CTRL_DEBUG_DELTA(delta_time);
    
    if (delta_time > CTRL_VELOCITY_SAMPLE_TIME_MS)
    {
        last_update_time = millis();
        
        meas_linear = Odom_GetLinearVelocity();
        meas_angular = Odom_GetAngularVelocity();
        
        ProcessPid(&linear_pid, (float) delta_time, linear, meas_linear);
        ProcessPid(&angular_pid, (float) delta_time, angular, meas_angular);
        
        LINEAR_DUMP_PID();
        ANGULAR_DUMP_PID();

        UniToDiff(linear_pid.output, angular_pid.output, &left_cmd_velocity, &right_cmd_velocity);
    }

}

void Control_Init()
{
    control_cmd_velocity = I2c_ReadCmdVelocity;
    PIDInit(&linear_pid, 0.0, 0.0, 0.0, CTRL_VELOCITY_SAMPLE_TIME_SEC, LINEAR_PID_MIN, LINEAR_PID_MAX, AUTOMATIC, DIRECT);
    PIDInit(&angular_pid, 0.0, 0.0, 0.0, CTRL_VELOCITY_SAMPLE_TIME_SEC, ANGULAR_PID_MIN, ANGULAR_PID_MAX, AUTOMATIC, DIRECT);
}

void Control_Start()
{   
    CAL_PID_TYPE *p_gains;

    p_gains = Cal_LinearGetPidGains();
    PIDTuningsSet(&linear_pid, p_gains->kp, p_gains->ki, p_gains->kd);
    p_gains = Cal_AngularGetPidGains();
    PIDTuningsSet(&angular_pid, p_gains->kp, p_gains->ki, p_gains->kd);    
}

void Control_Update()
{
    uint16 control = I2c_ReadDeviceControl();
    if (control & CONTROL_DISABLE_MOTOR_BIT)
    {
        Motor_Stop();
    }
    
    if (control & CONTROL_CLEAR_ODOMETRY_BIT)
    {
        Odom_Reset();
    }

    I2c_ReadDebugControl();
    
    UpdateSpeed();   
}

void Control_SetCommandVelocityFunc(COMMAND_FUNC_TYPE cmd)
{
    control_cmd_velocity = cmd;
}

void Control_RestoreCommandVelocityFunc()
{
    control_cmd_velocity = I2c_ReadCmdVelocity;
}

float Control_LeftGetCmdVelocity()
{
    return left_cmd_velocity;
}

float Control_RightGetCmdVelocity()
{
    return right_cmd_velocity;
}

void Control_LinearSetGains(float kp, float ki, float kd)
{
    PIDTuningsSet(&linear_pid, kp, ki, kd);
}

void Control_AngularSetGains(float kp, float ki, float kd)
{
    PIDTuningsSet(&angular_pid, kp, ki, kd);
}

/* [] END OF FILE */
