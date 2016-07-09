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

/* The purpose of this module is to handle control changes to the system.
 */

#define LEFT_RIGHT_CALC_MS  SAMPLE_TIME_MS(CONTROL_RATE)

static float left_cmd_velocity;
static float right_cmd_velocity;
#ifdef CLIFF_SENSORS    
static uint8 front_cliff_detect;
static uint8 rear_cliff_detect;
#endif

static PIDControl angular_pid;
static uint32 delta_angular_time;
static uint32 last_angular_time;

static void CalculateLeftRightSpeed()
/* Calculate the left/right wheel speed from the commanded linear/angular velocity
 */
{
    float linear;
    float angular;

    I2c_ReadCmdVelocity(&linear, &angular);

    /* Implement a PID using odometry as feedback

        Initialize Control PID:
            control_pid = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DIRECT, AUTOMATIC}
            PIDInit(&control_pid, 0, 0, 0, PID_SAMPLE_TIME_SEC, PID_MIN, PID_MAX, AUTOMATIC, DIRECT); 

        Process Control PID:
            Get measured linear, angular velocity from odometry
            Get commanded linear, angular velocity
            Compute PID
            Convert linear, angular to left, right velocity
    
        PIDSetpointSet(&control_pid, abs(angular));
        meas_angular = Odom_GetAngularVelocity()
        PIDInputSet(&control_pid, abs(meas_angular));
        
        if (PIDCompute(&control_pid))
            Convert linear/angular to left/right velocity
    
    
        Commanded Angular Velocity      Meas Angular Velocity       Error           Output
                   0.0                          0.001               0.001           -0.001 -> left/right velocity
    
     */
    
    delta_angular_time = millis() - last_angular_time;
    if (delta_angular_time >= 20)
    {
        last_angular_time = millis();
        
        //dir = tgt_speed > 0 ? 1 : -1;
        
        PIDSetpointSet(&angular_pid, abs(angular));
        float meas_angular = Odom_GetAngularVelocity();
        PIDInputSet(&angular_pid, abs(meas_angular));
        
        if (PIDCompute(&angular_pid))
        {
            angular = angular_pid.output;// * dir);
        }
        
        ConvertLinearAngularToDifferential(linear, angular, &left_cmd_velocity, &right_cmd_velocity);
    }
}

void Control_Init()
{
}

void Control_Start()
{    
    PIDInit(&angular_pid, 0.2, 0.0, 0.0, 0.05, 0, 1.0, AUTOMATIC, DIRECT);
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
    
    /* Check if calibration is requested */
    Cal_CheckRequest();
    
    CalculateLeftRightSpeed();   
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
