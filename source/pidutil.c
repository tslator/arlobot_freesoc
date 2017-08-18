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
   Description: This module provides utility for dumping the PID controller output.
 *-------------------------------------------------------------------------------------------------*/

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

        DEBUG_PRINT_ARG("%s pid: %.3f %.3f %.3f %.3f %.3f %.3f %d\r\n", name, set_point, input, error, last_input, iterm, output, pwm);
    }
}
#endif

/* [] END OF FILE */
