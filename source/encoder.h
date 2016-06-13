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

#ifndef ENCODER_H
#define ENCODER_H

#include <project.h>

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

void Encoder_Reset();

#endif
/* [] END OF FILE */
