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
#include "i2c.h"


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
void Debug_Update(uint16 control)
{
#ifdef COMMS_DEBUG_ENABLED
    // When debug is enabled, the bitmap can be used to turn on/off specific debug, e.g., encoder, pid, odom, and motor.

    debug_control_enabled = 0;
    
    if (control & ENCODER_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_LEFT_ENCODER_ENABLE_BIT | DEBUG_RIGHT_ENCODER_ENABLE_BIT;
    }
    
    if (control & PID_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_LEFT_PID_ENABLE_BIT | DEBUG_RIGHT_PID_ENABLE_BIT;
    }
    
    if (control & MOTOR_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_LEFT_MOTOR_ENABLE_BIT | DEBUG_RIGHT_MOTOR_ENABLE_BIT;
    }

    if (control & ODOM_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_ODOM_ENABLE_BIT;
    }
    
    if (control & SAMPLE_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_SAMPLE_ENABLE_BIT;
    }
    
#endif
}

/* [] END OF FILE */
