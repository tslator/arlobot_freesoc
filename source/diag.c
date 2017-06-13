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
 * Description
 *-------------------------------------------------------------------------------------------------*/
// Add a description of the module

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "diag.h"
#include "time.h"
#include "control.h"
#include "utils.h"
#include "config.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define DIAG_HEARTBEAT_MS SAMPLE_TIME_MS(HEARTBEAT_RATE)
#define DIAG_STATUS_LED_MS SAMPLE_TIME_MS(STATUS_LED_RATE)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static uint32 last_heartbeat_time;
static uint32 heartbeat;
static uint32 last_led_time;

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
    static uint32 delta_time = 0;
    uint32 now;
    
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
