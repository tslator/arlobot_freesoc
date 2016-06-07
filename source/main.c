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

static void WriteValue(uint32 count)
{
    char buf[10];
    
    sprintf(buf, "%ld \r\n", count);
    UART_Debug_PutString(buf);
}


int main()
{       
    CyGlobalIntEnable;      /* Enable global interrupts */
    
    /* Start this right away so that we debug as soon as possible */
    Debug_Init();
    Debug_Start();
    
    Diag_Init();
    Diag_Start();
        
    Control_Init();
    Time_Init();
    I2c_Init();
    Encoder_Init();
    Motor_Init();
    Pid_Init(I2c_LeftReadCmdVelocity, I2c_RightReadCmdVelocity);
    Odom_Init();
    Ultrasonic_Init();
    Infrared_Init();

    Control_Start();
    Time_Start();
    I2c_Start();
    Encoder_Start();
    Motor_Start();
    Pid_Start();
    Odom_Start();
    Ultrasonic_Start();
    Infrared_Start();
    
    UART_Debug_PutString("Hello, my name is ArloSoc!  I am the microcontroller for Arlobot.\r\n");
    
    /* Read calibration data from the Raspberry Pi */
    UART_Debug_PutString("Please wait while I upload calibration data ...\r\n");
    
    UART_Debug_PutString("Calibration complete!\r\n");
    
    UART_Debug_PutString("I'm entering my main loop now!\r\n");

    Mainloop_Pin_Write(0);
    Motor_LeftSetMmPerSec(0);
    Motor_RightSetMmPerSec(0);
    
    //Cal_Update();
    
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
