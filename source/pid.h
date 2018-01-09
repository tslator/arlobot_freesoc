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
   Description: This module provides a general abstraction for PID control.  There are PIDs for each
   wheel (left, right) and for linear and angular velocity.
 *-------------------------------------------------------------------------------------------------*/

#ifndef PID_H
#define PID_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "freesoc.h"
#include "pidtypes.h"
#include "calstore.h"
#include "pid_controller.h"
    
/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Pid_Init();
void Pid_Start();
void Pid_Update();
void Pid_SetLeftRightTarget(GET_TARGET_FUNC_TYPE left_target, GET_TARGET_FUNC_TYPE right_target);
void Pid_RestoreLeftRightTarget();
void Pid_Reset();
void Pid_Enable(BOOL left, BOOL right, BOOL uni);
void Pid_Bypass(BOOL left, BOOL right, BOOL uni);
void Pid_BypassAll(BOOL bypass);
BOOL Pid_SetGains(PIDControl* const p_pid, CAL_PID_TYPE* const p_gains);

void DumpPid(char* const name, UINT16 debug_bit, PIDControl* const pid);

#endif

/* [] END OF FILE */
