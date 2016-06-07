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

void Motor_Init();
void Motor_Start();
void Motor_Stop();

void Motor_LeftSetPwm(uint16 pwm);
void Motor_RightSetPwm(uint16 pwm);
void Motor_SetPwm(uint16 left_pwm, uint16 right_pwm);
void Motor_LeftSetCntsPerSec(float cps);
void Motor_RightSetCntsPerSec(float cps);
void Motor_LeftSetMmPerSec(int16 mmps);
void Motor_RightSetMmPerSec(int16 mmps);
uint16 Motor_LeftGetPwm();
uint16 Motor_RightGetPwm();

void Motor_CollectPwmCpsSamples(WHEEL_TYPE wheel, uint8 reverse_pwm, uint8 num_avg_iter, uint16 *pwm_samples, int32 *cps_samples);
void Motor_LeftSetCalibration(int32 *fwd_cps_samples, uint16 *fwd_pwm_samples, int32 *bwd_cps_samples, uint16 *bwd_pwm_samples);
void Motor_RightSetCalibration(int32 *fwd_cps_samples, uint16 *fwd_pwm_samples, int32 *bwd_cps_samples, uint16 *bwd_pwm_samples);

#endif

/* [] END OF FILE */
