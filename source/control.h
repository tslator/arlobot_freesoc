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

#ifndef CONTROL_H
#define CONTROL_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <project.h>
    
/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define CONTROL_DISABLE_MOTOR_BIT        (0x0001)
#define CONTROL_CLEAR_ODOMETRY_BIT       (0x0002)
#define CONTROL_CLEAR_CALIBRATION        (0x0004)

#define STATUS_HB25_CNTRL_INIT_BIT (0x0001)


/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef void (*COMMAND_FUNC_TYPE)(float *left, float *right, uint32 *timeout);

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void Control_Init();
void Control_Start();
void Control_Update();
void Control_SetCommandVelocityFunc(COMMAND_FUNC_TYPE cmd);
void Control_RestoreCommandVelocityFunction();
float Control_LeftGetCmdVelocity();
float Control_RightGetCmdVelocity();
void Control_OverrideDebug(uint8 override);

void Control_SetDeviceStatusBit(uint16 bit);
void Control_ClearDeviceStatusBit(uint16 bit);

void Control_SetCalibrationStatus(uint16 status);
void Control_SetCalibrationStatusBit(uint16 bit);
void Control_ClearCalibrationStatusBit(uint16 bit);

void Control_WriteOdom(float left_speed, float right_speed, float left_dist, float right_dist, float heading);
void Control_UpdateHeartbeat(uint32 heartbeat);


#endif

/* [] END OF FILE */
