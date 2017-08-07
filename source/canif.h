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
   Description: This module provides the implementation for communicating over CAN.
 *-------------------------------------------------------------------------------------------------*/    


#ifndef I2CIF_H
#define I2CIF_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <project.h>

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    

 /*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    

 /*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void CANIF_Init();
void CANIF_Start();

uint16 CANIF_ReadDeviceControl();
uint16 CANIF_ReadDebugControl();
void CANIF_ReadCmdVelocity(float *left, float *right, uint32 *timeout);

void CANIF_SetDeviceStatusBit(uint16 bit);
void CANIF_ClearDeviceStatusBit(uint16 bit);

void CANIF_SetCalibrationStatus(uint16 status);
void CANIF_SetCalibrationStatusBit(uint16 bit);
void CANIF_ClearCalibrationStatusBit(uint16 bit);

void CANIF_WriteSpeed(float left_speed, float right_speed);
void CANIF_WritePosition(float x_position, float y_position);
void CANIF_WriteHeading(float heading);
void CANIF_UpdateHeartbeat(uint32 heartbeat);

#endif

/* [] END OF FILE */