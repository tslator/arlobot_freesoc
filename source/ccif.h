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
#ifndef CCIF_H
#define CCIF_H
    
#include "config.h"


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
