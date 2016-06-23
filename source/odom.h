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
#ifndef ODOM_H
#define ODOM_H
    
#include <project.h>

void Odom_Init();
void Odom_Start();
void Odom_Update();
void Odom_Reset();
void Odom_GetPosition(float *x, float *y);

#endif

/* [] END OF FILE */
