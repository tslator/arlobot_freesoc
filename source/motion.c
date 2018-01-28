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
#include "motion.h"
#include "cal.h"
#include "odom.h"
#include "conserial.h"
#include "utils.h"
#include "time.h"
#include "control.h"
#include "contract.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;
#define MOTION_OK (VAL_OK)
#define MOTION_COMPLETE (VAL_COMPLETE)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef struct _tag_move
{
    MOVE_ENUM_TYPE type;
    FLOAT direction;
    FLOAT speed;
    FLOAT timeout;
    FLOAT goal;
    BOOL is_moving;
    FLOAT distance;
    void (*start)(void *);
    BOOL (*is_at_goal)(void *);
} MOVE_TYPE;


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static FLOAT cmd_linear_velocity;
static FLOAT cmd_angular_velocity;
static UINT32 cmd_timeout;

static UINT8 move_index;
static UINT8 move_end;

static UINT32 start_time;
static BOOL is_running;
static MOVE_TYPE moves[16];
static MOVE_TYPE *p_move;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------
    Default Start/Is At Goal
*/
static void default_start(void *this)
{
    this = this;
    ConSer_WriteLine(TRUE, "Yo! Wassup Start!!");
}

static BOOL default_is_at_goal(void *this)
{
    this = this;
    ConSer_WriteLine(TRUE, "Yo! Wassup Is At Goal!!");
    
    return TRUE;
}

/*---------------------------------------------------
    Linear Start/Is At Goal
*/
static void linear_start(void *this)
{   
    FLOAT x_pos;
    FLOAT y_pos;
    MOVE_TYPE *linear = (MOVE_TYPE *)this;

    ConSer_WriteLine(TRUE, "Starting Linear ...");

    Odom_GetXYPosition(&x_pos, &y_pos);
    linear->goal = sqrt(x_pos*x_pos + y_pos*y_pos) + linear->distance;
    
    cmd_linear_velocity = linear->direction == DIR_FORWARD ? linear->speed : -linear->speed;
    cmd_angular_velocity = 0.0;
    cmd_timeout = linear->timeout;
    linear->is_moving = TRUE;
}

static BOOL linear_is_at_goal(void *this)
{
    FLOAT x_pos;
    FLOAT y_pos;
    FLOAT curr_pos;
    MOVE_TYPE *linear = (MOVE_TYPE *)this;

    Odom_GetXYPosition(&x_pos, &y_pos);
    curr_pos = sqrt(x_pos*x_pos + y_pos*y_pos);
    if (lessthan_float(curr_pos, linear->goal, 0.0001))
    {
        return FALSE;
    }

    ConSer_WriteLine(TRUE, "Linear Complete");
    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    cmd_timeout = 5000;
    linear->is_moving = FALSE;

    return TRUE;
}

/*---------------------------------------------------
    Angular Start/Is At Goal
*/
static void angular_start(void *this)
{
    FLOAT heading;
    MOVE_TYPE *linear = (MOVE_TYPE *)this;

    ConSer_WriteLine(TRUE, "Starting Rotate ...");

    heading = Odom_GetHeading();
    linear->goal = NormalizeHeading(heading + DEGREES_TO_RADIANS(linear->distance));
    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = linear->direction == DIR_CW ? -linear->speed : linear->speed;
    cmd_timeout = 0;
    linear->is_moving = TRUE;
    start_time = millis();
}

static BOOL angular_is_at_goal(void *this)
{
    UINT32 delta;
    FLOAT heading;
    MOVE_TYPE *linear = (MOVE_TYPE *)this;

    /* Note: Wait a small amount of time before evaulating the heading */
    delta = millis() - start_time;
    if (delta < 500)
    {
        return VAL_OK;
    }

    heading = Odom_GetHeading();
    if (!equal_float(heading, linear->goal, 0.01))
    {
        return FALSE;
    }

    ConSer_WriteLine(TRUE, "Rotote Complete");
    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    linear->is_moving = FALSE;
    return TRUE;
}

/*---------------------------------------------------
    Pause Start/Is At Goal
*/
static void pause_start(void *this)
{
    MOVE_TYPE *pause = (MOVE_TYPE *)this;

    pause = pause;

    ConSer_WriteLine(TRUE, "Starting Pause ...");

    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    cmd_timeout = 0;
    pause->is_moving = TRUE;
    start_time = millis();
}

