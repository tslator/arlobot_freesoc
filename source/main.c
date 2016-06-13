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
#include <stdio.h>
#include <project.h>
#include "debug.h"
#include "diag.h"
#include "control.h"
#include "time.h"
#include "i2c.h"
#include "encoder.h"
#include "motor.h"
#include "pid.h"
#include "odom.h"
#include "ultrasonic.h"
#include "infrared.h"
#include "cal.h"

int main()
{       
    CyGlobalIntEnable;      /* Enable global interrupts */
    
    /* Start this right away so that we debug as soon as possible */
    Ser_Init();
    Debug_Init();
    Debug_Start();
    
    Diag_Init();
    Diag_Start();
        
    Control_Init();
    Cal_Init();
    Time_Init();
    I2c_Init();
    Encoder_Init();
    Motor_Init();
    Pid_Init(Control_LeftGetCmdVelocity, Control_RightGetCmdVelocity);
    Odom_Init();
    Ultrasonic_Init();
    Infrared_Init();

    Ser_Start();
    Control_Start();
    Cal_Start();
    Time_Start();
    I2c_Start();
    Encoder_Start();
    Motor_Start();
    Pid_Start();
    Odom_Start();
    Ultrasonic_Start();
    Infrared_Start();

    Ser_PutString("Hello, my name is ArloSoc!\r\n");
    Ser_PutString("I am the microcontroller for Arlobot.\r\n");
    Ser_PutString("I'm entering my main loop now!\r\n");
    
    for(;;)
    {
        LOOP_START();
        
        /* Update any control changes */
        Control_Update();
        /* Update encoder-related values */
        Encoder_Update();
        /* Apply the velocity command to PID */
        Pid_Update();
        /* Update the odometry calculation */
        Odom_Update();
        /* Diagnostic update */
        Diag_Update();
        
        LOOP_END();
    }
}

/* [] END OF FILE */
