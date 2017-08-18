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
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
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
#include "cal.h"
#include "nvstore.h"

/*---------------------------------------------------------------------------------------------------
 * Main Function
 *-------------------------------------------------------------------------------------------------*/    
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
        MAIN_LOOP_START();
        
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
        
        MAIN_LOOP_END();
    }
}

/* [] END OF FILE */
