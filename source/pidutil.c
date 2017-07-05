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
#include <math.h>
#include "pidutil.h"
#include "config.h"
#include "debug.h"
#include "utils.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/
#define IS_NAN_DEFAULT(x,d)   isnan(x) ? d : x

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

#if defined (LEFT_PID_DUMP_ENABLED) || defined(RIGHT_PID_DUMP_ENABLED) || defined(CTRL_LINEAR_PID_DUMP_ENABLED) || defined(CTRL_ANGULAR_PID_DUMP_ENABLED)

static char set_point_str[10];
static char input_str[10];
static char error_str[10];
static char last_input_str[10];
static char iterm_str[10];
static char output_str[10];
    
static float set_point;
static float input;
static float error;
static float last_input;
static float iterm;
static float output;
    
void DumpPid(char *name, PIDControl *pid, uint16 pwm)
{
    if (Debug_IsEnabled(DEBUG_LEFT_PID_ENABLE_BIT|DEBUG_RIGHT_PID_ENABLE_BIT))
    {
        set_point = IS_NAN_DEFAULT(pid->setpoint, 0);
        input = IS_NAN_DEFAULT(pid->input, 0);
        error = IS_NAN_DEFAULT(pid->setpoint - pid->input, 0);
        last_input = IS_NAN_DEFAULT(pid->lastInput, 0);
        iterm = IS_NAN_DEFAULT(pid->iTerm, 0);
        output = IS_NAN_DEFAULT(pid->output, 0);

        ftoa(set_point, set_point_str, 3);
        ftoa(input, input_str, 3);
        ftoa(error, error_str, 3);
        ftoa(last_input, last_input_str, 3);
        ftoa(iterm, iterm_str, 3);
        ftoa(output, output_str, 3);

        DEBUG_PRINT_ARG("%s pid: %s %s %s %s %s %s %d\r\n", name, set_point_str, input_str, error_str, last_input_str, iterm_str, output_str, pwm);
    }
}
#endif

/* [] END OF FILE */
