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
   Description: This module provides the implementation for communicating over I2C.
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
#ifdef TEST_I2C
#define I2CIF_TEST()  I2CIF_Test()
#else
#define I2CIF_TEST()
#endif        
    
/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void I2CIF_Init();
void I2CIF_Start();

uint16 I2CIF_ReadDeviceControl();
uint16 I2CIF_ReadDebugControl();
void I2CIF_ReadCmdVelocity(float *linear, float *angular, uint32 *timeout);

void I2CIF_SetDeviceStatusBit(uint16 bit);
void I2CIF_ClearDeviceStatusBit(uint16 bit);

void I2CIF_SetCalibrationStatus(uint16 status);
void I2CIF_SetCalibrationStatusBit(uint16 bit);
void I2CIF_ClearCalibrationStatusBit(uint16 bit);


void I2CIF_WriteSpeed(float linear, float angular);
void I2CIF_WritePosition(float x_position, float y_position);
void I2CIF_WriteHeading(float heading);
void I2CIF_UpdateHeartbeat(uint32 heartbeat);

#ifdef TEST_I2C
void I2CIF_Test();
#endif
#endif

/* [] END OF FILE */
