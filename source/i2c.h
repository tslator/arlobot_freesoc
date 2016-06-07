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
    
#define CONTROL_POWER_ON_MOTOR_BIT       (0x0001)
#define CONTROL_CLEAR_ENCODER_COUNT_BIT  (0x0002)
#define CONTROL_ENABLE_CALIBRATION_BIT   (0x0004)
#define CONTROL_UPLOAD_CALIBRATION_BIT   (0x0008)
#define CONTROL_DOWNLOAD_CALIBRATION_BIT (0x0010)
#define CONTROL_VALIDATE_CALIBRATION_BIT (0x0020)
    

#define STATUS_HB25_CNTRL_INIT_BIT (0x0001)
#define STATUS_CALIBRATED_BIT      (0x0002)
#define STATUS_CALIBRATING_BIT     (0x0004)

    

void I2c_Init();
void I2c_Start();

uint16 I2c_ReadControl();
void I2c_ReadLRCmdVelocity(int16 *left_velocity, int16 *right_velocity);
int16 I2c_LeftReadCmdVelocity();
int16 I2c_RightReadCmdVelocity();
void I2c_ReadLRCmdAcceleration(int16 *left_accel, int16 *right_accel);
void I2c_WriteCalReg(uint16 value);
uint16 I2c_ReadCalReg();

void I2c_SetStatusBit(uint8 bit);
void I2c_ClearStatusBit(uint8 bit);
void I2c_WriteLRCountPerSecond(int16 left_count_per_second, int16 right_count_per_second);
void I2c_LeftWriteOutput(float cps, int16 mmps);
void I2c_RightWriteOutput(float cps, int16 mmps);
void I2c_WriteLRWheelVelocity(int16 left_wheel_velocity, int16 right_wheel_velocity);
void I2c_WriteCountPerSecond(int16 count);
void I2c_WriteWheelVelocity(int16 velocity);

void I2c_WriteFrontUltrasonicDistance(uint8 offset, uint16 distance);
void I2c_WriteRearUltrasonicDistance(uint8 offset, uint16 distance);
void I2c_WriteFrontInfraredDistance(uint8 offset, uint8 distance);
void I2c_WriteRearInfraredDistance(uint8 offset, uint8 distance);
void I2c_WriteOdom(float x_dist, float y_dist, float heading, float linear_speed, float angular_speed);

void I2c_WriteHeartbeat();



#endif

/* [] END OF FILE */
