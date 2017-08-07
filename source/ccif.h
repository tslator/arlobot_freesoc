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
   Description: This module defines the command/control interfaces.  Two interfaces are supported:
       * I2C
       * CAN
    See config.h for settings.
 *-------------------------------------------------------------------------------------------------*/

#ifndef CCIF_H
#define CCIF_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include "config.h"


/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/
#if defined (ENABLE_I2CIF) && !defined(ENABLE_CANIF)
#include "i2cif.h"

#define ReadCmdVelocity                 I2CIF_ReadCmdVelocity
#define ReadDeviceControl               I2CIF_ReadDeviceControl
#define ReadDebugControl                I2CIF_ReadDebugControl
#define SetDeviceStatusBit              I2CIF_SetDeviceStatusBit
#define ClearDeviceStatusBit            I2CIF_ClearDeviceStatusBit
#define SetCalibrationStatus            I2CIF_SetCalibrationStatus    
#define SetCalibrationStatusBit         I2CIF_SetCalibrationStatusBit
#define ClearCalibrationStatusBit       I2CIF_ClearCalibrationStatusBit
#define WriteSpeed                      I2CIF_WriteSpeed
#define WritePosition                   I2CIF_WritePosition
#define WriteHeading                    I2CIF_WriteHeading
#define UpdateHeartbeat                 I2CIF_UpdateHeartbeat

#elif !defined(ENABLE_I2CIF) && defined(ENABLE_CANIF)
#include "canif.h"    

#define ReadCmdVelocity                 CANIF_ReadCmdVelocity
#define ReadDeviceControl               CANIF_ReadDeviceControl
#define ReadDebugControl                CANIF_ReadDebugControl
#define SetDeviceStatusBit              CANIF_SetDeviceStatusBit
#define ClearDeviceStatusBit            CANIF_ClearDeviceStatusBit
#define SetCalibrationStatus            CANIF_SetCalibrationStatus    
#define SetCalibrationStatusBit         CANIF_SetCalibrationStatusBit
#define ClearCalibrationStatusBit       CANIF_ClearCalibrationStatusBit
#define WriteSpeed                      CANIF_WriteSpeed
#define WritePosition                   CANIF_WritePosition
#define WriteHeading                    CANIF_WriteHeading
#define UpdateHeartbeat                 CANIF_UpdateHeartbeat

#else
#error "Only one interface can be defined at a time!"
#endif    

#endif /* CCIF_H */
/* [] END OF FILE */
