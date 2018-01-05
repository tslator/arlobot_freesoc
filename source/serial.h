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
#include "freesoc.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define MAX_STRING_LENGTH (127)
#define MAX_LINE_LENGTH (MAX_STRING_LENGTH)

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Ser_Init();
void Ser_Start();

void Ser_PutString(CHAR const * const str);
void Ser_PutStringFormat(CHAR const * const fmt, ...);
UINT8 Ser_ReadData(CHAR * const data);
UINT8 Ser_ReadByte();
INT8 Ser_ReadLine(CHAR* const line, BOOL echo, UINT8 max_length);
void Ser_WriteByte(UINT8 value);
void Ser_WriteLine(CHAR* const line, BOOL newline);

UINT8 Ser_GetConnectState(void);
#endif

/* [] END OF FILE */
