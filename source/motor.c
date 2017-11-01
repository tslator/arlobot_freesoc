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

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdio.h>
#include "motor.h"
#include "config.h"
#include "utils.h"
#include "encoder.h"
#include "time.h"
#include <math.h>
#include "utils.h"
#include "pwm.h"
#include "cal.h"
#include "debug.h"
#include "control.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef MOTOR_DUMP_ENABLED
#define MOTOR_DUMP(motor)   DumpMotor(motor)
#else    
#define MOTOR_DUMP(motor)
#endif

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define CAL_DATA_SIZE (CAL_NUM_SAMPLES)

/* Add a link to the HB25 data sheet
   Ref: https://www.parallax.com/sites/default/files/downloads/HB-25-Motor-Controller-Guide-v1.3.pdf
 */

#define HB25_ENABLE     (0)
#define HB25_DISABLE    (1)


/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/    
typedef void (*HB25_ENABLE_FUNC_TYPE)(uint8);
typedef void (*START_PWM_FUNC_TYPE)();
typedef void (*STOP_PWM_FUNC_TYPE)();

typedef struct _motor_tag
{
    char name[6];
    HB25_ENABLE_FUNC_TYPE   enable;
    START_PWM_FUNC_TYPE     start;
    STOP_PWM_FUNC_TYPE      stop;
    SET_MOTOR_PWM_FUNC_TYPE set_pwm;
    GET_MOTOR_PWM_FUNC_TYPE get_pwm;
} MOTOR_TYPE;

static MOTOR_TYPE left_motor = {
    "left",
    Left_HB25_Enable_Pin_Write,
    Left_HB25_PWM_Start,
    Left_HB25_PWM_Stop,
    Left_HB25_PWM_WriteCompare,
    Left_HB25_PWM_ReadCompare
};

static MOTOR_TYPE right_motor = {
    "right",
    Right_HB25_Enable_Pin_Write,
    Right_HB25_PWM_Start,
    Right_HB25_PWM_Stop,
    Right_HB25_PWM_WriteCompare,
    Right_HB25_PWM_ReadCompare
};

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

