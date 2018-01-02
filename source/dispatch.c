#include <stdio.h>
#include <stdlib.h>
#include "dispatch.h"
#include "serial.h"
#include "concmd.h"
#include "conmotor.h"
#include "conconfig.h"
#include "conpid.h"
#include "conmotion.h"
#include "debug.h"


static CONCMD_IF_TYPE active_command;

static void validate_motor_command(COMMAND_TYPE* const command)
{
    if (command->args.show)
    {
        command->is_valid = ConMotor_Assign(&active_command, 
                                            MOTOR_SHOW,
                                            GET_WHEEL(command->args.left, command->args.right),
                                            command->args.plain_text ? TRUE : FALSE);
    }
    else if (command->args.rep)
    {
        command->is_valid = ConMotor_Assign(&active_command, 
                                            MOTOR_REPEAT,
                                            GET_WHEEL(command->args.left, command->args.right),
                                            STR_TO_FLOAT(command->args.first), 
                                            STR_TO_FLOAT(command->args.second), 
                                            STR_TO_FLOAT(command->args.intvl),
                                            STR_TO_INT(command->args.iters),
                                            command->args.no_pid,
                                            command->args.no_accel);
    }
    else if (command->args.cal)
    {        
        command->is_valid = ConMotor_Assign(&active_command, MOTOR_CAL, 
            GET_WHEEL(command->args.left, command->args.right), 
            STR_TO_INT(command->args.iters));
        
    }
    else if (command->args.val)
    {
        command->is_valid = ConMotor_Assign(&active_command, 
                                            MOTOR_VAL, 
                                            GET_WHEEL(command->args.left, command->args.right),
                                            STR_TO_FLOAT(command->args.min_percent), 
                                            STR_TO_FLOAT(command->args.max_percent), 
                                            STR_TO_INT(command->args.num_points));
    }
    else
    {
        command->is_valid = ConMotor_Assign(&active_command, MOTOR_MOVE, 
            STR_TO_FLOAT(command->args.left_speed), 
            STR_TO_FLOAT(command->args.right_speed), 
            STR_TO_FLOAT(command->args.duration),
            command->args.no_pid,
            command->args.no_accel);
    }
    
}

static void validate_pid_command(COMMAND_TYPE* const command)
{
    if (command->args.show)
    {
        command->is_valid = ConPid_Assign(&active_command, PID_SHOW, 
            GET_WHEEL(command->args.left, command->args.right),
            command->args.plain_text ? TRUE : FALSE);
    }
    else if (command->args.cal)
    {
        command->is_valid = ConPid_Assign(&active_command, PID_CAL,
            GET_WHEEL(command->args.left, command->args.right),
            command->args.impulse ? TRUE : FALSE,
            STR_TO_FLOAT(command->args.step),
            command->args.with_debug ? TRUE : FALSE);
    }
    else if (command->args.val)
    {
        DIR_TYPE direction;

        if (command->args.forward)
        {
            direction = DIR_FORWARD;
        }        
        else if (command->args.backward)
        {
            direction = DIR_BACKWARD;
        }
        else
        {
            command->is_valid = FALSE;
            return;
        }
        
        command->is_valid = ConPid_Assign(&active_command, PID_VAL, 
            GET_WHEEL(command->args.left, command->args.right),
            direction,
            STR_TO_FLOAT(command->args.min_percent), 
            STR_TO_FLOAT(command->args.max_percent),
            STR_TO_INT(command->args.num_points));
    }
    else
    {
        command->is_valid = FALSE;
    }
}

