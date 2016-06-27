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

#ifdef COMMS_DEBUG_ENABLED

/* Debug enable/disable bits */
#define DEBUG_ENCODER_ENABLE_BIT    (0x0001)
#define DEBUG_PID_ENABLE_BIT        (0x0002)
#define DEBUG_MOTOR_ENABLE_BIT      (0x0004)
#define DEBUG_ODOM_ENABLE_BIT       (0x0008)
#define DEBUG_SAMPLE_ENABLE_BIT     (0x0010)
    

//#define LEFT_PID_DUMP_ENABLED
//#define LEFT_ENC_DUMP_ENABLED
//#define RIGHT_PID_DUMP_ENABLED
//#define RIGHT_ENC_DUMP_ENABLED
//#define MOTOR_DUMP_ENABLED
#define ODOM_DUMP_ENABLED
//#define ENC_UPDATE_DELTA_ENABLED
//#define PID_UPDATE_DELTA_ENABLED
//#define ODOM_UPDATE_DELTA_ENABLED
//#define MAIN_LOOP_DELTA_ENABLED
    
uint16 debug_control_enabled;
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
#define INSIDE_DEBUG_DETAIL(...)    do {                                                                \
                                    snprintf(formatted_string, sizeof(formatted_string), __VA_ARGS__);  \
                                    Ser_PutString(formatted_string);                                    \
                                    } while (0)

#define INSIDE_DEBUG(...)           do {                                                                     \
                                    snprintf(formatted_string, sizeof(formatted_string), __VA_ARGS__);  \
                                    Ser_PutString(formatted_string);                                    \
                                    } while (0)
                                
#define ENCODER_DEBUG_CONTROL_ENABLED   (debug_control_enabled & DEBUG_ENCODER_ENABLE_BIT)
#define PID_DEBUG_CONTROL_ENABLED       (debug_control_enabled & DEBUG_PID_ENABLE_BIT)
#define MOTOR_DEBUG_CONTROL_ENABLED     (debug_control_enabled & DEBUG_MOTOR_ENABLE_BIT)
#define ODOM_DEBUG_CONTROL_ENABLED      (debug_control_enabled & DEBUG_ODOM_ENABLE_BIT)
#define SAMPLE_DEBUG_CONTROL_ENABLED    (debug_control_enabled & DEBUG_SAMPLE_ENABLE_BIT)

    
#define DEBUG_PRINT_STR(_fmt)               INSIDE_DEBUG(_fmt)
#define DEBUG_PRINT_ARG(_fmt, ...)          INSIDE_DEBUG(_fmt, __VA_ARGS__)
#define DEBUG_PRINT_LINEFILE(_fmt, ...)     INSIDE_DEBUG_DETAIL(WHERESTR _fmt, WHEREARG,__VA_ARGS__)
#define DEBUG_PRINT_LEVEL(X, _fmt, ...)     if((DEBUG_LEVEL & X) == X) \
                                                INSIDE_DEBUG_DETAIL(WHERESTR _fmt, WHEREARG,__VA_ARGS__)

    
#define DEBUG_DELTA_TIME(name, delta) do                                                \
                                      {                                                 \
                                        if (SAMPLE_DEBUG_CONTROL_ENABLED)               \
                                        {                                               \
                                          char buf[100];                                \
                                          sprintf(buf, "%s: %ld \r\n", name, delta);    \
                                          Ser_PutString(buf);                           \
                                        }                                               \
                                      } while (0);

    
#else
    
#undef LEFT_PID_DUMP_ENABLED
#undef LEFT_ENC_DUMP_ENABLED
#undef RIGHT_PID_DUMP_ENABLED
#undef RIGHT_ENC_DUMP_ENABLED
#undef MOTOR_DUMP_ENABLED
#undef ODOM_DUMP_ENABLED
#undef ENC_UPDATE_DELTA_ENABLED
#undef PID_UPDATE_DELTA_ENABLED
#undef ODOM_UPDATE_DELTA_ENABLED
#undef MAIN_LOOP_DELTA_ENABLED
    
#undef ENCODER_DEBUG_CONTROL_ENABLED
#undef PID_DEBUG_CONTROL_ENABLED
#undef MOTOR_DEBUG_CONTROL_ENABLED
#undef ODOM_DEBUG_CONTROL_ENABLED
#undef SAMPLE_DEBUG_CONTROL_ENABLED
    
#define DEBUG_PRINT_STR(_fmt)
#define DEBUG_PRINT_ARG(_fmt, ...)
#define DEBUG_PRINT_LINEFILE(X, _fmt, ...)
#define DEBUG_PRINT_LEVEL(X, _fmt, ...)
#define DEBUG_DELTA_TIME(name, delta)

#endif
    

void Debug_Init();
void Debug_Start();

#endif

/* [] END OF FILE */

