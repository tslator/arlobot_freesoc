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
#include <project.h>
    
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
float Control_LinearGetCmdVelocity();
float Control_AngularGetCmdVelocity();
void Control_GetCmdVelocity(float *linear, float *angular);
void Control_LinearAngularUpdate(float linear, float angular);
void Control_OverrideDebug(uint8 override);

void Control_SetDeviceStatusBit(uint16 bit);
void Control_ClearDeviceStatusBit(uint16 bit);

void Control_SetCalibrationStatus(uint16 status);
void Control_SetCalibrationStatusBit(uint16 bit);
void Control_ClearCalibrationStatusBit(uint16 bit);

void Control_WriteOdom(float linear, float angular, float left_dist, float right_dist, float heading);
void Control_UpdateHeartbeat(uint32 heartbeat);

#endif

/* [] END OF FILE */