#ifdef MOTOR_DUMP_ENABLED
/*---------------------------------------------------------------------------------------------------
 * Name: DumpMotor
 * Description: Writes the current motor fields to the serial port
 * Parameters: motor - the relevant motor, e.g., left or right
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void DumpMotor(MOTOR_TYPE *motor)
{
    if (Debug_IsEnabled(DEBUG_LEFT_MOTOR_ENABLE_BIT | DEBUG_RIGHT_MOTOR_ENABLE_BIT))
    {
#ifdef JSON_OUTPUT_ENABLE
        DEBUG_PRINT_ARG("{\"%s motor\": {\"pwm\":%d}}\r\n",
                        motor->name, 
                        motor->get_pwm()
        );
#else    
        DEBUG_PRINT_ARG("%s: %d \r\n", motor->name, motor->get_pwm());
#endif
    }
}
#endif

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_Init
 * Description: Initializes the left/right motors by disabling the motor driver
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_Init()
{
    left_motor.enable(HB25_DISABLE);
    right_motor.enable(HB25_DISABLE);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_Start
 * Description: Starts the left/right motors by enabling the motor driver, starting the PWM component
 *              and setting the PWM output to stop
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_Start()
{
    /* Enable the power on the HB-25 motor 
       Note: The HB-25 has a specific initialization sequence that is handled in hardware (see HB-25 reference).  
       All that is necessary in software is to enable power to the HB-25 motor controller and start the PWM.  
       The PWM will be enabled on to the HB-25 signal pin when the HB-25 indicates initialization is complete.
     */
    left_motor.enable(HB25_ENABLE);    
    left_motor.start();
    left_motor.set_pwm(PWM_STOP);
    right_motor.enable(HB25_ENABLE);
    right_motor.start();
    right_motor.set_pwm(PWM_STOP);    
    
    Control_SetDeviceStatusBit(STATUS_HB25_CNTRL_INIT_BIT);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_LeftSetPwm
 * Description: Sets the left motor PWM value
 * Parameters: pwm - the pwm value to be set.  Range from 1000 - 2000
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_LeftSetPwm(PWM_TYPE pwm)
{
    pwm = constrain(pwm, MIN_PWM_VALUE, MAX_PWM_VALUE);
    left_motor.set_pwm(pwm);
    MOTOR_DUMP(&left_motor);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_RightSetPwm
 * Description: Sets the right motor PWM value
 * Parameters: pwm - the pwm value to be set.  Range from 1000 - 2000
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_RightSetPwm(PWM_TYPE pwm)
{
    pwm = constrain(pwm, MIN_PWM_VALUE, MAX_PWM_VALUE);
    right_motor.set_pwm(pwm);
    MOTOR_DUMP(&right_motor);
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: Motor_SetPwm
 * Description: Sets the left/right motor PWM value
 * Parameters: left_pwm - the pwm for the left motor.
 *             right_pwm - the pwm for the right motor.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_SetPwm(PWM_TYPE left_pwm, PWM_TYPE right_pwm)
{
    Motor_LeftSetPwm(left_pwm);
    Motor_RightSetPwm(right_pwm);
}
        
/*---------------------------------------------------------------------------------------------------
 * Name: Motor_LeftGetPwm
 * Description: Gets the left motor PWM value
 * Parameters: None
 * Return: uint16
 * 
 *-------------------------------------------------------------------------------------------------*/
PWM_TYPE Motor_LeftGetPwm()
{
    return left_motor.get_pwm();
}
           
/*---------------------------------------------------------------------------------------------------
 * Name: Motor_RightGetPwm
 * Description: Gets the right motor PWM value
 * Parameters: None
 * Return: uint16
 * 
 *-------------------------------------------------------------------------------------------------*/
PWM_TYPE Motor_RightGetPwm()
{
    return right_motor.get_pwm();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_GetPwm
 * Description: Gets the left/right motor PWM values
 * Parameters: left - the left motor pwm value
 *             right - the right motor pwm value
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_GetPwm(PWM_TYPE *left, PWM_TYPE *right)
{
    *left = left_motor.get_pwm();
    *right = right_motor.get_pwm();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_Stop
 * Description: Stops both left/right motors by setting the PWM to stop, stopping the PWM component,
 *              and disabling the motor driver.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_Stop()
{
    left_motor.set_pwm(PWM_STOP);
    right_motor.set_pwm(PWM_STOP);
    left_motor.stop();
    right_motor.stop();
    left_motor.enable(HB25_DISABLE);
    right_motor.enable(HB25_DISABLE);
    
    Control_ClearDeviceStatusBit(STATUS_HB25_CNTRL_INIT_BIT);
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalcRampParams
 * Description: Calculates the parameters used to ramp the motor velocity
 * Parameters: motor - the relevant motor, left or right
 *             time - the time in milliseconds over which the motor speed will be reduced.
 *             target - the target pwm
 *             step - the resolution of the change; modified to match the change in pwm value
 *             time_delay - the time between for each step change
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 static void CalcRampParams(MOTOR_TYPE *motor, uint32 time_ms, PWM_TYPE target, int16 *step, uint32 *time_delay)
{
    #define MIN_PWM_STEP (1)
    #define MAX_PWM_STEP (10)

    PWM_TYPE curr_pwm;
    int16 delta_pwm;
    int8 dir_factor;

    /* Calculate delta pwm and time_delay */
    curr_pwm = motor->get_pwm();    
    delta_pwm = ((int16) target) - ((int16) curr_pwm);

    dir_factor = delta_pwm > 0 ? 1 : -1;

    if (delta_pwm == 0)
    {
        *step = 0;
        *time_delay = 0;
    }
    else
    {
        *step = constrain(*step, MIN_PWM_STEP, min(abs(delta_pwm), MAX_PWM_STEP));
        *time_delay = (time_ms * (uint32) *step) /(uint32) abs(delta_pwm);
        *step *= dir_factor;
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ramp
 * Description: Ramps the motor velocity to the target over the given time (in milliseconds).
 * Parameters: motor - the relevant motor, left or right
 *             time_ms - the time in milliseconds over which the motor speed will be reduced.
 *             target - the target pwm
 *             step - the resolution of the change
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 static void Ramp(MOTOR_TYPE *motor, uint32 time_ms, PWM_TYPE target, PWM_TYPE step)
{
    int16 pwm_step;
    PWM_TYPE pwm;
    uint32 time_delay;

    pwm_step = step;
    CalcRampParams(motor, time_ms, target, &pwm_step, &time_delay);
    pwm = motor->get_pwm();
    
    do
    {
        pwm += pwm_step;
        motor->set_pwm(pwm);
        CyDelay(time_delay);
    } while (pwm != target);

}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_LeftRamp
 * Description: Ramps the left motor velocity to the target over the given time (in milliseconds).
 * Parameters: time - the time in milliseconds over which the motor speed will be adjusted.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 void Motor_LeftRamp(uint32 time, PWM_TYPE target)
{
    Ramp(&left_motor, time, target, 10);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_RightRamp
 * Description: Ramps the right motor velocity to the target over the given time (in milliseconds).
 * Parameters: time - the time in milliseconds over which the motor speed will be adjusted.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 void Motor_RightRamp(uint32 time, PWM_TYPE target)
{
    Ramp(&right_motor, time, target, 10);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_LeftRampDown
 * Description: Ramps down the left motor velocity over the given time (in milliseconds).
 * Parameters: time - the time in milliseconds over which the motor speed will be reduced.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_LeftRampDown(uint32 time)
{
    Ramp(&left_motor, time, PWM_STOP, 10);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_RightRampDown
 * Description: Ramps down the right motor velocity over the given time (in milliseconds).
 * Parameters: time - the time in milliseconds over which the motor speed will be reduced.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_RightRampDown(uint32 time)
{
    Ramp(&right_motor, time, PWM_STOP, 10);
}


/* [] END OF FILE */
