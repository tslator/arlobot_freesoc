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
   Description: This module provides macros and functions for diagnostics.
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "diag.h"
#include "time.h"
#include "control.h"
#include "utils.h"
#include "config.h"
#include "consts.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define DIAG_HEARTBEAT_MS SAMPLE_TIME_MS(HEARTBEAT_RATE)
#define DIAG_STATUS_LED_MS SAMPLE_TIME_MS(STATUS_LED_RATE)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static UINT32 last_heartbeat_time;
static UINT32 heartbeat;
static UINT32 last_led_time;

/*---------------------------------------------------------------------------------------------------
 * Name: Diag_Init
 * Description: Initializes the mainloop cycle pin state and the heartbeat variable  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Diag_Init()
{
    Diag_Pin_Write(0);
    last_heartbeat_time = millis();
    heartbeat = 0;
    last_led_time = millis();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Diag_Start
 * Description: None  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Diag_Start()
{

}

/*---------------------------------------------------------------------------------------------------
 * Name: Diag_Update
 * Description: Updates the heartbeat timer and writes the heartbeat counter to I2C.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 void Diag_Update()
{
    static UINT32 delta_time = 0;
    UINT32 now;
    
    now = millis();
    delta_time = now - last_heartbeat_time;    
    // Increment a counter that can be read over I2C
    if (delta_time > DIAG_HEARTBEAT_MS)
    {
        last_heartbeat_time = millis();
        
        heartbeat++;
        Control_UpdateHeartbeat(heartbeat);
    }
    
    delta_time = now - last_led_time;
    if (delta_time > DIAG_STATUS_LED_MS)
    {
        last_led_time = millis();
        
        LED_Write(~LED_Read());
    }
}

/* [] END OF FILE */
