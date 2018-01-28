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

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;
#define MAX_TEXT_BUFFER (5000)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static CHAR line_data[MAX_LINE_LENGTH];
static UINT8 char_offset = 0;
static CHAR str[MAX_TEXT_BUFFER];
static CHAR fmt_str[MAX_TEXT_BUFFER];

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

    memset(line_data, 0, MAX_LINE_LENGTH);
    char_offset = 0;

    return length;
}

/*---------------------------------------------------------------------------------------------------
 * Name: SetCharData
 * Description: Updates the character data in the line, adjusts the character offset, and echos the
 *              character is echo is TRUE.
 * Parameters: device - the serial device to which the characters will be written.
 *             line - output line data
 *             echo - indicates whether the character should be echoed
 * Return: line length
 * 
 *-------------------------------------------------------------------------------------------------*/
static void SetCharData(SERIAL_DEVICE_TYPE device, CHAR ch, BOOL echo)
{
    line_data[char_offset] = ch;    
    char_offset++;

    if (echo)
    {
        Ser_WriteByte(device, ch);
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
    memset(line_data, 0, MAX_LINE_LENGTH);
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
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_PutString
 * Description: Sends a string to the serial port
 * Parameters: device - the serial device to which the characters will be written.
 *             str - the string to be output
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_PutString(SERIAL_DEVICE_TYPE device, CHAR const * const str)
{
    device.put_string((CHAR * const) str);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_PutStringFormat
 * Description: Sends a string containing format specifiers to the serial port
 * Parameters: device - the serial device to which the characters will be written.
 *             fmt - the string to be output
 *             ... - variable argument list
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_PutStringFormat(SERIAL_DEVICE_TYPE device, CHAR const * const fmt, ...)
{    
    CHAR str[MAX_STRING_LENGTH];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(str, MAX_STRING_LENGTH, fmt, ap);
    va_end(ap);       
    
    device.put_string(str);
    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_ReadData
 * Description: Reads one or more bytes (max 64) of data from the specified device.
 * Parameters: device - the serial device to which the characters will be written.
 *             data - characters read from the serial device
 * Return: Number of bytes read.
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT8 Ser_ReadData(SERIAL_DEVICE_TYPE device, CHAR* const data)
{
    return device.get_all(data);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_ReadByte
 * Description: Returns a byte from the serial port.
 * Parameters: device - the serial device to which the characters will be written.
 * Return: Number of characters written
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT8 Ser_ReadByte(SERIAL_DEVICE_TYPE device)
{
    return device.get_char();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_ReadLine
 * Description: Non-blocking reads all serial data until a newline is received.
 *              Note: Putty apparently does not send \n so \r is being used.
 * Parameters: device - the serial device to which the characters will be written.
 *             line - pointer to charater buffer
 *             echo - echos characters to serial port if TRUE.
 *             max_length - the maximum length of the line (maximum is 64).
 * Return: 0 for newline or carriage return
 *         -1 for no input
 *         > 0 for all other
 * 
 *-------------------------------------------------------------------------------------------------*/
INT16 Ser_ReadLine(SERIAL_DEVICE_TYPE device, CHAR* const line, BOOL echo, UINT16 max_length)
{
    UINT8 length;
    UINT8 line_length;
    CHAR ch;
    BOOL max_chars_read = FALSE;
    
    length = 0;
    line_length = max_length == 0 ? MAX_LINE_LENGTH : min(max_length, MAX_LINE_LENGTH);
    line_length--;

    max_chars_read = char_offset == line_length;

    ch = Ser_ReadByte(device);
    if (ch == 0x00)
    {
        /* This means there was no input and since this is a polled routine we return -1 length
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
            SetCharData(device, ch, echo);
        }
        return length;
    }
    else
    {
        SetCharData(device, ch, echo);
    }

    return -1;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_WriteByte
 * Description: Writes a byte to the serial port.
 * Parameters: device - the serial device to which the characters will be written.
 *             value - the character to be written.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_WriteByte(SERIAL_DEVICE_TYPE device, UINT8 value)
{
    device.put_char(value);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Ser_WriteLine
 * Description: Writes a line of characters to the specified serial device.
 * Parameters: device - the serial device to which the characters will be written.
 *             new_line - when TRUE, appends a 'new line', i.e., \r\n, to the end; otherwise nothing.
 *             fmt - a format string
 *             args - a variable argument list
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Ser_WriteLine(SERIAL_DEVICE_TYPE device, BOOL new_line, CHAR const * const fmt, va_list args)
{
    UINT32 length;
    
    /* Break up the string into max string size chunks */
    
    length = strlen(fmt);
    (void)strcpy(fmt_str, fmt);
    
    (void)strcpy(fmt_str + length, new_line == TRUE ? "\r\n" : "");
    length += new_line == TRUE ? 2 : 0;
    fmt_str[length] = '\0';
    
    (void) vsnprintf(str, MAX_TEXT_BUFFER, fmt_str, args);
    
    device.put_string(str);
}

/* [] END OF FILE */
