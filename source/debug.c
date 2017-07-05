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

/*---------------------------------------------------------------------------------------------------
 * Description
 *-------------------------------------------------------------------------------------------------*/
// Add a description of the module

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "debug.h"

static uint16 debug_control_enabled;
static uint16 saved_debug_control_enabled;


/*---------------------------------------------------------------------------------------------------
 * Name: Debug_Init
 * Description: Initializes the debug control enabled variable based on the compile debug settings.  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Debug_Init()
{
#ifdef COMMS_DEBUG_ENABLED  
    memset(formatted_string, 0, sizeof(formatted_string));

#if defined LEFT_ENC_DUMP_ENABLED
    debug_control_enabled |= DEBUG_LEFT_ENCODER_ENABLE_BIT;
#endif
#if defined RIGHT_ENC_DUMP_ENABLED
    debug_control_enabled |= DEBUG_RIGHT_ENCODER_ENABLE_BIT;
#endif
#if defined LEFT_PID_DUMP_ENABLED
    debug_control_enabled |= DEBUG_LEFT_PID_ENABLE_BIT;
#endif
#if  defined RIGHT_PID_DUMP_ENABLED
    debug_control_enabled |= DEBUG_RIGHT_PID_ENABLE_BIT;
#endif
#if defined MOTOR_DUMP_ENABLED
    debug_control_enabled |= DEBUG_LEFT_MOTOR_ENABLE_BIT | DEBUG_RIGHT_MOTOR_ENABLE_BIT;
#endif   
#if defined ODOM_DUMP_ENABLED
    debug_control_enabled |= DEBUG_ODOM_ENABLE_BIT;
#endif
#if defined ENC_UPDATE_DELTA_ENABLED || defined PID_UPDATE_DELTA_ENABLED || defined MAIN_LOOP_DELTA_ENABLED || defined SENSOR_UPDATE_DELTA_ENABLED
    debug_control_enabled |= DEBUG_SAMPLE_ENABLE_BIT;
#endif

#endif    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Debug_Start
 * Description: Performs actions for internal objects  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Debug_Start()
{
}

/*---------------------------------------------------------------------------------------------------
 * Name: Debug_Update
 * Description: Changes debug state for specified components
 * Parameters: control
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Debug_Enable(uint16 flag)
{
#ifdef COMMS_DEBUG_ENABLED
    debug_control_enabled |= flag;
#endif
}

void Debug_Disable(uint16 flag)
{
#ifdef COMMS_DEBUG_ENABLED
    debug_control_enabled &= ~flag;
#endif
}

uint8 Debug_IsEnabled(uint16 flag)
{
    return debug_control_enabled & flag;
}

void Debug_EnableAll()
{
#ifdef COMMS_DEBUG_ENABLED
    debug_control_enabled = 0xFFFF;
#endif
}

void Debug_DisableAll()
{
#ifdef COMMS_DEBUG_ENABLED
    debug_control_enabled = 0;
#endif
}

void Debug_Store()
{
    saved_debug_control_enabled = debug_control_enabled;
}

void Debug_Restore()
{
    debug_control_enabled = saved_debug_control_enabled;
}

/* [] END OF FILE */
