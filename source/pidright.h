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
   Description: This module provides the implementation of the right wheel PID.
 *-------------------------------------------------------------------------------------------------*/    

#ifndef RIGHTPID_H
#define RIGHTPID_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "freesoc.h"
#include "pidtypes.h"
#include "consts.h"
    
/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
// Right PID min/max in count/sec
#define RIGHTPID_MIN (0)
#define RIGHTPID_MAX (min(MAX_WHEEL_FORWARD_COUNT_PER_SEC, abs(MAX_WHEEL_BACKWARD_COUNT_PER_SEC)))
    

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void RightPid_Init();
void RightPid_Start();
void RightPid_Process();

void RightPid_SetGains(FLOAT kp, FLOAT ki, FLOAT kd, FLOAT kf);
void RightPid_GetGains(FLOAT* const kp, FLOAT* const ki, FLOAT* const kd, FLOAT* const kf);

void RightPid_SetTarget(GET_TARGET_FUNC_TYPE target);
void RightPid_RestoreTarget();

void RightPid_Reset();
void RightPid_Enable(BOOL value);
void RightPid_Bypass(BOOL value);

#endif

/* [] END OF FILE */
