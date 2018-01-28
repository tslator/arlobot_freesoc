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
#include "uartif.h"
#include "uart.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define MAX_BUFFER_SIZE (100)

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void UARTIF_Init(void)
{

}

void UARTIF_Start(void)
{
    UART_Start();
}

void UARTIF_Update(void)
{

}

void UARTIF_PutString(CHAR* const str)
{
    UART_PutString(str);
}

UINT8 UARTIF_GetAll(CHAR* const data)
{
    CHAR value;
    UINT8 offset;

    offset = 0;
    value = UART_GetChar();
    while (value != 0 && offset < MAX_BUFFER_SIZE)
    {
        data[offset] = value;
        offset++;
    }

    return offset;
}

UINT8 UARTIF_GetChar(void)
{
    return UART_GetChar();
}

void UARTIF_PutChar(CHAR value)
{
    UART_PutChar(value);
}
