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

#ifndef I2C_H
#define I2C_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <project.h>
    
/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define STATUS_HB25_CNTRL_INIT_BIT (0x0001)

    
/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void I2c_Init();
void I2c_Start();

uint16 I2c_ReadDeviceControl();
void I2c_ReadDebugControl();
void I2c_ReadCmdVelocity(float *left, float *right, uint32 *timeout);

void I2c_SetDeviceStatusBit(uint16 bit);
void I2c_ClearDeviceStatusBit(uint16 bit);
void I2c_SetCalibrationStatusBit(uint16 bit);
void I2c_ClearCalibrationStatusBit(uint16 bit);
void I2c_SetCalibrationStatus(uint16 status);
void I2c_WriteUltrasonicFrontDistances(float* distances);
void I2c_WriteUltrasonicRearDistances(float* distances);
void I2c_WriteInfraredFrontDistances(float* distances);
void I2c_WriteInfraredRearDistances(float* distances);

void I2c_WriteOdom(float left_speed, float right_speed, float left_delta_dist, float right_delta_dist, float heading);

void I2c_UpdateHeartbeat(uint32 counter);

#endif

/* [] END OF FILE */
