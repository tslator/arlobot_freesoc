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

#ifndef CONTROL_H
#define CONTROL_H

#include <project.h>
    
typedef void (*COMMAND_FUNC_TYPE)(float *linear, float *angular);

void Control_Init();
void Control_Start();
void Control_Update();
void Control_SetCommandVelocityFunc(COMMAND_FUNC_TYPE cmd);
void Control_RestoreCommandVelocityFunction();
float Control_LeftGetCmdVelocity();
float Control_RightGetCmdVelocity();


#endif

/* [] END OF FILE */
