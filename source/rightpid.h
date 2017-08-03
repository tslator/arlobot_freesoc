/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef RIGHTPID_H
#define RIGHTPID_H
    
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
void RightPid_Init();
void RightPid_Start();
void RightPid_Process();

void RightPid_SetGains(float kp, float ki, float kd);
void RightPid_GetGains(float *kp, float *ki, float *kd);

void RightPid_SetTarget(GET_TARGET_FUNC_TYPE target);
void RightPid_RestoreTarget();

void RightPid_Reset();
void RightPid_Enable(uint8 enable);

#endif

/* [] END OF FILE */
