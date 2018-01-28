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
#include "freesoc.h"
#include "config.h"
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
#include "usbif.h"
#include "serial.h"
#include "conserial.h"
#include "utils.h"
#include "console.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Main Function
 *-------------------------------------------------------------------------------------------------*/    
int main()
{   
    CyGlobalIntEnable;
    
    Nvstore_Init();
    ConSer_Init();
    Ser_Init();
    Console_Init();
    Debug_Init();
    Debug_Start();    
    Diag_Init();
    Diag_Start();        
    I2CIF_Init();
    Control_Init();
    Time_Init();
    Encoder_Init();
    Motor_Init();
    Pid_Init();
    Odom_Init();
    Cal_Init();
    
    Nvstore_Start();
    USBIF_Start();
    Ser_Start();
    Console_Start();
    I2CIF_Start();
    Control_Start();
    Time_Start();
    Encoder_Start();
    Motor_Start();
    Pid_Start();
    Odom_Start();
    Cal_Start();
                
    Debug_DisableAll();
    
    for(;;)
    {
        MAIN_LOOP_START();

        /* Update any control changes */
        Control_Update();   // reads and validates linear/angular
        
        /* Update encoder-related values */
        Encoder_Update();   // measures current left/right speed

        /* Apply the velocity command to PIDs */
        Pid_Update();       // tracks linear/angular velocity

        /* Update the odometry calculation */
        Odom_Update();      // measures left/right speed, x/y position, heading, linear/angular
        
        /* Diagnostic update */
        Diag_Update();

        /* Keep the USB connection active */
        //USBIF_Update();
        ConSer_Update();
        
        /* Handle Console */
        Console_Update();

        I2CIF_TEST();
        MAIN_LOOP_END();
    }
}

/* [] END OF FILE */
