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

#ifndef MOTOR_H
#define MOTOR_H

#include <project.h>
#include "config.h"
#include "cal.h"

void Motor_Init();
void Motor_Start();
void Motor_Stop();

void Motor_LeftSetPwm(uint16 pwm);
void Motor_RightSetPwm(uint16 pwm);
void Motor_SetPwm(uint16 left_pwm, uint16 right_pwm);
void Motor_LeftSetCntsPerSec(float cps);
void Motor_RightSetCntsPerSec(float cps);
void Motor_LeftSetMeterPerSec(float mmps);
void Motor_RightSetMeterPerSec(float mmps);
uint16 Motor_LeftGetPwm();
uint16 Motor_RightGetPwm();

void Motor_CollectPwmCpsSamples(WHEEL_TYPE wheel, uint8 reverse_pwm, uint8 num_avg_iter, uint16 *pwm_samples, int32 *cps_samples);
void Motor_LeftSetCalibration(CAL_DATA_TYPE *fwd_cal_data, CAL_DATA_TYPE *bwd_cal_data);
void Motor_RightSetCalibration(CAL_DATA_TYPE *fwd_cal_data, CAL_DATA_TYPE *bwd_cal_data);

#endif

/* [] END OF FILE */
