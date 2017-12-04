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
#include "utils.h"
#include "consts.h"

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

#define CDC_IS_READY_TIMEOUT (10)


static BOOL is_connected;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static BOOL WaitForCDCIsReady(UINT32 timeout)
{
    /* The purpose of this routine is to check if CDC is ready in a non-blocking manner.
       If a serial port is connected to the USB then CDC is ready should return immediately; however
       if nothing is connected to the USB then obviously we're not interested in the output and
       so we don't want the main loop to be blocked by calls to check the CDC.
    */
    UINT32 tick;
    
    tick = timeout;

    while (tick > 0)
    {    
        if (0u != USBUART_GetConfiguration())
        {
            while (0 == USBUART_CDCIsReady())
            {
            }
            is_connected = TRUE;
            return is_connected;
        }
        tick--;
        CyDelayUs(1);
    }
    
    is_connected = FALSE;
    return is_connected;    
}

static void Initialize(void)
{
    UINT32 timeout = 10;
    
    /* During startup or any time there is a configuration change detected via Ser_Update,
       wait for a configuration to set and then initialize the CDC and set as connected.
    
       If no configuration is detected then consider the USBUART unconnected.
    
     */
        
    /* Initialize IN endpoints when device is configured. */
    while (0u == USBUART_GetConfiguration() && timeout > 0)
    {
        timeout--;
        CyDelayUs(1);
    }
    
    if (timeout > 0)
    {        
        /* Note: This path occurs when the USB cable is attached (at any time).
           Cable insertion registers as a configuration change.
         */
        (void) USBUART_CDC_Init();
        is_connected = TRUE;
    }
    else
    {
        /* Note: This path occurs when the USB cable is not attached at Psoc startup */
        is_connected = FALSE;
    }
    
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
    is_connected = FALSE;
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

    Initialize();
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
    /* Only check if there has been a configuration change.
       This occurs when a USB cable is plugged into the device.
       Note: No events are signaled when the plug is removed.
    
       The USB interface must be 'pumped' which is done by the routines that read and write.  
       The purpose of this routine is to dynamically discover changes to the USB interface.
     */
    if (0u != USBUART_IsConfigurationChanged())
    {
        Initialize();
    }    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_PutString
 * Description: Sends a string to the serial port
 * Parameters: str - the string to be output
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_PutString(char *str)
{
    if (WaitForCDCIsReady(CDC_IS_READY_TIMEOUT))
    {
        /* Note: I have discovered that if the USB cable is removed after entering this function
           it can get stuck in an internal while loop and remain there until the cable is plugged
           in again.  This is an annoyance and can be resolved by plugging in the cable.
           Just be aware.
         */
        USBUART_PutString(str);
    }    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_PutStringFormat
 * Description: Sends a string containing format specifiers to the serial port
 * Parameters: fmt - the string to be output
 *             ... - variable argument list
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
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

    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
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
 *             echo - echos characters to serial port if TRUE.
 *             max_length - the maximum length of the line (maximum is 64).
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT8 Ser_ReadLine(CHAR *line, UINT8 echo, UINT8 max_length)
{
    static CHAR line_data[64];
    static UINT8 char_offset = 0;
    UINT8 length;
    UINT8 line_length;
    CHAR ch;
    
    length = 0;
    line_length = max_length == 0 ? 64 : min(max_length, 64);

    ch = Ser_ReadByte();
    if (ch == 0x00)
    {
        return length;
    }
    else if (ch == '\n' || ch == '\r')
    {   
        length = char_offset;
        line_data[char_offset] = '\0';
        memcpy(line, line_data, length);
        memset(line_data, 0, 64);
        char_offset = 0;
    }
    else
    {
        line_data[char_offset] = ch;
        if (echo)
        {
            Ser_WriteByte(ch);
        }
        char_offset++;

        if (char_offset == line_length)
        {
            char_offset--;
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
    if (WaitForCDCIsReady(CDC_IS_READY_TIMEOUT))
    {
        USBUART_PutChar(value);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_GetConnectState
 * Description: Returns the connection state of the serial interface.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT8 Ser_GetConnectState(void)
{
    return is_connected;
}

/* [] END OF FILE */
