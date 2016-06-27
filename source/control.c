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
#ifdef CLIFF_SENSORS    
static uint8 front_cliff_detect;
static uint8 rear_cliff_detect;
#endif

static void CalculateLeftRightSpeed()
/* Calculate the left/right wheel speed from the commanded linear/angular velocity
 */
{
    float linear;
    float angular;

    I2c_ReadCmdVelocity(&linear, &angular);
    #ifdef CLIFF_SENSORS    
    if ( (front_cliff_detect && linear > 0 && angular != 0) ||
         (rear_cliff_detect && linear < 0 && angular != 0) )
    {
        linear = 0;
        angular = 0;
    }
    #endif
    
    ConvertLinearAngularToDifferential(linear, angular, &left_cmd_velocity, &right_cmd_velocity);
    
}

void Control_Init()
{
}

void Control_Start()
{    
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
    
#ifdef COMMS_DEBUG_ENABLED
    // When debug is enabled, the bitmap can be used to turn on/off specific debug, e.g., encoder, pid, odom, etc.
    debug_control_enabled = I2c_ReadDebugControl();
#endif

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
