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
 *
 * Provides a wrapper for the Cypress USB serial component.
 *
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include "serial.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define USBFS_DEVICE    (0u)

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)
#define LINE_STR_LENGTH     (20u)

#define MAX_STRING_LENGTH (255)

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_Init
 * Description: Initializes the serial module
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_Init()
{
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_Start
 * Description: Starts the serial module
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_Update
 * Description: Pulses the USB serial component to keep it alive.  Call from the main loop.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_Update()
{
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
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_PutString
 * Description: Sends a string to the serial port
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_PutString(char *str)
{
    /* Note: Consider implementing variable argument to eliminate the need for sprintf */
    Ser_Update();
    
    if (0u != USBUART_GetConfiguration())
    {
        while (0u == USBUART_CDCIsReady())
        {
        }
        
        USBUART_PutString(str);
    }
}

void Ser_PutStringFormat(const char *fmt, ...)
{
    char str[MAX_STRING_LENGTH];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(str, MAX_STRING_LENGTH, fmt, ap);
    va_end(ap);       
    
    Ser_PutString(str);
    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_ReadData
 * Description: Reads one or more bytes (max 64) of data from the USB serial port.
 * Parameters: None
 * Return: Number of bytes read.
 * 
 *-------------------------------------------------------------------------------------------------*/
uint8 Ser_ReadData(uint8_t *data)
{
    uint8 count = 0;
    uint8 buffer[64];
    
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            /* Read received data and re-enable OUT endpoint. */
            count = USBUART_GetAll(buffer);

            if (0u != count)
            {
                memset(data, 0, 64);
                memcpy(data, buffer, count);
            }
        }
    }
    
    return count;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_ReadByte
 * Description: Returns a byte from the serial port.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
uint8 Ser_ReadByte()
{
    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            /* Read received data and re-enable OUT endpoint. */
            return USBUART_GetChar();
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_WriteByte
 * Description: Writes a byte to the serial port.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_WriteByte(uint8 value)
{
    if (0u != USBUART_GetConfiguration())
    {
        while (0u == USBUART_CDCIsReady())
        {
        }
        
        USBUART_PutChar(value);
    }
}

/* [] END OF FILE */
