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
    CyGlobalIntEnable;
    
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
    Ser_Start(0);
    I2CIF_Start();
    Control_Start();
    Cal_Start();
    Time_Start();
    Encoder_Start();
    Motor_Start();
    Pid_Start();
    Odom_Start();

            
    //Ser_PutString("Hello, my name is ArloSoc!\r\n");
    //Ser_PutString("I am the microcontroller for Arlobot.\r\n");
    //Ser_PutString("I'm entering my main loop now!\r\n");

    Pid_Bypass(0, 0, 0);
    Pid_Enable(1, 1, 0);
    
    for(;;)
    {
        MAIN_LOOP_START();
        
        /* Update any control changes */
        Control_Update();  // reads and validates linear/angular
        
        /* Update encoder-related values */
        Encoder_Update();  // measures current left/right speed

        /* Apply the velocity command to PIDs */
        Pid_Update(); // tracks linear/angular velocity

        /* Update the odometry calculation */
        Odom_Update();      // measures left/right speed, x/y position, heading, linear/angular
        
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
