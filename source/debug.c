/* 
MIT License

Copyright (c) 2017 Tim Slator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


/*---------------------------------------------------------------------------------------------------
   Description: This module provides macros and functions for debugging.  Debugging is done via the 
   USB serial port.
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "debug.h"
#include "serial.h"
#include "uartif.h"
#include "usbif.h"


/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;
#define ENCODER_DEBUG_BIT   (0x0001)
#define PID_DEBUG_BIT       (0x0002)
#define MOTOR_DEBUG_BIT     (0x0004)
#define ODOM_DEBUG_BIT      (0x0008)
#define SAMPLE_DEBUG_BIT    (0x0010)



/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static UINT8 debug_control_enabled;
static UINT8 saved_debug_control_enabled;
/*static SERIAL_DEVICE_TYPE device = {
    UARTIF_PutString,
    UARTIF_GetAll,
    UARTIF_GetChar,
    UARTIF_PutChar
};*/

/* Temporarily using the USB interface until a UART interface is wired up */
static SERIAL_DEVICE_TYPE device = {
    USBIF_PutString,
    USBIF_GetAll,
    USBIF_GetChar,
    USBIF_PutChar
};

static CHAR fmt_str[1000];
static UINT8 debug_level = DBG_DBG;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

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
#if  defined UNIPID_DUMP_ENABLED
    debug_control_enabled |= DEBUG_UNIPID_ENABLE_BIT;
#endif
#if  defined ANGPID_DUMP_ENABLED
    debug_control_enabled |= DEBUG_ANGPID_ENABLE_BIT;
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

    UARTIF_Init();
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
    UARTIF_Start();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Debug_Update
 * Description: Changes debug state for specified components
 * Parameters: control
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Debug_Enable(UINT8 flag)
{
#ifdef COMMS_DEBUG_ENABLED
    debug_control_enabled |= flag;
#endif
}

void Debug_Disable(UINT8 flag)
{
#ifdef COMMS_DEBUG_ENABLED
    debug_control_enabled &= ~flag;
#endif
}

UINT8 Debug_IsEnabled(UINT8 flag)
{
#ifdef COMMS_DEBUG_ENABLED
    return debug_control_enabled & flag;
#endif
}

void Debug_EnableAll()
{
#ifdef COMMS_DEBUG_ENABLED
    debug_control_enabled = 0xFF;
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

UINT16 Debug_GetMask()
{
    return debug_control_enabled;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update_Debug
 * Description: Enables/Disables debug based on the specified bits.
 * Parameters: bits - contains bits corresponding to the supported debug, e.g., encoder, pid, motor, etc
 *                    1 indicates debug is enabled, 0 indicates debug is disabled
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/ 
void Debug_Update(UINT8 bits)
{
    if (bits & ENCODER_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_LEFT_ENCODER_ENABLE_BIT);
        Debug_Enable(DEBUG_RIGHT_ENCODER_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_LEFT_ENCODER_ENABLE_BIT);
        Debug_Disable(DEBUG_RIGHT_ENCODER_ENABLE_BIT);
    }
    
    if (bits & PID_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_LEFT_PID_ENABLE_BIT);
        Debug_Enable(DEBUG_RIGHT_PID_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_LEFT_PID_ENABLE_BIT);
        Debug_Disable(DEBUG_RIGHT_PID_ENABLE_BIT);
    }
    
    if (bits & MOTOR_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_LEFT_MOTOR_ENABLE_BIT);
        Debug_Enable(DEBUG_RIGHT_MOTOR_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_LEFT_MOTOR_ENABLE_BIT);
        Debug_Disable(DEBUG_RIGHT_MOTOR_ENABLE_BIT);
    }

    if (bits & ODOM_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_ODOM_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_ODOM_ENABLE_BIT);
    }
    
    if (bits & SAMPLE_DEBUG_BIT)
    {
        Debug_Enable(DEBUG_SAMPLE_ENABLE_BIT);
    }
    else
    {
        Debug_Disable(DEBUG_SAMPLE_ENABLE_BIT);
    }
}

void Debug_Print(CHAR const * const file, CHAR const * const func, UINT16 line, CHAR const * const msg, ...)
{
    va_list args;
    va_start(args, msg);
    sprintf(fmt_str, "%s - [FILE : %s, FUNC : %s, LINE : %d]: %s", "<LEVEL>", file, func, line, msg);
    Ser_WriteLine(device, TRUE, fmt_str, args);
    va_end(args);
}

void Debug_SetLevel(UINT8 level)
{
    debug_level = level;
}

UINT8 Debug_GetLevel()
{
    return debug_level;
}

/* [] END OF FILE */
