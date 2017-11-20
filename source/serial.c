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

#define CDC_IS_READY_TIMEOUT (10000000)


static UINT8 global_disable;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static void WaitForCDCIsReady(UINT32 timeout)
{
    /* The purpose of this routine is to check if CDC is ready in a non-blocking manner.
       If a serial port is connected to the USB then CDC is ready should return immediately; however
       if nothing is connected to the USB then obviously we're not interested in the output and
       so we don't want the main loop to be blocked by calls to check the CDC.
    */
#ifdef CDC_TIMEOUT       
    UINT32 tick;

    if (timeout > 0)
    {
        tick = timeout;

        while (tick > 0)
        {
            if (USBUART_CDCIsReady())
            {
                break;
            }
            tick--;
        }
    }    
#else
    timeout = timeout;
    
    while (0 == USBUART_CDCIsReady())
    {
    }
#endif
}


/*---------------------------------------------------------------------------------------------------
 * Name: Ser_Init
 * Description: Initializes the serial module
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_Init()
{
    global_disable = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_Start
 * Description: Starts the serial module
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_Start(UINT8 disable)
{
    global_disable = disable;
    
    if (global_disable)
    {
        return;
    }
    
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
    if (global_disable)
    {
        return;
    }
    
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
    if (global_disable)
    {
        return;
    }
    
    if (0u != USBUART_GetConfiguration())
    {
        WaitForCDCIsReady(CDC_IS_READY_TIMEOUT);
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
UINT8 Ser_ReadData(UINT8 *data)
{
    UINT8 count = 0;
    UINT8 buffer[64];
    
    if (global_disable)
    {
        return 0;
    }
    
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
UINT8 Ser_ReadByte()
{
    if (global_disable)
    {
        return 0;
    }
    
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
 * Name: Ser_ReadLine
 * Description: Non-blocking reads all serial data until a newline is received.
 *              Note: Putty apparently does not send \n so \r is being used.
 * Parameters: line - pointer to charater buffer
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT8 Ser_ReadLine(char *line, UINT8 echo)
{
    char ch;
    static UINT8 ii = 0;
    UINT8 length;
    
    length = 0;
    ch = Ser_ReadByte();
    if (ch == 0x00)
    {
        return length;
    }
    else if (ch == '\n' || ch == '\r')
    {   
        length = ii;
        line[ii] = '\0';
        ii = 0;
    }
    else
    {
        line[ii] = ch;
        ii++;
        if (echo)
        {
            Ser_WriteByte(ch);
        }
    }
    
    return length;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_WriteByte
 * Description: Writes a byte to the serial port.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_WriteByte(UINT8 value)
{
    if (global_disable)
    {
        return;
    }
    
    if (0u != USBUART_GetConfiguration())
    {
        WaitForCDCIsReady(CDC_IS_READY_TIMEOUT);        
        USBUART_PutChar(value);
    }
    
}

/* [] END OF FILE */
