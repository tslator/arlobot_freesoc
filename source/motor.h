/* 
MIT License

Copyright (c) 2017 Tim Slator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*---------------------------------------------------------------------------------------------------
   Description: This module provides functionality for controlling the motors by setting the PWM
   output.
 *-------------------------------------------------------------------------------------------------*/


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

void Motor_LeftRamp(uint32 time, PWM_TYPE target);
void Motor_RightRamp(uint32 time, PWM_TYPE target);
void Motor_LeftRampDown(uint32 time);
void Motor_RightRampDown(uint32 time);

#endif

/* [] END OF FILE */
