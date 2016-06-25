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

#define USBFS_DEVICE    (0u)

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)
#define LINE_STR_LENGTH     (20u)


void Ser_Init()
{
}

void Ser_Start()
{
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
}

void Ser_PutString(char *str)
{
    if (0u != USBUART_GetConfiguration())
    {    
        if (USBUART_CDCIsReady())
        {
            USBUART_PutString(str);
        }
    }
}

uint8 Ser_IsDataReady()
{
    uint8 result = 0;
    if (0u != USBUART_GetConfiguration())
    {    
        if (USBUART_CDCIsReady())
        {
            result = USBUART_DataIsReady();
        }
    }
    
    return result;
}

void Ser_ReadFloat(float *value)
{
    if (0u != USBUART_GetConfiguration())
    {    
        if (USBUART_CDCIsReady())
        {
            USBUART_GetAll((uint8 *) value);
        }
    }
}

void Ser_ReadString(char *str)
{
    if (0u != USBUART_GetConfiguration())
    {    
        if (USBUART_CDCIsReady())
        {
            if (USBUART_DataIsReady())
            {
                USBUART_GetAll((uint8 *) str);
            }
        }
    }
}

char Ser_ReadChar()
{
    char data = 0;
    if (0u != USBUART_GetConfiguration())
    {    
        if (USBUART_CDCIsReady())
        {
            if (USBUART_DataIsReady())
            {
                data = USBUART_GetChar();
            }
        }
    }
    
    return data;
}

void Ser_FlushRead()
{
    char data[10];
    if (0u != USBUART_GetConfiguration())
    {    
        if (USBUART_CDCIsReady())
        {
            USBUART_GetAll((uint8 *) data);
        }
    }
}

/* [] END OF FILE */
