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
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <stdarg.h>
#include "conserial.h"
#include "serial.h"
#include "usbif.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
static SERIAL_DEVICE_TYPE device = {
    USBIF_PutString,
    USBIF_GetAll,
    USBIF_GetChar,
    USBIF_PutChar
};

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void ConSer_Init(void)
{
    USBIF_Init();
}

void ConSer_Start(void)
{
    USBIF_Start();
}

void ConSer_Update(void)
{
    USBIF_Update();
}

INT16 ConSer_ReadLine(CHAR * const str, BOOL echo, UINT16 max_length)
{
    return Ser_ReadLine(device, str, echo, max_length);
}

void ConSer_WriteLine(BOOL new_line, CHAR const * const str, ...)
{
    va_list args;
    va_start(args, str);
    Ser_WriteLine(device, new_line, str, args);
    va_end(args);
}