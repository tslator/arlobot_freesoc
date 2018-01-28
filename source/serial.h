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
   Description: This module provides a wrapper around the USBUART component including variable 
   argument format handling.
 *-------------------------------------------------------------------------------------------------*/    


#ifndef SERIAL_H
#define SERIAL_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdarg.h>
#include "freesoc.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    


/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef void (*PUT_STRING_FUNC)(CHAR * const str);
typedef UINT8 (*GET_ALL_FUNC)(CHAR * const data);
typedef UINT8 (*GET_CHAR_FUNC)(void);
typedef void (*PUT_CHAR_FUNC)(CHAR value);

typedef struct _tag_serial_device
{
    PUT_STRING_FUNC put_string;
    GET_ALL_FUNC get_all;
    GET_CHAR_FUNC get_char;
    PUT_CHAR_FUNC put_char;  
} SERIAL_DEVICE_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Ser_Init();
void Ser_Start();

void Ser_PutString(SERIAL_DEVICE_TYPE device, CHAR const * const str);
void Ser_PutStringFormat(SERIAL_DEVICE_TYPE device, CHAR const * const fmt, ...);
UINT8 Ser_ReadData(SERIAL_DEVICE_TYPE device, CHAR * const data);
UINT8 Ser_ReadByte(SERIAL_DEVICE_TYPE device);
INT16 Ser_ReadLine(SERIAL_DEVICE_TYPE device, CHAR* const line, BOOL echo, UINT16 max_length);
void Ser_WriteByte(SERIAL_DEVICE_TYPE device, UINT8 value);
void Ser_WriteLine(SERIAL_DEVICE_TYPE device, BOOL new_line, CHAR const * const fmt, va_list args);

#endif

/* [] END OF FILE */