static void validate_config_command(COMMAND_TYPE* const command)
{
    if (command->args.debug)
    {
        // lmotor|rmotor|lenc|renc|lpid|rpid|odom|all
        UINT16 mask = 0;

        if (command->args.mask)
        {
            mask = STR_TO_INT(command->args.mask);
        }
        else
        {
            if (command->args.all)
            {
                mask = 0x03FF;
            }
            else
            {
                mask |= command->args.lmotor ? DEBUG_LEFT_MOTOR_ENABLE_BIT : 0;
                mask |= command->args.rmotor ? DEBUG_RIGHT_MOTOR_ENABLE_BIT : 0;
                mask |= command->args.lenc ? DEBUG_LEFT_ENCODER_ENABLE_BIT : 0;
                mask |= command->args.renc ? DEBUG_RIGHT_ENCODER_ENABLE_BIT : 0;
                mask |= command->args.lpid ? DEBUG_LEFT_PID_ENABLE_BIT : 0;
                mask |= command->args.rpid ? DEBUG_RIGHT_PID_ENABLE_BIT : 0;
                mask |= command->args.odom ? DEBUG_ODOM_ENABLE_BIT : 0;
            }
        }

        BOOL enable_disable = command->args.enable ? TRUE : FALSE;

        command->is_valid = ConConfig_Assign(&active_command, CONFIG_DEBUG, enable_disable, mask);
    }
    else if (command->args.show)
    {
        // motor|encoder|pid|bias|debug

        UINT8 mask = 0;

        mask |= command->args.motor ? 1 << 0 : 0;
        mask |= command->args.pid ? 1 << 1 : 0;
        mask |= command->args.bias ? 1 << 2 : 0;
        mask |= command->args.debug ? 1 << 3 : 0;
        mask |= command->args.params ? 1 << 4 : 0;

        command->is_valid = ConConfig_Assign(&active_command, CONFIG_SHOW, mask, command->args.plain_text);
    }
    else if (command->args.clear)
    {
        // motor|encoder|pid|bias|debug|all
        UINT8 mask = 0;

        if (command->args.all)
        {
            mask = 0x0001F;
        }
        else
        {
            mask |= command->args.motor ? 1 << 0 : 0;
            mask |= command->args.pid ? 1 << 1 : 0;
            mask |= command->args.bias ? 1 << 2 : 0;
            mask |= command->args.debug ? 1 << 3 : 0;
        }

        command->is_valid = ConConfig_Assign(&active_command, CONFIG_CLEAR, mask, command->args.plain_text);
    }
    else
    {
        command->is_valid = FALSE;
    }
}

static void validate_motion_command(COMMAND_TYPE* const command)
{
    if (command->args.cal)
    {
        if (command->args.linear)
        {
            command->is_valid = ConMotion_Assign(&active_command, MOTION_CAL_LINEAR, STR_TO_FLOAT(command->args.distance));
        }
        else if (command->args.angular)
        {
            command->is_valid = ConMotion_Assign(&active_command, MOTION_CAL_ANGULAR, STR_TO_FLOAT(command->args.angle));
        }
        else if (command->args.umbmark)
        {
            // http://www-personal.umich.edu/~johannb/Papers/umbmark.pdf
            command->is_valid = ConMotion_Assign(&active_command, MOTION_CAL_UMB);
        }
        else
        {
            command->is_valid = FALSE;
        }
    }
    else if (command->args.val)
    {
        if (command->args.linear)
        {
            command->is_valid = ConMotion_Assign(&active_command, MOTION_VAL_LINEAR, STR_TO_FLOAT(command->args.distance));
        }
        else if (command->args.angular)
        {
            command->is_valid = ConMotion_Assign(&active_command, MOTION_VAL_ANGULAR, STR_TO_FLOAT(command->args.angle));
        }        
        else if (command->args.square)
        {
            BOOL left_or_right = command->args.left ? TRUE : FALSE;
            command->is_valid = ConMotion_Assign(&active_command, MOTION_VAL_SQUARE, 
                left_or_right,
                STR_TO_FLOAT(command->args.side));
        }
        else if (command->args.circle)
        {
            int cw_or_ccw = command->args.cw ? TRUE : FALSE;
            command->is_valid = ConMotion_Assign(&active_command, MOTION_VAL_CIRCLE, cw_or_ccw, STR_TO_FLOAT(command->args.radius));
        }
        else if (command->args.out_and_back)
        {
            command->is_valid = ConMotion_Assign(&active_command, MOTION_VAL_OUTANDBACK, STR_TO_FLOAT(command->args.distance));
        }
    }
    else
    {
        command->is_valid = FALSE;
    }
}


void Dispatch_Init(void)
{
}

void Dispatch_Start(void)
{
}

BOOL Dispatch_IsRunning(void)
{
    if (active_command.is_assigned)
    {
        return active_command.status();
    }
    
    return FALSE;
}

BOOL Dispatch_Update()
{
    if (active_command.is_assigned)
    {
        return active_command.update();
    }
    
    return FALSE;
}

void Dispatch_Results()
{
    if (active_command.is_assigned)
    {
        active_command.results();
        active_command.is_assigned = FALSE;
    }
}

void Disp_Dispatch(COMMAND_TYPE* const command)
{
    command->is_valid = FALSE;
    active_command.is_assigned = FALSE;

    /* Note: config needs to be checked first to eliminate any ambiguity */
    if (command->args.config)
    {
        validate_config_command(command);
    }
    else if (command->args.pid)
    {
        validate_pid_command(command);
    }
    else if (command->args.motor)
    {   
        validate_motor_command(command);
    }
    else if (command->args.motion)
    {
        validate_motion_command(command);
    }
    
} 