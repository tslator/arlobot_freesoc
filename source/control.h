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

#endif

/* [] END OF FILE */
