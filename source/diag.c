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
#include "diag.h"
#include "time.h"
#include "i2c.h"
#include "utils.h"
#include "config.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define DIAG_HEARTBEAT_MS SAMPLE_TIME_MS(HEARTBEAT_RATE)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static uint32 last_heartbeat_time;

/*---------------------------------------------------------------------------------------------------
 * Name: Diag_Init
 * Description: Initializes the mainloop cycle pin state and the heartbeat variable  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Diag_Init()
{
    Mainloop_Pin_Write(0);
    last_heartbeat_time = millis();
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

*---------------------------------------------------------------------------------------------------
 * Name: Diag_Update
 * Description: Updates the heartbeat timer and writes the heartbeat counter to I2C.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 void Diag_Update()
{
    static uint32 delta_time = 0;
    static uint32 counter = 0;
    
    delta_time = millis() - last_heartbeat_time;
    // Increment a counter that can be read over I2C
    if (delta_time > DIAG_HEARTBEAT_MS)
    {
        last_heartbeat_time = millis();
        I2c_UpdateHeartbeat(counter++);
    }
}

/* [] END OF FILE */
