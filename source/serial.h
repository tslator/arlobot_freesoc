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

#include <project.h>

void Ser_Init();
void Ser_Start();
void Ser_PutString(char *str);
void Ser_ReadFloat(float *value);
uint8 Ser_IsDataReady();
void Ser_ReadString(char *str);
char Ser_ReadChar();
void Ser_FlushRead();

/* [] END OF FILE */
