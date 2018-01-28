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
#include <stdio.h>
#include <stdlib.h>
#include "disp.h"
#include "serial.h"
#include "concmd.h"
#include "dispmotor.h"
#include "dispconfig.h"
#include "disppid.h"
#include "dispmotion.h"
#include "debug.h"
#include "utils.h"
#include "concmdif.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#define BOOL_TO_BITMASK(value, bit)  (value ? 1 << bit : 0)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static CONCMD_IF_PTR_TYPE p_active_command;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
static void ValidateConfigCommand(COMMAND_TYPE* const command)
{
    /* Note: The args flags have contextual meaning.  To resolve ambiguity, 'clear' is processed
       before 'debug': 

            config clear debug
            vs
            config debug mask
    */

    if (command->args.clear)
    {
        UINT8 mask = 0;

        if (command->args.all)
        {
            mask = 0x0001F;
        }
        else
        {
            mask |= command->args.motor ? CONCONFIG_MOTOR_BIT : 0;
            mask |= command->args.pid ? CONCONFIG_PID_BIT : 0;
            mask |= command->args.bias ? CONCONFIG_BIAS_BIT : 0;
            mask |= command->args.debug ? CONCONFIG_DEBUG_BIT : 0;
        }

        DispConfig_InitConfigClear(mask, command->args.plain_text);
    }
    else if (command->args.show)
    {
        UINT8 mask = 0;

        mask |= command->args.motor ? CONCONFIG_MOTOR_BIT : 0;
        mask |= command->args.pid ? CONCONFIG_PID_BIT : 0;
        mask |= command->args.bias ? CONCONFIG_BIAS_BIT : 0;
        mask |= command->args.debug ? CONCONFIG_DEBUG_BIT : 0;
        mask |= command->args.status ? CONCONFIG_STATUS_BIT : 0;
        mask |= command->args.params ? CONCONFIG_PARAMS_BIT : 0;

        DispConfig_InitConfigShow(mask, command->args.plain_text);
    }
    else if (command->args.debug)
    {
        UINT16 mask = 0;

        if (command->args.all)
        {
            mask = 0x03FF;
        }
        else if (command->args.mask)
        {
            mask = STR_TO_INT(command->args.mask);
        }
        else
        {
            mask |= command->args.lenc ? DEBUG_LEFT_ENCODER_ENABLE_BIT : 0;
            mask |= command->args.renc ? DEBUG_RIGHT_ENCODER_ENABLE_BIT : 0;
            mask |= command->args.lpid ? DEBUG_LEFT_PID_ENABLE_BIT : 0;
            mask |= command->args.rpid ? DEBUG_RIGHT_PID_ENABLE_BIT : 0;
            mask |= command->args.lmotor ? DEBUG_LEFT_MOTOR_ENABLE_BIT : 0;
            mask |= command->args.rmotor ? DEBUG_RIGHT_MOTOR_ENABLE_BIT : 0;
            mask |= command->args.odom ? DEBUG_ODOM_ENABLE_BIT : 0;
        }

        DispConfig_InitConfigDebug(command->args.enable, mask);
    }
}

static void ValidateMotorCommand(COMMAND_TYPE* const command)
{
    if (command->args.show)
    {
        int wheel;

        wheel = GET_WHEEL(command->args.left, command->args.right);

        if (wheel >= 0)
        {
            DispMotor_InitMotorShow(wheel, command->args.plain_text);
        }
    }
    else if (command->args.rep)
    {
        int wheel;

        wheel = GET_WHEEL(command->args.left, command->args.right);

        if (wheel >= 0)
        {
            DispMotor_InitMotorRepeat(
                (WHEEL_TYPE) wheel,
                STR_TO_FLOAT(command->args.first),
                STR_TO_FLOAT(command->args.second),
                STR_TO_FLOAT(command->args.interval),
                (UINT8) STR_TO_INT(command->args.iters),
                command->args.no_pid,
                command->args.no_accel);
        }
    }
    else if (command->args.cal)
    {   
        int wheel;
        UINT8 iters;

        wheel = GET_WHEEL(command->args.left, command->args.right);
        iters = (UINT8) STR_TO_INT(command->args.iters);
        if (wheel >= 0)
        {
            DispMotor_InitMotorCal(
                (WHEEL_TYPE) wheel,
                iters);
        }
    }
    else if (command->args.val)
    {
        int wheel;
        int direction;

        wheel = GET_WHEEL(command->args.left, command->args.right);
        direction = GET_DIRECTON(command->args.forward, command->args.backward);

        if (wheel >= 0 && direction >= 0)
        {
            DispMotor_InitMotorVal(
                (WHEEL_TYPE) wheel,
                (DIR_TYPE) direction,
                STR_TO_FLOAT(command->args.min_percent),
                STR_TO_FLOAT(command->args.max_percent),
                (UINT8) STR_TO_INT(command->args.num_points));
        }
    }
    else
    {
        DispMotor_InitMotorMove(
            STR_TO_FLOAT(command->args.left_speed),
            STR_TO_FLOAT(command->args.right_speed),
            STR_TO_FLOAT(command->args.duration),
            command->args.no_pid,
            command->args.no_accel);
    }
}

