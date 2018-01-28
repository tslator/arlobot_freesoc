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
#include "dispmotor.h"
#include "conserial.h"
#include "consts.h"
#include "types.h"
#include "utils.h"
#include "cal.h"
#include "calmotor.h"
#include "valmotor.h"
#include "concmdif.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;
#define MIN_DURATION (0.1)
#define MAX_DURATION (60)
#define MIN_CAL_ITER (1)
#define MAX_CAL_ITER (10)
#define MIN_INTERVAL (0.1)
#define MAX_INTERVAL (10)
#define MIN_PERCENT  (0.0)
#define MAX_PERCENT  (1.0)
#define MIN_NUM_POINTS (3)
#define MAX_NUM_POINTS (13)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static BOOL is_running;

/*---------------------------------------------------------------------------------------------------
 * Module Interface
 *-------------------------------------------------------------------------------------------------*/
void DispMotor_Init(void)
{
    is_running = FALSE;
}

void DispMotor_Start(void)
{
}

void DispMotor_InitMotorShow(WHEEL_TYPE wheel, BOOL plain_text)
{
    ConCmdIf_SetIfaceFuncs(NULL_UPDATE, NULL_STATUS, NULL_RESULTS);
    Cal_PrintMotorParams(wheel, !plain_text);
        
    is_running = TRUE;
}

void DispMotor_InitMotorRepeat(
            WHEEL_TYPE wheel,
            FLOAT first,
            FLOAT second,
            FLOAT interval,
            INT8 iterations,
            BOOL no_pid,
            BOOL no_accel)
{
    /* Note: Parser requires all three parameters to be present so they will always have a value */
    if ( !in_range(first, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) ||
         !in_range(second, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) ||
         !in_range(interval, MIN_INTERVAL, MAX_INTERVAL) ||
         !in_range(iterations, 1, 10))
    {
        return;
    }

    ConCmdIf_SetIfaceFuncs(ValMotor_RepeatUpdate, NULL_STATUS, NULL_RESULTS);    
    ValMotor_RepeatInit(wheel, first, second, interval, iterations, no_pid, no_accel);
    is_running = TRUE;
}

void DispMotor_InitMotorCal(
            WHEEL_TYPE wheel,
            INT8 iterations)
{
    if (!in_range(iterations, MIN_CAL_ITER, MAX_CAL_ITER))
    {
        return;
    }

    ConCmdIf_SetIfaceFuncs(CalMotor_Update, NULL_STATUS, NULL_RESULTS);
    CalMotor_Init(wheel, iterations);
    is_running = TRUE;
}
            
void DispMotor_InitMotorVal(
            WHEEL_TYPE wheel,
            DIR_TYPE direction,
            FLOAT min_percent,
            FLOAT max_percent,
            INT8 num_points)
{
    ConSer_WriteLine(TRUE, "Starting Motor Validation");
    
    if (!in_range_float(min_percent, MIN_PERCENT, MAX_PERCENT) || 
        !in_range_float(max_percent, MIN_PERCENT, MAX_PERCENT) || 
        min_percent >= max_percent || 
        is_even(num_points) || 
        !in_range(num_points, MIN_NUM_POINTS, MAX_NUM_POINTS))
    {
        return;
    }

    ConCmdIf_SetIfaceFuncs(ValMotor_Update, NULL_STATUS, NULL_RESULTS);
    ValMotor_Init(wheel, direction, min_percent, max_percent, num_points);
    is_running = TRUE;    
}
            
void DispMotor_InitMotorMove(
            FLOAT left_speed,
            FLOAT right_speed,
            FLOAT duration,
            BOOL no_pid,
            BOOL no_accel)
{
    left_speed = IS_VALID_FLOAT(left_speed) ? left_speed : 0.0;
    right_speed = IS_VALID_FLOAT(right_speed) ? right_speed : 0.0;

    if (!in_range_float(left_speed, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) ||
        !in_range_float(right_speed, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) ||
        !in_range_float(duration, MIN_DURATION, MAX_DURATION))
    {
        return;
    }

    ConCmdIf_SetIfaceFuncs(ValMotor_MoveUpdate, NULL_STATUS, NULL_RESULTS);
    ValMotor_MoveInit(left_speed, right_speed, duration, no_pid, no_accel);
    is_running = TRUE;
}