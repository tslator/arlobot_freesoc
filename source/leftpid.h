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

#ifndef LEFTPID_H
#define LEFTPID_H
    
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
void LeftPid_Init();
void LeftPid_Start();
void LeftPid_Process();

void LeftPid_SetGains(float kp, float ki, float kd);
void LeftPid_GetGains(float *kp, float *ki, float *kd);

void LeftPid_SetTarget(GET_TARGET_FUNC_TYPE target);
void LeftPid_RestoreTarget();

void LeftPid_Reset();
void LeftPid_Enable(uint8 enable);


#endif

/* [] END OF FILE */
