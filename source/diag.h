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
#include <stdio.h>
#include "serial.h"

#define LOOP_START()    Mainloop_Pin_Write( 1 )
#define LOOP_END()      Mainloop_Pin_Write( 0 )

#define DEBUG_DELTA_TIME(name, delta) do                                                \
                                      {                                                 \
                                          char buf[100];                                \
                                          sprintf(buf, "%s: %ld \r\n", name, delta);    \
                                          Ser_PutString(buf);                           \
                                      } while (0);

void Diag_Init();
void Diag_Start();
void Diag_Update();

/* [] END OF FILE */