static void ValidatePidCommand(COMMAND_TYPE* const command)
{
    if (command->args.show)
    {
        int wheel;

        wheel = GET_WHEEL(command->args.left, command->args.right);

        if (wheel >= 0)
        {
            DispPid_InitPidShow(wheel, command->args.plain_text);
        }
    }
    else if (command->args.cal)
    {
        int wheel;
        FLOAT step;

        step = 0.0;

        wheel = GET_WHEEL(command->args.left, command->args.right);
        if (command->args.step)
        {
            step = STR_TO_FLOAT(command->args.step);
        }

        if (wheel >= 0)
        {
            DispPid_InitPidCal(FALSE,
                FALSE,
                FALSE,
                FALSE,
                wheel,
                command->args.impulse,
                step,
                command->args.no_debug);
        }
    }
    else if (command->args.val)
    {
        int wheel;
        int direction;
        FLOAT duration;

        wheel = GET_WHEEL(command->args.left, command->args.right);
        direction = GET_DIRECTON(command->args.forward, command->args.backward);
        /* Take duration from command when the console supports duration for PID validation */
        duration = 3.0;

        if (wheel >= 0 && direction >= 0)
        {
            DispPid_InitPidVal(
                wheel,
                direction,
                STR_TO_FLOAT(command->args.min_percent),
                STR_TO_FLOAT(command->args.max_percent),
                STR_TO_INT(command->args.num_points),
                duration);
        }
    }
}

static void ValidateMotionCalCommands(COMMAND_TYPE* const command)
{
    if (command->args.linear)
    {
        DispMotion_InitCalLinear(STR_TO_FLOAT(command->args.distance));
    }
    else if (command->args.angular)
    {
        DispMotion_InitMotionCalAngular(STR_TO_FLOAT(command->args.angle));
    }
    else if (command->args.umbmark)
    {
        // http://www-personal.umich.edu/~johannb/Papers/umbmark.pdf
        DispMotion_InitMotionCalUmbmark();
    }
}

static void ValidateMotionValCommands(COMMAND_TYPE* const command)
{
    if (command->args.linear)
    {
        DispMotion_InitMotionValLinear(STR_TO_FLOAT(command->args.distance));
    }
    else if (command->args.angular)
    {
        DispMotion_InitMotionValAngular(STR_TO_FLOAT(command->args.angle));
    }        
    else if (command->args.square)
    {
        DispMotion_InitMotionValSquare(command->args.left, STR_TO_FLOAT(command->args.side));
    }
    else if (command->args.circle)
    {
        DispMotion_InitMotionValCircle(command->args.cw, STR_TO_FLOAT(command->args.radius));
    }
    else if (command->args.out_and_back)
    {
        DispMotion_InitMotionValOutAndBack(STR_TO_FLOAT(command->args.distance));
    }
}

static void ValidateMotionCommand(COMMAND_TYPE* const command)
{
    if (command->args.cal)
    {
        ValidateMotionCalCommands(command);
    }
    else if (command->args.val)
    {
        ValidateMotionValCommands(command);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Module Interface
 *-------------------------------------------------------------------------------------------------*/    

void Disp_Init(void)
{
    p_active_command = (CONCMD_IF_TYPE *) NULL;
    
    DispConfig_Init();
    DispMotor_Init();
    DispPid_Init();
    DispMotion_Init();
}

void Disp_Start(void)
{
    DispConfig_Start();
    DispMotor_Start();
    DispPid_Start();
    DispMotion_Start();
}

BOOL Disp_IsRunning(void)
{
    if (p_active_command)
    {
        if (p_active_command->status)
        {
            return p_active_command->status();
        }
        else
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL Disp_Update()
{
    if (p_active_command && p_active_command->update)
    {
        return p_active_command->update();
    }
    
    return FALSE;
}

void Disp_Results()
{
    if (p_active_command && p_active_command->results)
    {
        p_active_command->results();
    }

    ConCmdIf_ReleaseIfacePtr();
    p_active_command = (CONCMD_IF_TYPE *) NULL;
}

void Disp_Dispatch(COMMAND_TYPE* const command)
{
    /* Note: config needs to be checked first to eliminate any ambiguity between config the command and config */
    if (command->args.config)
    {
        ValidateConfigCommand(command);
    }
    else if (command->args.pid)
    {
        ValidatePidCommand(command);
    }
    else if (command->args.motor)
    {   
        ValidateMotorCommand(command);
    }
    else if (command->args.motion)
    {
        ValidateMotionCommand(command);
    }

    command->is_valid = ConCmdIf_GetIfacePtr(&p_active_command);
}

/* [] END OF FILE */