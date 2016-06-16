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

/* The purpose of this module is to handle control changes to the system.
 */

#define LEFT_RIGHT_CALC_MS  SAMPLE_TIME_MS(CONTROL_RATE)

static float left_cmd_velocity;
static float right_cmd_velocity;

static void CalculateLeftRightSpeed()
/* Calculate the left/right wheel speed from the commanded linear/angular velocity
 */
{
    float linear;
    float angular;
    
    I2c_ReadCmdVelocity(&linear, &angular);
    left_cmd_velocity = linear - (angular * TRACK_WIDTH)/2;
    right_cmd_velocity = linear + (angular * TRACK_WIDTH)/2;
}

void Control_Init()
{
}

void Control_Start()
{    
}

void Control_Update()
{
    uint16 control;
    
    control = I2c_ReadControl();
    
    if (control & CONTROL_DISABLE_MOTOR_BIT)
    {
        Motor_Stop();
    }
    
    if (control & CONTROL_CLEAR_ODOMETRY_BIT)
    {
        Odom_Reset();
    }    

    /* Perform calibration */
    if (control & CONTROL_ENABLE_CALIBRATION_BIT)
    {
        Cal_Update();
    }
    
    if (control & CONTROL_VALIDATE_CALIBRATION_BIT)
    {
        Cal_Validate();
    }
    
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
