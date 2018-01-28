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
   Description: Implements a serial console interface that is used for displaying text while the
   console is active.  By default, the serial console uses the USBUART component and the 'USER'
   USB connector on the Freesoc.
 *-------------------------------------------------------------------------------------------------*/    
#ifndef CONSERIAL_H
#define CONSERIAL_H
    
#include "freesoc.h"

void ConSer_Init(void);
void ConSer_Start(void);
void ConSer_Update(void);
INT16 ConSer_ReadLine(CHAR * const str, BOOL echo, UINT16 max_length);
void ConSer_WriteLine(BOOL new_line, CHAR const * const str, ...);

#endif