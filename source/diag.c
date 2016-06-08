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

#include "diag.h"
#include "time.h"
#include "i2c.h"

#define HEARTBEAT_PERIOD (1000)

static uint32 last_heartbeat_time;
static uint32 heartbeat;

void Diag_Init()
{
    Mainloop_Pin_Write(0);
}

void Diag_Start()
{

}

void Diag_Update()
{
    uint32 now;
    
    now = millis();
    
    // Increment a counter that can be read over I2C
    //if ((now - last_heartbeat_time) > HEARTBEAT_PERIOD)
    //{
    //    heartbeat++;
    //    I2c_WriteHeartbeat(heartbeat);
    //}
}

/* [] END OF FILE */
