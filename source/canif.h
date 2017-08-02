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