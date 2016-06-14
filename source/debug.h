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

#ifndef COMMS_DEBUG_H
#define COMMS_DEBUG_H

#include <project.h>
#include <stdio.h>
#include "config.h"
#include "serial.h"

char formatted_string[256];

#define NONE    0x00
#define DBG     0x1F
#define INFO    0x0F
#define ERR     0x07
#define EMR     0x03
#define CRIT    0x01

#define DEBUG_LEVEL ERR

#define WHERESTR "[FILE : %s, FUNC : %s, LINE : %d]: "
#define WHEREARG __FILE__,__func__,__LINE__
#define INSIDE_DEBUG_DETAIL(...)  do { \
                                snprintf(formatted_string, sizeof(formatted_string), __VA_ARGS__); \
                                Ser_PutString(formatted_string);            \
                           } while (0)

#define INSIDE_DEBUG(...)  do {                                                                     \
                                snprintf(formatted_string, sizeof(formatted_string), __VA_ARGS__);  \
                                Ser_PutString(formatted_string);                                    \
                           } while (0)
                                
#ifdef COMMS_DEBUG_ENABLED
#define DEBUG_PRINT(_fmt, ...)           INSIDE_DEBUG_DETAIL(WHERESTR _fmt, WHEREARG,__VA_ARGS__)
#define DEBUG_PRINT_LEVEL(X, _fmt, ...)  if((DEBUG_LEVEL & X) == X) \
                                             INSIDE_DEBUG_DETAIL(WHERESTR _fmt, WHEREARG,__VA_ARGS__)
#define DEBUG_PRINT_NOARG(_fmt)          INSIDE_DEBUG(_fmt)
#define DEBUG_PRINT_STR(_fmt, ...)       INSIDE_DEBUG(_fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(X, _fmt, ...)
#define DEBUG_PRINT_LEVEL(X, _fmt, ...)
#define DEBUG_PRINT_NOARG(_fmt)
#define DEBUG_PRINT_STR(_fmt, ...)
#endif
    

void Debug_Init();
void Debug_Start();

#endif

/* [] END OF FILE */

