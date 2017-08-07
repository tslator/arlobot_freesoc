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
#include <project.h>

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/    
typedef float (*GET_ENCODER_FUNC_TYPE)();

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Encoder_Init();
void Encoder_Start();
void Encoder_Update();

float Encoder_LeftGetCntsPerSec();
float Encoder_RightGetCntsPerSec();

float Encoder_LeftGetMeterPerSec();
float Encoder_RightGetMeterPerSec();

int32 Encoder_LeftGetCount();
int32 Encoder_RightGetCount();

float Encoder_LeftGetDeltaCount();
float Encoder_RightGetDeltaCount();

float Encoder_LeftGetDeltaDist();
float Encoder_RightGetDeltaDist();

float Encoder_GetCenterDist();

void Encoder_Reset();

#endif
/* [] END OF FILE */
