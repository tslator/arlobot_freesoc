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
 * Description: 
 *-------------------------------------------------------------------------------------------------*/

#ifndef CONCONFIG_H
#define CONCONFIG_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include "freesoc.h"

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef enum { CONCONFIG_MOTOR_BIT=0x0001, CONCONFIG_PID_BIT=0x0002, CONCONFIG_BIAS_BIT=0x0004, CONCONFIG_DEBUG_BIT=0x0008, CONCONFIG_STATUS_BIT=0x0010, CONCONFIG_PARAMS_BIT=0x0020} CONCONFIG_BITS_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void DispConfig_Init(void);
void DispConfig_Start(void);
void DispConfig_InitConfigDebug(BOOL enable, UINT16 mask);
void DispConfig_InitConfigShow(UINT16 mask, BOOL plain_text);
void DispConfig_InitConfigClear(UINT16 mask, BOOL plain_text);

#endif