static BOOL pause_is_at_goal(void *this)
{
    UINT32 delta;
    MOVE_TYPE *pause = (MOVE_TYPE *)this;

    /* Note: Wait a small amount of time before evaulating the heading */
    delta = millis() - start_time;
    if (delta < pause->timeout)
    {
        return FALSE;
    }

    ConSer_WriteLine(TRUE, "Pause Complete");
    pause->is_moving = TRUE;
    return TRUE;
}

/*---------------------------------------------------
    Move Helpers
*/
static UINT8 ExecuteMove()
{
    if (!p_move->is_moving)
    {
        p_move->start(p_move);
        return MOTION_OK;
    }
    else
    {
        if (p_move->is_at_goal(p_move))
        {
            return MOTION_COMPLETE;
        }
    }

    return MOTION_OK;
}

static UINT8 SetNextMove()
{
    if (move_index < move_end)
    {
        move_index++;
        p_move = &moves[move_index];
    }
    
    return move_index == move_end ? MOTION_COMPLETE : MOTION_OK;
}

static void SetCommandVelocity(FLOAT *linear, FLOAT *angular, UINT32 *timeout)
{
    *linear = cmd_linear_velocity;
    *angular = cmd_angular_velocity;
    *timeout = cmd_timeout;
}

static void SetLinearMove(MOVE_ENUM_TYPE move, LINEAR_MOVE_DATA_TYPE const * const p_data)
{
    if (move == MOVE_LINEAR)
    {
        p_move->distance = p_data->distance;
        p_move->speed = p_data->speed;
        p_move->direction = p_data->distance > 0 ? DIR_FORWARD : DIR_BACKWARD;
        p_move->is_moving = FALSE;
        p_move->is_at_goal = linear_is_at_goal;
        p_move->start = linear_start;        
        p_move->goal = 0.0;        
    }
}

static void SetAngularMove(MOVE_ENUM_TYPE move, ANGULAR_MOVE_DATA_TYPE const * const p_data)
{
    if (move == MOVE_ROTATE)
    {
        p_move->distance = p_data->angle;
        p_move->speed = p_data->speed;
        p_move->direction = p_data->angle > 0 ? DIR_CCW : DIR_CW;
        p_move->is_moving = FALSE;
        p_move->is_at_goal = angular_is_at_goal;
        p_move->start = angular_start;
        p_move->goal = 0.0;
    }
}

static void SetPauseMove(MOVE_ENUM_TYPE move, PAUSE_MOVE_DATA_TYPE const * const p_data)
{
    if (move == MOVE_PAUSE)
    {
        p_move->timeout = p_data->duration;
        p_move->is_at_goal = pause_is_at_goal;
        p_move->start = pause_start;
    }
}

/*---------------------------------------------------------------------------------------------------
 * Motion Module
 *-------------------------------------------------------------------------------------------------*/
void Motion_Init(void)
{   
    UINT8 ii;
    /* Reset state variables */
    for ( ii = 0; ii < 10; ++ii)
    {
        memset(&moves[ii], 0, sizeof(moves[0]));
        moves[ii].start = default_start;
        moves[ii].is_at_goal = default_is_at_goal;
    }
        
    move_index = 0;
    p_move = &moves[move_index];
    move_end = 0;

    start_time = 0;
    is_running = FALSE;
}

void Motion_AddMove(MOVE_ENUM_TYPE move, void * const data)
{
    /* Supported moves:
           - linear move
           - angular move
           - pause move

        Each move is added to a list in order of execution
    */
    
    REQUIRE(in_range(move, MOVE_FIRST, MOVE_LAST));

    p_move = &moves[move_index];
    p_move->type = move;

    SetLinearMove(move, (LINEAR_MOVE_DATA_TYPE const * const) data);
    SetAngularMove(move, (ANGULAR_MOVE_DATA_TYPE const * const) data);
    SetPauseMove(move, (PAUSE_MOVE_DATA_TYPE const * const) data);

    move_index++;
    move_end++;
}

void Motion_Start(void)
{
    Odom_Reset();

    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    cmd_timeout = 0;
    Control_SetCommandVelocityFunc(SetCommandVelocity);

    move_index = 0;
    p_move = &moves[move_index];
    
    is_running = TRUE;
}

BOOL Motion_Update(void)
{
    UINT8 result;

    result = ExecuteMove();
    if (result == MOTION_COMPLETE)
    {
        result = SetNextMove();
        if (result == MOTION_COMPLETE)
        {
            move_index = 0;
            move_end = 0;
            is_running = FALSE;
        }
    }

    return result == MOTION_OK;
}

BOOL Motion_Status(void)
{
    return is_running;
}

void Motion_Results(void)
{
    Control_RestoreCommandVelocityFunc();
}
