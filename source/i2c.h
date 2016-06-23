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

#include <project.h>
    
#define CONTROL_DISABLE_MOTOR_BIT        (0x0001)
#define CONTROL_CLEAR_ODOMETRY_BIT       (0x0002)
#define CONTROL_ENABLE_CALIBRATION_BIT   (0x0004)
#define CONTROL_VALIDATE_CALIBRATION_BIT (0x0008)
    

#define STATUS_HB25_CNTRL_INIT_BIT (0x0001)
#define STATUS_CALIBRATED_BIT      (0x0002)
#define STATUS_CALIBRATING_BIT     (0x0004)

    

void I2c_Init();
void I2c_Start();

uint16 I2c_ReadDeviceControl();
uint16 I2c_ReadDebugControl();
uint16 I2c_ReadCalibrationControl();
void I2c_ReadCmdVelocity(float *linear, float *angular);

void I2c_SetDeviceStatusBit(uint8 bit);
void I2c_ClearDeviceStatusBit(uint8 bit);
void I2c_SetCalibrationStatusBit(uint8 bit);
void I2c_ClearCalibrationStatusBit(uint8 bit);

void I2c_WriteFrontUltrasonicDistance(uint8 offset, uint16 distance);
void I2c_WriteRearUltrasonicDistance(uint8 offset, uint16 distance);
void I2c_WriteFrontInfraredDistance(uint8 offset, uint8 distance);
void I2c_WriteRearInfraredDistance(uint8 offset, uint8 distance);
void I2c_WriteOdom(float x_dist, float y_dist, float heading, float linear_speed, float angular_speed);

void I2c_UpdateHeartbeat(uint32 counter);



#endif

/* [] END OF FILE */
