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
void I2CIF_ReadCmdVelocity(float *left, float *right, uint32 *timeout);

void I2CIF_SetDeviceStatusBit(uint16 bit);
void I2CIF_ClearDeviceStatusBit(uint16 bit);

void I2CIF_SetCalibrationStatus(uint16 status);
void I2CIF_SetCalibrationStatusBit(uint16 bit);
void I2CIF_ClearCalibrationStatusBit(uint16 bit);


void I2CIF_WriteSpeed(float left_speed, float right_speed);
void I2CIF_WriteDistance(float left_delta_dist, float right_delta_dist);
void I2CIF_WriteHeading(float heading);
void I2CIF_UpdateHeartbeat(uint32 heartbeat);

#ifdef TEST_I2C
void I2CIF_Test();
#endif
#endif

/* [] END OF FILE */
