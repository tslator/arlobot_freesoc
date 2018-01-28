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

#include <stdarg.h>
#include "dispconfig.h"
#include "consts.h"
#include "debug.h"
#include "cal.h"
#include "utils.h"
#include "concmdif.h"
#include "conserial.h"

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

void DispConfig_Init(void)
{    
    is_running = FALSE;
}

void DispConfig_Start(void)
{
}

void DispConfig_InitConfigDebug(BOOL enable, UINT16 mask)
{
    if (enable)
    {
        Debug_Enable(mask);
    }
    else
    {
        Debug_Disable(mask);
    }
    
    ConSer_WriteLine(TRUE, "0x%02x", Debug_GetMask());

    ConCmdIf_SetIfaceFuncs(NULL_UPDATE, NULL_STATUS, NULL_RESULTS);
    is_running = TRUE;
}

void DispConfig_InitConfigShow(UINT16 mask, BOOL plain_text)
{
    BOOL as_json;

    as_json = !plain_text;

    switch (mask)
    {
        case CONCONFIG_MOTOR_BIT:
            Cal_PrintAllMotorParams(as_json);
            break;

        case CONCONFIG_PID_BIT:
            Cal_PrintAllPidGains(as_json);
            break;

        case CONCONFIG_BIAS_BIT:
            Cal_PrintBias(as_json);
            break;
            
        case CONCONFIG_DEBUG_BIT:
            ConSer_WriteLine(TRUE, "0x%02x", Debug_GetMask());
            break;

        case CONCONFIG_STATUS_BIT:
            Cal_PrintStatus(as_json);
            break;
            
        case CONCONFIG_PARAMS_BIT:
        {
            Cal_PrintParams(as_json);
            break; 
        }
    }

    ConCmdIf_SetIfaceFuncs(NULL_UPDATE, NULL_STATUS, NULL_RESULTS);
    is_running = TRUE;
}

void DispConfig_InitConfigClear(UINT16 mask, BOOL plain_text)
{
    ConSer_WriteLine(TRUE, "No implementation yet!");
    ConCmdIf_SetIfaceFuncs(NULL_UPDATE, NULL_STATUS, NULL_RESULTS);
    is_running = TRUE;    
}

/* [] END OF FILE */