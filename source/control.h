/* 
MIT License

Copyright (c) 2017 Tim Slator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*---------------------------------------------------------------------------------------------------
   Description: This module provides functionality for controlling the robot state.  The primary
   purpose is to receive control and speed commands and apply them.  Additionally, debugging levels
   are also handled here.
 *-------------------------------------------------------------------------------------------------*/


#ifndef CONTROL_H
#define CONTROL_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include "freesoc.h"
    
/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define CONTROL_DISABLE_MOTOR_BIT        (0x0001)
#define CONTROL_CLEAR_ODOMETRY_BIT       (0x0002)
#define CONTROL_CLEAR_CALIBRATION        (0x0004)

#define STATUS_HB25_CNTRL_INIT_BIT (0x0001)
    
#define ENCODER_DEBUG_BIT   (0x0001)
#define PID_DEBUG_BIT       (0x0002)
#define MOTOR_DEBUG_BIT     (0x0004)
#define ODOM_DEBUG_BIT      (0x0008)
#define SAMPLE_DEBUG_BIT    (0x0010)
    


/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef void (*COMMAND_FUNC_TYPE)(FLOAT *linear, FLOAT *angular, UINT32 *timeout);

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void Control_Init();
void Control_Start();
void Control_Update();
void Control_SetCommandVelocityFunc(COMMAND_FUNC_TYPE cmd);
void Control_RestoreCommandVelocityFunc();
FLOAT Control_LeftGetCmdVelocityCps();
FLOAT Control_RightGetCmdVelocityCps();
void Control_GetCmdVelocity(FLOAT* const linear, FLOAT* const angular);
void Control_SetCmdVelocity(FLOAT linear, FLOAT angular);
void Control_OverrideDebug(BOOL override);

void Control_SetDeviceStatusBit(UINT16 bit);
void Control_ClearDeviceStatusBit(UINT16 bit);

void Control_SetCalibrationStatus(UINT16 status);
void Control_SetCalibrationStatusBit(UINT16 bit);
void Control_ClearCalibrationStatusBit(UINT16 bit);

void Control_WriteOdom(FLOAT linear, FLOAT angular, FLOAT left_dist, FLOAT right_dist, FLOAT heading);
void Control_UpdateHeartbeat(UINT32 heartbeat);

void Control_SetLeftRightVelocityOverride(BOOL enable);
void Control_SetLeftRightVelocityMps(FLOAT left, FLOAT right);
void Control_SetLeftRightVelocityCps(FLOAT left, FLOAT right);
void Control_EnableAcceleration(BOOL enable);

#endif

/* [] END OF FILE */
