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
#include "conmotor.h"
#include "conconfig.h"
#include "conpid.h"
#include "conmotion.h"
#include "debug.h"
#include "utils.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#define BOOL_TO_BITMASK(value, bit)  (value ? 1 << bit : 0)


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static CONCMD_IF_TYPE * p_active_command;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
static CONCMD_IF_TYPE * const validate_config_command(COMMAND_TYPE* const command)
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
            mask |= BOOL_TO_BITMASK(command->args.motor, 0);
            mask |= BOOL_TO_BITMASK(command->args.pid, 1);
            mask |= BOOL_TO_BITMASK(command->args.bias, 2);
            mask |= BOOL_TO_BITMASK(command->args.debug, 3);
        }

        return ConConfig_InitConfigClear(mask, command->args.plain_text);
    }
    else if (command->args.show)
    {
        UINT8 mask = 0;

        mask |= BOOL_TO_BITMASK(command->args.motor, 0);
        mask |= BOOL_TO_BITMASK(command->args.pid, 1);
        mask |= BOOL_TO_BITMASK(command->args.bias, 2);
        mask |= BOOL_TO_BITMASK(command->args.debug, 3);
        mask |= BOOL_TO_BITMASK(command->args.params, 4);

        return ConConfig_InitConfigShow(mask, command->args.plain_text);
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

        return ConConfig_InitConfigDebug(command->args.enable, mask);
    }

    return (CONCMD_IF_TYPE *) NULL;
}

static CONCMD_IF_TYPE * const validate_motor_command(COMMAND_TYPE* const command)
{
    if (command->args.show)
    {
        int wheel;

        wheel = GET_WHEEL(command->args.left, command->args.right);

        if (wheel >= 0)
        {
            return ConMotor_InitMotorShow(wheel, command->args.plain_text);
        }
    }
    else if (command->args.rep)
    {
        int wheel;

        wheel = GET_WHEEL(command->args.left, command->args.right);

        if (wheel >= 0)
        {
            return ConMotor_InitMotorRepeat(
                (WHEEL_TYPE) wheel,
                STR_TO_FLOAT(command->args.first),
                STR_TO_FLOAT(command->args.second),
                STR_TO_FLOAT(command->args.intvl),
                STR_TO_INT(command->args.iters),
                command->args.no_pid,
                command->args.no_accel);
        }
    }
    else if (command->args.cal)
    {   
        int wheel;

        wheel = GET_WHEEL(command->args.left, command->args.right);

        if (wheel >= 0)
        {
            return ConMotor_InitMotorCal(
                (WHEEL_TYPE) wheel,
                STR_TO_INT(command->args.iters));
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
            return ConMotor_InitMotorVal(
                (WHEEL_TYPE) wheel,
                (DIR_TYPE) direction,
                STR_TO_FLOAT(command->args.min_percent),
                STR_TO_FLOAT(command->args.max_percent),
                STR_TO_INT(command->args.num_points));
        }
    }
    else
    {
        return ConMotor_InitMotorMove(
            STR_TO_FLOAT(command->args.left_speed),
            STR_TO_FLOAT(command->args.right_speed),
            STR_TO_FLOAT(command->args.duration),
            command->args.no_pid,
            command->args.no_accel);
    }

    return (CONCMD_IF_TYPE *) NULL;
}

static CONCMD_IF_TYPE * const validate_pid_command(COMMAND_TYPE* const command)
{
    if (command->args.show)
    {
        int wheel;

        wheel = GET_WHEEL(command->args.left, command->args.right);

        if (wheel >= 0)
        {
            return ConPid_InitPidShow(wheel, command->args.plain_text);
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
            return ConPid_InitPidCal(wheel,
                                                command->args.impulse,
                                                step,
                                                command->args.with_debug);
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
            return ConPid_InitPidVal(
                wheel,
                direction,
                STR_TO_FLOAT(command->args.min_percent),
                STR_TO_FLOAT(command->args.max_percent),
                STR_TO_INT(command->args.num_points));
        }
    }

    return (CONCMD_IF_TYPE *) NULL;
}

static CONCMD_IF_TYPE * const validate_motion_cal_commands(COMMAND_TYPE* const command)
{
    if (command->args.linear)
    {
        return ConMotion_InitCalLinear(STR_TO_FLOAT(command->args.distance));
    }
    else if (command->args.angular)
    {
        return ConMotion_InitMotionCalAngular(STR_TO_FLOAT(command->args.angle));
    }
    else if (command->args.umbmark)
    {
        // http://www-personal.umich.edu/~johannb/Papers/umbmark.pdf
        return ConMotion_InitMotionCalUmbmark();
    }

    return (CONCMD_IF_TYPE *) NULL;
}

static CONCMD_IF_TYPE * const validate_motion_val_commands(COMMAND_TYPE* const command)
{
    if (command->args.linear)
    {
        return ConMotion_InitMotionValLinear(STR_TO_FLOAT(command->args.distance));
    }
    else if (command->args.angular)
    {
        return ConMotion_InitMotionValAngular(STR_TO_FLOAT(command->args.angle));
    }        
    else if (command->args.square)
    {
        return ConMotion_InitMotionValSquare(command->args.left, STR_TO_FLOAT(command->args.side));
    }
    else if (command->args.circle)
    {
        return ConMotion_InitMotionValCircle(command->args.cw, STR_TO_FLOAT(command->args.radius));
    }
    else if (command->args.out_and_back)
    {
        return ConMotion_InitMotionValOutAndBack(STR_TO_FLOAT(command->args.distance));
    }

    return (CONCMD_IF_TYPE *) NULL;
}

static CONCMD_IF_TYPE * const validate_motion_command(COMMAND_TYPE* const command)
{
    if (command->args.cal)
    {
        return validate_motion_cal_commands(command);
    }
    else if (command->args.val)
    {
        return validate_motion_val_commands(command);
    }

    return (CONCMD_IF_TYPE *) NULL;
}

/*---------------------------------------------------------------------------------------------------
 * Module Interface
 *-------------------------------------------------------------------------------------------------*/    

void Disp_Init(void)
{
    p_active_command = (CONCMD_IF_TYPE *) NULL;
    
    ConConfig_Init();
    ConMotor_Init();
    ConPid_Init();
    ConMotion_Init();
}

void Disp_Start(void)
{
    ConConfig_Start();
    ConMotor_Start();
    ConPid_Start();
    ConMotion_Start();
}

BOOL Disp_IsRunning(void)
{
    if (p_active_command && p_active_command->status)
    {
        return p_active_command->status();
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
}

void Disp_Dispatch(COMMAND_TYPE* const command)
{
    p_active_command = (CONCMD_IF_TYPE *) NULL;

    /* Note: config needs to be checked first to eliminate any ambiguity between config the command and config */
    if (command->args.config)
    {
        p_active_command = validate_config_command(command);
    }
    else if (command->args.pid)
    {
        p_active_command = validate_pid_command(command);
    }
    else if (command->args.motor)
    {   
        p_active_command = validate_motor_command(command);
    }
    else if (command->args.motion)
    {
        p_active_command = validate_motion_command(command);
    }
    
    command->is_valid = p_active_command != (CONCMD_IF_TYPE *) NULL ? TRUE : FALSE;    

} 

/* [] END OF FILE */