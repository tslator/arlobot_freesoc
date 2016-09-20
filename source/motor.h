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

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <project.h>
#include "config.h"
#include "cal.h"

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/    
typedef void (*SET_MOTOR_PWM_FUNC_TYPE)(uint16 pwm);
typedef uint16 (*GET_MOTOR_PWM_FUNC_TYPE)();
typedef void (*RAMP_DOWN_PWM_FUNC_TYPE)(uint32 millis);

    
/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Motor_Init();
void Motor_Start();
void Motor_Stop();

void Motor_LeftSetPwm(uint16 pwm);
void Motor_RightSetPwm(uint16 pwm);
void Motor_SetPwm(uint16 left_pwm, uint16 right_pwm);

uint16 Motor_LeftGetPwm();
uint16 Motor_RightGetPwm();
void Motor_GetPwm(uint16 *left, uint16 *right);

void Motor_LeftRampDown(uint32 millis);
void Motor_RightRampDown(uint32 millis);

#endif

/* [] END OF FILE */
