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

#include "debug.h"


void Debug_Init()
{
#ifdef COMMS_DEBUG_ENABLED    
    memset(formatted_string, 0, sizeof(formatted_string));

#if defined LEFT_ENC_DUMP_ENABLED || defined RIGHT_ENC_DUMP_ENABLED
    debug_control_enabled |= DEBUG_LEFT_ENCODER_ENABLE_BIT | DEBUG_RIGHT_ENCODER_ENABLE_BIT;
#endif
#if defined LEFT_PID_DUMP_ENABLED || defined RIGHT_PID_DUMP_ENABLED
    debug_control_enabled |= DEBUG_LEFT_PID_ENABLE_BIT | DEBUG_RIGHT_PID_ENABLE_BIT;
#endif
#if defined MOTOR_DUMP_ENABLED
    debug_control_enabled |= DEBUG_LEFT_MOTOR_ENABLE_BIT | DEBUG_RIGHT_MOTOR_ENABLE_BIT;
#endif   
#if defined ODOM_DUMP_ENABLED
    debug_control_enabled |= DEBUG_ODOM_ENABLE_BIT;
#endif
#if defined ENC_UPDATE_DELTA_ENABLED || defined PID_UPDATE_DELTA_ENABLED || defined ODOM_UPDATE_DELTA_ENABLED || defined MAIN_LOOP_DELTA_ENABLED
    debug_control_enabled |= DEBUG_SAMPLE_ENABLE_BIT;
#endif

#endif    
}

void Debug_Start()
{
}

/* [] END OF FILE */
