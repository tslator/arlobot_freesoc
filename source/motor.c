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
#include "i2c.h"

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
    if (MOTOR_DEBUG_CONTROL_ENABLED)
    {
        DEBUG_PRINT_ARG("%s: %d \r\n", motor->name, motor->get_pwm());
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
    
    I2c_SetDeviceStatusBit(STATUS_HB25_CNTRL_INIT_BIT);
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
    
    I2c_ClearDeviceStatusBit(STATUS_HB25_CNTRL_INIT_BIT);
}

/*---------------------------------------------------------------------------------------------------
 * Name: RampDown
 * Description: Ramps down the motor velocity over the given time (in milliseconds).
 * Parameters: motor - the relevant motor, left or right
 *             millis - the time in milliseconds over which the motor speed will be reduced.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void RampDown(MOTOR_TYPE *motor, uint32 millis)
{
    #define MAX_PWM_STEP (10)
    static PWM_TYPE pwm;
    static int16 delta_pwm;
    static uint32 time_delay;
    static int16 pwm_step;
    
    pwm = motor->get_pwm();    
    if (pwm == PWM_STOP)
    {
        return;
    }
    
    delta_pwm = ((int16) PWM_STOP) - ((int16) pwm);
    time_delay = (millis * MAX_PWM_STEP)/((uint32) abs(delta_pwm));
    pwm_step = delta_pwm > 0 ? MAX_PWM_STEP : -MAX_PWM_STEP;
    
    do
    {
        pwm += pwm_step;
        motor->set_pwm(pwm);
        CyDelay(time_delay);        
    } while (pwm != PWM_STOP);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_LeftRampDown
 * Description: Ramps down the left motor velocity over the given time (in milliseconds).
 * Parameters: millis - the time in milliseconds over which the motor speed will be reduced.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_LeftRampDown(uint32 millis)
{
    RampDown(&left_motor, millis);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Motor_RightRampDown
 * Description: Ramps down the right motor velocity over the given time (in milliseconds).
 * Parameters: millis - the time in milliseconds over which the motor speed will be reduced.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Motor_RightRampDown(uint32 millis)
{
    RampDown(&right_motor, millis);
}


/* [] END OF FILE */
