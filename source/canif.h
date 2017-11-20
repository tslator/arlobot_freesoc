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
#include "freesoc.h"

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

UINT16 CANIF_ReadDeviceControl();
UINT16 CANIF_ReadDebugControl();
void CANIF_ReadCmdVelocity(FLOAT *linear, FLOAT *angular, UINT32 *timeout);

void CANIF_SetDeviceStatusBit(UINT16 bit);
void CANIF_ClearDeviceStatusBit(UINT16 bit);

void CANIF_SetCalibrationStatus(UINT16 status);
void CANIF_SetCalibrationStatusBit(UINT16 bit);
void CANIF_ClearCalibrationStatusBit(UINT16 bit);

void CANIF_WriteSpeed(FLOAT linear, FLOAT angular);
void CANIF_WritePosition(FLOAT x_position, FLOAT y_position);
void CANIF_WriteHeading(FLOAT heading);
void CANIF_UpdateHeartbeat(UINT32 heartbeat);

#endif

/* [] END OF FILE */