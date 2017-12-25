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
#include "usbif.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/


#define MAX_STRING_LENGTH (255)

static CHAR line_data[USBUART_BUFFER_SIZE];
static UINT8 char_offset = 0;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------
 * Name: CopyAndTerminateLine
 * Description: Copies the accumulated line data into the output variable, captures the line length
 *              from the character offset, clears the line data, and returns the length.
 * Parameters: line - output line data
 * Return: line length
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 CopyAndTerminateLine(CHAR* const line)
{
    UINT8 length;

    length = char_offset;
    
    if (length > 0)
    {
        memcpy(line, line_data, length);
        line[length] = '\0';
    }

    memset(line_data, 0, USBUART_BUFFER_SIZE);
    char_offset = 0;

    return length;
}

/*---------------------------------------------------------------------------------------------------
 * Name: SetCharData
 * Description: Updates the character data in the line, adjusts the character offset, and echos the
 *              character is echo is TRUE.
 * Parameters: line - output line data
 *             echo - indicates whether the character should be echoed
 * Return: line length
 * 
 *-------------------------------------------------------------------------------------------------*/
static void SetCharData(CHAR ch, UINT8 echo)
{
    line_data[char_offset] = ch;    
    char_offset++;

    if (echo)
    {
        Ser_WriteByte(ch);
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
    memset(line_data, 0, USBUART_BUFFER_SIZE);
    char_offset = 0;
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
    USBIF_Start();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_PutString
 * Description: Sends a string to the serial port
 * Parameters: str - the string to be output
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_PutString(CHAR* const str)
{
    USBIF_PutString(str);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_PutStringFormat
 * Description: Sends a string containing format specifiers to the serial port
 * Parameters: fmt - the string to be output
 *             ... - variable argument list
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_PutStringFormat(const CHAR* const fmt, ...)
{    
    CHAR str[MAX_STRING_LENGTH];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(str, MAX_STRING_LENGTH, fmt, ap);
    va_end(ap);       
    
    Ser_PutString(str);
    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_ReadData
 * Description: Reads one or more bytes (max 64) of data from the USB serial port.
 * Parameters: date read from the serial port
 * Return: Number of bytes read.
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT8 Ser_ReadData(CHAR* const data)
{
    return USBIF_GetAll(data);
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
    return USBIF_GetChar();
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
UINT8 Ser_ReadLine(CHAR* const line, UINT8 echo, UINT8 max_length)
{
    UINT8 length;
    UINT8 line_length;
    CHAR ch;
    BOOL max_chars_read = FALSE;
    
    length = 0;
    line_length = max_length == 0 ? USBUART_BUFFER_SIZE : min(max_length, USBUART_BUFFER_SIZE);
    line_length--;

    max_chars_read = char_offset == line_length;

    ch = Ser_ReadByte();
    if (ch == 0x00)
    {
        /* This means there was no input and since this is a polled routine we return zero length
           to indicate that nothing happened.
         */
        length = -1;
    }
    else if (ch == '\n' || ch == '\r' || max_chars_read)
    {   
        length = CopyAndTerminateLine(line);
        /* max_chars_read is True if we have read max_length characters.
           we will have actually read the next character when this condition
           occurs, so we store it before returning so that we don't drop
           data.
         */
        if (max_chars_read)
        {
            SetCharData(ch, echo);
        }
    }
    else
    {
        SetCharData(ch, echo);
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
    USBIF_PutChar(value);
}

/* [] END OF FILE */
