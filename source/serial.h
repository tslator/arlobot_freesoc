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

#ifndef SERIAL_H
#define SERIAL_H

#include <project.h>

void Ser_Init();
void Ser_Start();
void Ser_Update();

void Ser_PutString(char *str);
uint8 Ser_ReadByte();
void Ser_WriteByte(uint8 value);

#endif

/* [] END OF FILE */
