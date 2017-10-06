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
   Description: This module provides the implementation of the linear velocity PID.
 *-------------------------------------------------------------------------------------------------*/    

#ifndef UNIPID_H
#define UNIPID_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <project.h>
#include "pid.h"
    
/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
    

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void UniPid_Init();
void UniPid_Start();
void UniPid_Process();

void UniPid_SetGains(float theta_kp, float theta_ki, float theta_kd, float theta_kf);
void UniPid_GetGains(float *theta_kp, float *theta_ki, float *theta_kd, float *theta_kf);

void UniPid_Reset();
void UniPid_Enable(uint8 enable);
void UniPid_Bypass(uint8 bypass);


#endif

/* [] END OF FILE */
