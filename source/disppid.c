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
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdarg.h>
#include "disppid.h"
#include "conserial.h"
#include "calpid.h"
#include "valpid.h"
#include "utils.h"
#include "cal.h"
#include "concmdif.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static BOOL is_running;

/*---------------------------------------------------------------------------------------------------
 * Module Interface
 *-------------------------------------------------------------------------------------------------*/
void DispPid_Init(void)
{
    is_running = FALSE;
}

void DispPid_Start(void)
{

}

void DispPid_InitPidShow(WHEEL_TYPE wheel, BOOL plain_text)
{
    ConSer_WriteLine(TRUE, "Wheel: %d, Plain Text: %d", wheel, plain_text);

    BOOL as_json = !plain_text;
    
    switch (wheel)
    {
        case WHEEL_LEFT:
            Cal_PrintLeftPidGains(as_json);
            break;

        case WHEEL_RIGHT:
            Cal_PrintRightPidGains(as_json);
            break;

        case WHEEL_BOTH:
        default:
            Cal_PrintAllPidGains(as_json);
    }

    ConCmdIf_SetIfaceFuncs(NULL_UPDATE, NULL_STATUS, NULL_RESULTS);    
    is_running = TRUE;
}

void DispPid_InitPidCal(BOOL set,
                        BOOL clear,
                        BOOL interactive,
                        BOOL load_gains,
                        WHEEL_TYPE wheel, 
                        BOOL impulse, 
                        FLOAT step, 
                        BOOL no_debug)
{
    if (set)
    {
        if (step > 0.0)
        {
            CalPid_SetStep(step);
        }
        else if (no_debug)
        {
            CalPid_SetDebug(no_debug);
        }
        else if (interactive)
        {
            CalPid_SetInteractive();
        }
        else if (load_gains)
        {
            CalPid_SetLoadGains();
        }
        
        ConCmdIf_SetIfaceFuncs(NULL_UPDATE, NULL_STATUS, NULL_RESULTS);        
    }
    else if (clear)
    {
        CalPid_Clear();
        ConCmdIf_SetIfaceFuncs(NULL_UPDATE, NULL_STATUS, NULL_RESULTS);        
    }        
    else
    {
        ConSer_WriteLine(TRUE, "PID Calibration Init");

        /* Note: By default, calibration should always have debug.  Change the console arguments to use --no-debug instead of --with-debug */
        CalPid_Init(wheel, impulse, step, 1);
        ConCmdIf_SetIfaceFuncs(CalPid_Update, NULL_STATUS, CalPid_Results);
    }
    is_running = TRUE;
}

void DispPid_InitPidVal(WHEEL_TYPE wheel, 
                        DIR_TYPE direction,
                        FLOAT min_percent,
                        FLOAT max_percent,
                        INT8 num_points,
                        FLOAT duration)
{
    if (!in_range_float(min_percent, 0.2, 0.8) ||
        !in_range_float(max_percent, 0.2, 0.8) || 
        is_even(num_points) ||
        !in_range(num_points, 3, 13) ||
        in_range_float(duration, 0.5, 3.0))
    {
        return;
    }

    ConCmdIf_SetIfaceFuncs(ValPid_Update, NULL_STATUS, ValPid_Results);
    ValPid_Init(wheel, direction, min_percent, max_percent, num_points, duration);    
    
    is_running = TRUE;
}


