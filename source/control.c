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
#define LIN_DUMP_PID()  if (debug_control_enabled & DEBUG_CTRL_LINEAR_PID_ENABLE_BIT) DumpPid("clin", &linear_pid)
#else
#define LIN_DUMP_PID()
#endif

#ifdef CTRL_ANGULAR_PID_DUMP_ENABLED
#define ANG_DUMP_PID()  if (debug_control_enabled & DEBUG_CTRL_ANGULAR_PID_ENABLE_BIT) DumpPid("cang", &angular_pid)
#else
#define ANG_DUMP_PID()
#endif


#define CTRL_VELOCITY_SAMPLE_TIME_MS  SAMPLE_TIME_MS(CTRL_VELOCITY_RATE)
#define CTRL_VELOCITY_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(CTRL_VELOCITY_RATE)

#ifdef CTRL_UPDATE_DELTA_ENABLED
#define CTRL_DEBUG_DELTA(delta)  DEBUG_DELTA_TIME("ctrl", delta)
#else
#define CTRL_DEBUG_DELTA(delta)
#endif

#define LIN_PID_MIN (0.0)
#define LIN_PID_MAX (1.0)
#define ANG_PID_MIN (0.0)
#define ANG_PID_MAX (1.0)


static COMMAND_FUNC_TYPE control_cmd_velocity;

static PIDControl angular_pid;
static PIDControl linear_pid;
static uint32 last_update_time;

static float left_cmd_velocity;
static float right_cmd_velocity;

#ifdef CLIFF_SENSORS    
static uint8 front_cliff_detect;
static uint8 rear_cliff_detect;
#endif


static void ProcessPid(PIDControl *pid, float tgt_speed, float meas_speed)
{
    int8 dir;
    
    dir = tgt_speed > 0 ? 1 : -1;
    
    PIDSetpointSet(pid, abs(tgt_speed));
    PIDInputSet(pid, abs(meas_speed));
    
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
    float odom_linear;
    float odom_angular;
    uint32 delta_time;

    control_cmd_velocity(&linear, &angular);

    /* Implements a PID for linear and angular velocity using odometry as feedback
    
       This is needed to couple the left/right PIDs that control wheel velocity,
       necessary to keep the robot moving in a straight line.

     */
        
    delta_time = millis() - last_update_time;        
    CTRL_DEBUG_DELTA(delta_time);
    
    if (last_update_time == 0 || delta_time >= CTRL_VELOCITY_SAMPLE_TIME_MS)
    {    
        last_update_time = millis();
        
        odom_linear = Odom_GetLinearVelocity();
        odom_angular = Odom_GetAngularVelocity();

        ProcessPid(&linear_pid, linear, odom_linear);
        ProcessPid(&angular_pid, angular, odom_angular);
        
        LIN_DUMP_PID();
        ANG_DUMP_PID();
     
    }

    ConvertLinearAngularToDifferential(linear_pid.output, angular_pid.output, &left_cmd_velocity, &right_cmd_velocity);        
}

void Control_Init()
{
    control_cmd_velocity = I2c_ReadCmdVelocity;
}

void Control_Start()
{    
    PIDInit(&linear_pid, 0.2, 0.0, 0.0, CTRL_VELOCITY_SAMPLE_TIME_SEC, LIN_PID_MIN, LIN_PID_MAX, AUTOMATIC, DIRECT);
    PIDInit(&angular_pid, 0.2, 0.0, 0.0, CTRL_VELOCITY_SAMPLE_TIME_SEC, ANG_PID_MIN, ANG_PID_MAX, AUTOMATIC, DIRECT);
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

/* [] END OF FILE */
