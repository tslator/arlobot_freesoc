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
uint8 Ser_Write(uint8 *data, size_t num);
uint8 Ser_Read(uint8 *data, size_t *num);
void Ser_PutString(char *str);

/* [] END OF FILE */
