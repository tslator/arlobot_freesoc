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
    uint16 dev_control;
    uint16 cal_control;
    
    dev_control = I2c_ReadDeviceControl();
#ifdef COMMS_DEBUG_ENABLED
    // When debug is enabled, the bitmap can be used to turn on/off specify debug, e.g., encoder, pid, odom, etc.
    debug_control_enabled = I2c_ReadDebugControl();
#endif
    cal_control = I2c_ReadCalibrationControl();
    
    if (dev_control & CONTROL_DISABLE_MOTOR_BIT)
    {
        Motor_Stop();
    }
    
    if (dev_control & CONTROL_CLEAR_ODOMETRY_BIT)
    {
        Odom_Reset();
    }
    
    /* Note: We can add more logic here regarding the status of calibration.  For example, if cps2pwm calibration has
       not been done, then we can prevent any request for movement.
     */
    if (cal_control)
    {
        Cal_Update(cal_control);
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
