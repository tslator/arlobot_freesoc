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

#include "serial.h"
#include "utils.h"
#include "time.h"

#ifdef COMMS_DEBUG_ENABLED    
#define USBFS_DEVICE    (0u)

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)
#define LINE_STR_LENGTH     (20u)
#endif

typedef struct _line_tag
{
    char string[256];
} LINE_TYPE;

static LINE_TYPE buffer[100];
static uint8 back_offset;
static uint8 front_offset;

void Ser_Init()
{
    memset(buffer, 0, sizeof(LINE_TYPE)*100);
    front_offset = 0;
    back_offset = 0;    
}

void Ser_Start()
{
#ifdef COMMS_DEBUG_ENABLED    
    /* Start USBFS operation with 5-V operation. */
    USBUART_Start(USBFS_DEVICE, USBUART_5V_OPERATION);

    /* Host can send double SET_INTERFACE request. */
    if (0u != USBUART_IsConfigurationChanged())
    {
        /* Initialize IN endpoints when device is configured. */
        if (0u != USBUART_GetConfiguration())
        {
            /* Enumeration is done, enable OUT endpoint to receive data 
             * from host. */
            USBUART_CDC_Init();
        }
    }

    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        USBUART_PutString("USB UART is configured!\r\n");
    }
#endif
}

void Ser_PutString(char *str)
{
#ifdef COMMS_DEBUG_ENABLED    
    if (0u != USBUART_GetConfiguration())
    {    
        if (USBUART_CDCIsReady())
        {
            USBUART_PutString(str);
        }
    }
#else
    str = str;
#endif    
}

void Ser_ReadFloat(float *value)
{
    if (0u != USBUART_GetConfiguration())
    {    
        if (USBUART_CDCIsReady())
        {
            uint32 start = millis();
            while (USBUART_GetCount() == 0 && millis() - start < 5000)
            {
            }
            
            if (USBUART_GetCount() > 0)
            {
                USBUART_GetAll((uint8 *) value);
            }
            else
            {
                *value = 1.0;
            }
        }
    }
}

/* [] END OF FILE */
