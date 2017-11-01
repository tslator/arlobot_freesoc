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
#include <project.h>
#include "config.h"
#include "pid_controller.h"
    
/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
        

#define PID_SAMPLE_TIME_MS  SAMPLE_TIME_MS(PID_SAMPLE_RATE)
#define PID_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(PID_SAMPLE_RATE)

    
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef float (*GET_TARGET_FUNC_TYPE)();
typedef float (*GET_INPUT_FUNC_TYPE)();
typedef float (*PID_UPDATE_TYPE)(float target, float input);

typedef struct _pid_tag
{
    char name[8];
    PIDControl pid;
    int sign;
    GET_TARGET_FUNC_TYPE get_target;
    GET_INPUT_FUNC_TYPE get_input;
    PID_UPDATE_TYPE update;
} PID_TYPE;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Pid_Init();
void Pid_Start();
void Pid_Update();
void Pid_SetLeftRightTarget(GET_TARGET_FUNC_TYPE left_target, GET_TARGET_FUNC_TYPE right_target);
void Pid_RestoreLeftRightTarget();
void Pid_Reset();
void Pid_Enable(uint8 left, uint8 right, uint8 uni);
void Pid_Bypass(uint8 left, uint8 right, uint8 uni);
void Pid_BypassAll(uint8 bypass);
uint8 Pid_SetGains(PIDControl *p_pid, CAL_PID_TYPE *p_gains);

void DumpPid(char *name, PIDControl *pid);

#endif

/* [] END OF FILE */
