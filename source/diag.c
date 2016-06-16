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
#include "utils.h"
#include "config.h"

#define DIAG_HEARTBEAT_MS SAMPLE_TIME_MS(HEARTBEAT_RATE)

static uint32 last_heartbeat_time;

void Diag_Init()
{
    Mainloop_Pin_Write(0);
    last_heartbeat_time = millis();
}

void Diag_Start()
{

}

void Diag_Update()
{
    //static uint32 delta_time;
    
    //delta_time = millis() - last_heartbeat_time;
    // Increment a counter that can be read over I2C
    //if (delta_time > DIAG_HEARTBEAT_MS)
    //{
    //    last_heartbeat_time = millis();
        I2c_UpdateHeartbeat();
    //}
}

/* [] END OF FILE */
