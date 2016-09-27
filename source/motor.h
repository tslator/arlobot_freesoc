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
typedef void (*SET_MOTOR_PWM_FUNC_TYPE)(PWM_TYPE pwm);
typedef PWM_TYPE (*GET_MOTOR_PWM_FUNC_TYPE)();
typedef void (*RAMP_DOWN_PWM_FUNC_TYPE)(uint32 millis);

    
/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Motor_Init();
void Motor_Start();
void Motor_Stop();

void Motor_LeftSetPwm(PWM_TYPE pwm);
void Motor_RightSetPwm(PWM_TYPE pwm);
void Motor_SetPwm(PWM_TYPE left_pwm, PWM_TYPE right_pwm);

PWM_TYPE Motor_LeftGetPwm();
PWM_TYPE Motor_RightGetPwm();
void Motor_GetPwm(PWM_TYPE *left, PWM_TYPE *right);

void Motor_LeftRampDown(uint32 millis);
void Motor_RightRampDown(uint32 millis);

#endif

/* [] END OF FILE */
