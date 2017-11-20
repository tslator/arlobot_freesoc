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
   Description: This module provides the implementation for reading the encoders and calculating
   motor speed and distance.
 *-------------------------------------------------------------------------------------------------*/    


#ifndef ENCODER_H
#define ENCODER_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "freesoc.h"
    
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/    
typedef FLOAT (*GET_ENCODER_FUNC_TYPE)();
typedef INT32 (*GET_RAW_COUNT_FUNC_TYPE)();
typedef void (*RESET_COUNT_FUNC_TYPE)();

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Encoder_Init();
void Encoder_Start();
void Encoder_Update();

void Encoder_LeftReset();
void Encoder_RightReset();

INT32 Encoder_LeftGetRawCount();
INT32 Encoder_RightGetRawCount();

FLOAT Encoder_LeftGetCntsPerSec();
FLOAT Encoder_RightGetCntsPerSec();

FLOAT Encoder_LeftGetMeterPerSec();
FLOAT Encoder_RightGetMeterPerSec();

INT32 Encoder_LeftGetCount();
INT32 Encoder_RightGetCount();

FLOAT Encoder_LeftGetDeltaCount();
FLOAT Encoder_RightGetDeltaCount();

FLOAT Encoder_LeftGetDeltaDist();
FLOAT Encoder_RightGetDeltaDist();

FLOAT Encoder_GetCenterDist();

FLOAT Encoder_LinearGetVelocity();
FLOAT Encoder_AngularGetVelocity();


void Encoder_Reset();

#endif
/* [] END OF FILE */
