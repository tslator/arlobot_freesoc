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
#include "i2cif.h"
#include "encoder.h"
#include "motor.h"
#include "pid.h"
#include "odom.h"
#include "ultrasonic.h"
#include "infrared.h"
#include "cal.h"
#include "nvstore.h"
#include "sensor.h"

#ifdef MAIN_LOOP_DELTA_ENABLED
static uint32 main_loop_delta;
static uint32 last_main_loop;
#define MAIN_LOOP_DEBUG_DELTA(delta) DEBUG_DELTA_TIME("main", delta)
#else
#define MAIN_LOOP_DEBUG_DELTA(delta)
#endif    

#ifdef MAIN_LOOP_DELTA_ENABLED
#define MAIN_LOOP_DELTA()       do { \
                                    main_loop_delta = millis() - last_main_loop; \
                                    MAIN_LOOP_DEBUG_DELTA(main_loop_delta) \
                                    last_main_loop = millis(); \
                                while (0);
#else
#define MAIN_LOOP_DELTA()
#endif        

int main()
{       
    /* Think about whether this should be enabled before all of the init and start calls.  Maybe interrupts shouldn't
       be enabled until everything is ready to go?
     */
    CyGlobalIntEnable;      /* Enable global interrupts */
    
    /* Start this right away so that we debug as soon as possible */
    Nvstore_Init();
    Ser_Init();
    Debug_Init();
    Debug_Start();
    
    Diag_Init();
    Diag_Start();
        
    I2CIF_Init();
    Control_Init();
    Cal_Init();
    Time_Init();
    Encoder_Init();
    Motor_Init();
    Pid_Init();
    Odom_Init();

    Nvstore_Start();
    Ser_Start();
    I2CIF_Start();
    Control_Start();
    Cal_Start();
    Time_Start();
    Encoder_Start();
    Motor_Start();
    Pid_Start();
    Odom_Start();

    DEBUG_PRINT_STR("Hello, my name is ArloSoc!\r\n");
    DEBUG_PRINT_STR("I am the microcontroller for Arlobot.\r\n");
    DEBUG_PRINT_STR("I'm entering my main loop now!\r\n");

    for(;;)
    {
        MAIN_LOOP_DELTA();
        LOOP_START();
        MAIN_LOOP_DELTA();
        
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
        /* Handle calibration request */
        Cal_Update();
        
        /* Keep the serial connection active */
        Ser_Update();

        I2CIF_TEST();
        
        LOOP_END();
    }
}

/* [] END OF FILE */
