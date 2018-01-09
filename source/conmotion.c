#include <stdarg.h>
#include "conmotion.h"
#include "cal.h"
#include "control.h"
#include "odom.h"
#include "encoder.h"
#include "serial.h"
#include "utils.h"
#include "debug.h"
#include "time.h"
#include "assertion.h"

typedef enum {MOTION_FIRST = 0, MOTION_CAL_LINEAR=MOTION_FIRST, MOTION_CAL_ANGULAR, MOTION_CAL_UMBMARK, MOTION_VAL_LINEAR, MOTION_VAL_ANGULAR, MOTION_VAL_CIRCLE,
    MOTION_VAL_SQUARE, MOTION_VAL_OUTANDBACK, MOTION_LAST} MOTION_CMD_TYPE;


typedef struct _tag_motion_val_outandback
{
    FLOAT distance;
} MOTION_VAL_OUTANDBACK_TYPE;
typedef struct _tag_motion_val_circle
{
    BOOL cw;
    FLOAT radius;
} MOTION_VAL_CIRCLE_TYPE;
typedef struct _tag_motion_val_square
{
    BOOL left;
    FLOAT side;
} MOTION_VAL_SQUARE_TYPE;

typedef struct _tag_motion_val_angular
{
    FLOAT angle;
    FLOAT start_heading;
    FLOAT end_heading;
    FLOAT actual_heading;
} MOTION_VAL_ANGULAR_TYPE;

typedef struct _tag_motion_val_linear
{
    FLOAT distance;
    FLOAT start_x_pos;
    FLOAT start_y_pos;
    FLOAT end_x_pos;
    FLOAT end_y_pos;
    FLOAT actual_x_pos;
    FLOAT actual_y_pos;
} MOTION_VAL_LINEAR_TYPE;

typedef struct _tag_motion_cal_umbmark
{
} MOTION_CAL_UMBMARK_TYPE;
typedef struct _tag_motion_cal_angular
{
    FLOAT angle;
} MOTION_CAL_ANGULAR_TYPE;
typedef struct _tag_motion_cal_linear
{
    FLOAT distance;
}MOTION_CAL_LINEAR_TYPE;

typedef struct _tag_move
{
    FLOAT direction;
    FLOAT speed;
    FLOAT timeout;
    FLOAT goal;
    BOOL is_moving;
    FLOAT distance;
    void (*start)(void *);
    BOOL (*is_at_goal)(void *);
} MOVE_TYPE;

static BOOL pause_is_at_goal(void *this);
static void pause_start(void *this);
static BOOL angular_is_at_goal(void *this);
static void angular_start(void *this);
static BOOL linear_is_at_goal(void *this);
static void linear_start(void *this);


typedef enum { MOVE_FIRST=0, MOVE_LINEAR=MOVE_FIRST, MOVE_ANGULAR, MOVE_PAUSE, MOVE_LAST } MOVE_ENUM_TYPE;

static MOVE_TYPE linear_move = {
    DIR_FORWARD, // direction
    0.2, // speed
    10.0, // timeout
    1.0, // goal
    FALSE,
    1.0, // distance
    linear_start,
    linear_is_at_goal
};

static MOVE_TYPE angular_move = {
    DIR_CCW, // direction
    0.3, // speed
    10.0, // timeout
    360, // goal
    FALSE,
    1.0, // distance
    angular_start,
    angular_is_at_goal
};

static MOVE_TYPE pause_move = {
    DIR_NONE, // direction
    0.0, // speed
    1.0, // timeout
    0.0, // goal
    FALSE,
    0.0, // distance
    pause_start,
    pause_is_at_goal
};

static UINT8 move_index = 0;
static MOVE_TYPE * square_move[16] = {
    /* Side 1 */
    &linear_move, 
    &pause_move, 
    &angular_move,
    &pause_move,

    /* Side 2 */
    &linear_move, 
    &pause_move, 
    &angular_move,
    &pause_move,

    /* Side 3 */
    &linear_move, 
    &pause_move, 
    &angular_move,
    &pause_move,

    /* Side 4 */
    &linear_move, 
    &pause_move, 
    &angular_move,
    &pause_move
};

static MOVE_TYPE * oandb_move[8] = 
{
    /* Leg 1 */
    &linear_move, 
    &pause_move, 
    &angular_move,
    &pause_move,

    /* Leg 2 */
    &linear_move, 
    &pause_move, 
    &angular_move,
    &pause_move,
};

static MOVE_TYPE *gen_move_seq[1];
static MOVE_TYPE *p_move;
static MOVE_TYPE **p_move_seq;
static UINT8 max_move_seq;


static BOOL is_running;

static MOTION_CAL_LINEAR_TYPE motion_cal_linear;
static MOTION_CAL_ANGULAR_TYPE motion_cal_angular;
static MOTION_CAL_UMBMARK_TYPE motion_cal_umbmark;
static MOTION_VAL_LINEAR_TYPE motion_val_linear;
static MOTION_VAL_ANGULAR_TYPE motion_val_angular;
static MOTION_VAL_SQUARE_TYPE motion_val_square;
static MOTION_VAL_CIRCLE_TYPE motion_val_circle;
static MOTION_VAL_OUTANDBACK_TYPE motion_val_outandback;

static CONCMD_IF_TYPE cmd_if_array[MOTION_LAST];

static FLOAT cmd_linear_velocity;
static FLOAT cmd_angular_velocity;
static UINT32 cmd_timeout;

static UINT32 start_time;

static void linear_start(void *this)
{   
    FLOAT x_pos;
    FLOAT y_pos;
    FLOAT end_pos;
    MOVE_TYPE *linear = (MOVE_TYPE *)this;

    Ser_WriteLine("Starting Linear ...", TRUE);

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

    Ser_WriteLine("Linear Complete", TRUE);
    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    cmd_timeout = 5000;
    linear->is_moving = FALSE;

    return TRUE;
}

static void angular_start(void *this)
{
    FLOAT heading;
    FLOAT end_heading;
    MOVE_TYPE *linear = (MOVE_TYPE *)this;

    Ser_WriteLine("Starting Rotate ...", TRUE);

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

    Ser_WriteLine("Rotote Complete", TRUE);
    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    linear->is_moving = FALSE;
    return TRUE;
}

static void pause_start(void *this)
{
    MOVE_TYPE *linear = (MOVE_TYPE *)this;

    linear = linear;

    Ser_WriteLine("Starting Pause ...", TRUE);

    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    cmd_timeout = 0;
    linear->is_moving = TRUE;
    start_time = millis();
}

static BOOL pause_is_at_goal(void *this)
{
    UINT32 delta;
    MOVE_TYPE *linear = (MOVE_TYPE *)this;

    /* Note: Wait a small amount of time before evaulating the heading */
    delta = millis() - start_time;
    if (delta < linear->timeout)
    {
        return FALSE;
    }

    Ser_WriteLine("Pause Complete", TRUE);
    linear->is_moving = TRUE;
    return TRUE;
}

static UINT8 ExecuteMove()
{
    if (!p_move->is_moving)
    {
        p_move->start(p_move);
        return VAL_OK;
    }
    else
    {
        if (p_move->is_at_goal(p_move))
        {
            return VAL_COMPLETE;
        }
        return VAL_OK;
    }

    return VAL_COMPLETE;
}

static UINT8 SetNextMove()
{
    move_index++;
    p_move = p_move_seq[move_index];
    
    return move_index == max_move_seq ? VAL_COMPLETE : VAL_OK;
}

static void SetCommandVelocity(FLOAT *linear, FLOAT *angular, UINT32 *timeout)
{
    *linear = cmd_linear_velocity;
    *angular = cmd_angular_velocity;
    *timeout = cmd_timeout;
}

/*------------------------------------------------------------------------------------------
    Motion Calibration Linear
*/
static CONCMD_IF_PTR_TYPE motion_cal_linear_init(FLOAT distance)
{
    motion_cal_linear.distance = distance;

    is_running = TRUE;

    return &cmd_if_array[MOTION_CAL_LINEAR];
}

static BOOL motion_cal_linear_update(void)
{
    is_running = FALSE;
    return is_running;
}

static BOOL motion_cal_linear_status(void)
{
    return is_running;
}

static void motion_cal_linear_results(void)
{
}

/*------------------------------------------------------------------------------------------
    Motion Calibration Angular
*/
static CONCMD_IF_PTR_TYPE motion_cal_angular_init(FLOAT angle)
{
    motion_cal_angular.angle = angle;

    is_running = TRUE;

    return &cmd_if_array[MOTION_CAL_ANGULAR];    
}

static BOOL motion_cal_angular_update(void)
{
    is_running = FALSE;
    return is_running;
}

static BOOL motion_cal_angular_status(void)
{
    return is_running;
}

static void motion_cal_angular_results(void)
{

}

/*------------------------------------------------------------------------------------------
    Motion Calibration UMBMark
*/
static CONCMD_IF_PTR_TYPE motion_cal_umbmark_init(void)
{
    is_running = TRUE;

    return &cmd_if_array[MOTION_CAL_UMBMARK];        
}

static BOOL motion_cal_umbmark_update(void)
{
    is_running = FALSE;
    return is_running;
}

static BOOL motion_cal_umbmark_status(void)
{
    return is_running;
}

static void motion_cal_umbmark_results(void)
{

}

/*------------------------------------------------------------------------------------------
    Motion Validation Linear
*/

static CONCMD_IF_PTR_TYPE motion_val_linear_init(FLOAT distance)
{
    motion_val_linear.distance = distance;
    
    Ser_PutStringFormat("Motion Val Linear Init: %.3f\r\n",
        motion_val_linear.distance);
    
    Odom_Reset();
        
    linear_move.direction = distance > 0 ? DIR_FORWARD : DIR_BACKWARD;
    linear_move.distance = distance;
    linear_move.speed = 0.3;
    linear_move.timeout = 10;

    move_index = 0;
    max_move_seq = 1;
    gen_move_seq[move_index] = &linear_move;
    p_move_seq = &gen_move_seq[move_index];
    p_move = p_move_seq[move_index];

    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    cmd_timeout = 0;
    Control_SetCommandVelocityFunc(SetCommandVelocity);

    is_running = TRUE;

    return &cmd_if_array[MOTION_VAL_LINEAR];
}

static BOOL motion_val_linear_update(void)
{
    UINT8 result;

    result = ExecuteMove();
    if (result == VAL_COMPLETE)
    {
        result = SetNextMove();
    }

    is_running = result == VAL_OK;
    return is_running;
}

static BOOL motion_val_linear_status(void)
{
    return is_running;
}

static void motion_val_linear_results(void)
{
//    Ser_PutStringFormat("Start Position: %.3f, %.3f\r\n", motion_val_linear.start_x_pos, motion_val_linear.start_y_pos);
//    Ser_PutStringFormat("Est End Position: %.3f, %.3f\r\n", motion_val_linear.end_x_pos, motion_val_linear.end_y_pos);
//    Ser_PutStringFormat("Actual End Position: %.3f, %.3f\r\n", motion_val_linear.actual_x_pos, motion_val_linear.actual_y_pos);
//    FLOAT x_error = motion_val_linear.end_x_pos-motion_val_linear.actual_x_pos;
//    FLOAT y_error = motion_val_linear.end_y_pos-motion_val_linear.actual_y_pos;
//    Ser_PutStringFormat("Error/%%Error: x pos %.3f/%.3f, y pos %.3f/%.3f\r\n", x_error, x_error/motion_val_linear.end_x_pos, y_error, y_error/motion_val_linear.end_y_pos);
}

/*------------------------------------------------------------------------------------------
    Motion Validation Angular
*/
static CONCMD_IF_PTR_TYPE motion_val_angular_init(FLOAT angle)
{
    Ser_PutStringFormat("Motion Val Linear Init: %.3f\r\n",
        motion_val_angular.angle);
    
    motion_val_angular.angle = angle;

    Odom_Reset();
    angular_move.direction = angle > 0 ? DIR_CCW : DIR_CW;
    angular_move.distance = angle;
    angular_move.speed = 0.5;
    angular_move.timeout = 10;
    
    move_index = 0;
    max_move_seq = 1;
    gen_move_seq[move_index] = &angular_move;
    p_move_seq = &gen_move_seq[move_index];
    p_move = p_move_seq[move_index];
    
    Control_OverrideDebug(TRUE);
    
    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    cmd_timeout = 0;
    Control_SetCommandVelocityFunc(SetCommandVelocity);    
    
    is_running = TRUE;
    
    start_time = millis();

    return &cmd_if_array[MOTION_VAL_ANGULAR];
}

static BOOL motion_val_angular_update(void)
{
    UINT8 result;

    result = ExecuteMove();
    if (result == VAL_COMPLETE)
    {
        result = SetNextMove();
    }

    is_running = result == VAL_OK;
    return is_running;    
}

static BOOL motion_val_angular_status(void)
{
    return is_running;
}

static void motion_val_angular_results(void)
{
//    Ser_PutStringFormat("Start Heading: %f\r\n", motion_val_angular.start_heading);
//    Ser_PutStringFormat("Est End Heading: %f\r\n", motion_val_angular.end_heading);
//    Ser_PutStringFormat("Actual End Heading: %f\r\n", motion_val_angular.actual_heading);
//    FLOAT heading_error = motion_val_angular.end_heading-motion_val_angular.actual_heading;
//    Ser_PutStringFormat("Error %f/%f\r\n", heading_error, heading_error/motion_val_angular.end_heading);
}

/*------------------------------------------------------------------------------------------
    Motion Validation Square
*/
static CONCMD_IF_PTR_TYPE  motion_val_square_init(BOOL left, FLOAT side)
{
    motion_val_square.left = left;
    motion_val_square.side = side;

    Odom_Reset();

    linear_move.distance = side;
    angular_move.direction = left ? DIR_CCW : DIR_CW;
    
    /* Initialize the move array to have square moves or create a static array and point to it. */
    #define MAX_SQUARE_MOVE (16)
    move_index = 0;
    max_move_seq = MAX_SQUARE_MOVE;
    p_move_seq = &square_move[move_index];
    p_move = p_move_seq[move_index];

    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    cmd_timeout = 0;
    Control_SetCommandVelocityFunc(SetCommandVelocity);
    
    Control_OverrideDebug(TRUE);
    //Debug_Enable(DEBUG_ODOM_ENABLE_BIT|DEBUG_LEFT_ENCODER_ENABLE_BIT|DEBUG_RIGHT_ENCODER_ENABLE_BIT);
    
    is_running = TRUE;

    return &cmd_if_array[MOTION_VAL_SQUARE];
}

static BOOL motion_val_square_update(void)
{
    UINT8 result;

    result = ExecuteMove();
    if (result == VAL_COMPLETE)
    {
        result = SetNextMove();
    }

    is_running = result == VAL_OK;
    return is_running;
}

static BOOL motion_val_square_status(void)
{
    return is_running;
}

static void motion_val_square_results(void)
{
    Control_OverrideDebug(FALSE);
    Control_RestoreCommandVelocityFunc();
}

/*------------------------------------------------------------------------------------------
    Motion Validation Circle
*/
static CONCMD_IF_PTR_TYPE motion_val_circle_init(BOOL cw, FLOAT radius)
{
    motion_val_circle.cw = cw;
    motion_val_circle.radius = radius;
    
    /*
        so, to drive in a circle of a certain radius what is needed?
            specify a robot angular velocity
            specify a radius rotation
    
        Use middle value of Robot Velocity to calculate Robot W
        Set Robot V/W into Control velocity

        Vavg = MAX_WHEEL_METER_PER_SECOND / 2.0;
        Wavg = Vavg/R

        UniToDiff(Vavg, Wavg, Vl, Vr)
        
    
    
    */


    
    is_running = TRUE;

    return &cmd_if_array[MOTION_VAL_CIRCLE];
}

static BOOL motion_val_circle_update(void)
{
    is_running = FALSE;
    return is_running;
}

static BOOL motion_val_circle_status(void)
{
    return is_running;
}

static void motion_val_circle_results(void)
{

}

/*------------------------------------------------------------------------------------------
    Motion Validation Circle
*/
static CONCMD_IF_PTR_TYPE  motion_val_outandback_init(FLOAT distance)
{
    motion_val_outandback.distance = distance;

    linear_move.direction = distance > 0 ? DIR_FORWARD : DIR_BACKWARD;
    linear_move.distance = distance;
    linear_move.speed = 0.2;
    angular_move.direction = DIR_CW;
    angular_move.distance = 180;
    angular_move.speed = 0.5;
    /* Initialize the move array to have square moves or create a static array and point to it. */
    move_index = 0;
    max_move_seq = 8;
    p_move_seq = &oandb_move[move_index];
    p_move = p_move_seq[move_index];

    cmd_linear_velocity = 0.0;
    cmd_angular_velocity = 0.0;
    cmd_timeout = 0;
    Control_SetCommandVelocityFunc(SetCommandVelocity);
    
    Control_OverrideDebug(TRUE);
    //Debug_Enable(DEBUG_ODOM_ENABLE_BIT|DEBUG_LEFT_ENCODER_ENABLE_BIT|DEBUG_RIGHT_ENCODER_ENABLE_BIT);
    
    is_running = TRUE;

    return &cmd_if_array[MOTION_VAL_OUTANDBACK];
}

static BOOL motion_val_outandback_update(void)
{
    UINT8 result;

    result = ExecuteMove();
    if (result == VAL_COMPLETE)
    {
        result = SetNextMove();
    }

    is_running = result == VAL_OK;
    return is_running;
}

static BOOL motion_val_outandback_status(void)
{
    return is_running;
}

static void motion_val_outandback_results(void)
{

}

/*------------------------------------------------------------------------------------------
    Motion Module
*/
void ConMotion_Init(void)
{
    cmd_if_array[MOTION_CAL_LINEAR].update = motion_cal_linear_update;
    cmd_if_array[MOTION_CAL_LINEAR].status = motion_cal_linear_status;
    cmd_if_array[MOTION_CAL_LINEAR].results = motion_cal_linear_results;    
    cmd_if_array[MOTION_CAL_ANGULAR].update = motion_cal_angular_update;
    cmd_if_array[MOTION_CAL_ANGULAR].status = motion_cal_angular_status;
    cmd_if_array[MOTION_CAL_ANGULAR].results = motion_cal_angular_results;
    cmd_if_array[MOTION_CAL_UMBMARK].update = motion_cal_umbmark_update;
    cmd_if_array[MOTION_CAL_UMBMARK].status = motion_cal_umbmark_status;
    cmd_if_array[MOTION_CAL_UMBMARK].results = motion_cal_umbmark_results;

    cmd_if_array[MOTION_VAL_LINEAR].update = motion_val_linear_update;
    cmd_if_array[MOTION_VAL_LINEAR].status = motion_val_linear_status;
    cmd_if_array[MOTION_VAL_LINEAR].results = motion_val_linear_results;
    cmd_if_array[MOTION_VAL_ANGULAR].update = motion_val_angular_update;
    cmd_if_array[MOTION_VAL_ANGULAR].status = motion_val_angular_status;
    cmd_if_array[MOTION_VAL_ANGULAR].results = motion_val_angular_results;
    cmd_if_array[MOTION_VAL_CIRCLE].update = motion_val_circle_update;
    cmd_if_array[MOTION_VAL_CIRCLE].status = motion_val_circle_status;
    cmd_if_array[MOTION_VAL_CIRCLE].results = motion_val_circle_results;
    cmd_if_array[MOTION_VAL_SQUARE].update = motion_val_square_update;
    cmd_if_array[MOTION_VAL_SQUARE].status = motion_val_square_status;
    cmd_if_array[MOTION_VAL_SQUARE].results = motion_val_square_results;
    cmd_if_array[MOTION_VAL_OUTANDBACK].update = motion_val_outandback_update;
    cmd_if_array[MOTION_VAL_OUTANDBACK].status = motion_val_outandback_status;
    cmd_if_array[MOTION_VAL_OUTANDBACK].results = motion_val_outandback_results;

    is_running = FALSE;
}

CONCMD_IF_PTR_TYPE ConMotion_InitCalLinear(FLOAT distance)
{
    return motion_cal_linear_init(distance);
}

CONCMD_IF_PTR_TYPE ConMotion_InitMotionCalAngular(FLOAT angle)
{
    return motion_cal_angular_init(angle);
}

CONCMD_IF_PTR_TYPE ConMotion_InitMotionCalUmbmark(void)
{
    return motion_cal_umbmark_init();
}

CONCMD_IF_PTR_TYPE ConMotion_InitMotionValLinear(FLOAT distance)
{
    return motion_val_linear_init(distance);
}

CONCMD_IF_PTR_TYPE ConMotion_InitMotionValAngular(FLOAT angle)
{
    return motion_val_angular_init(angle);
}

CONCMD_IF_PTR_TYPE ConMotion_InitMotionValSquare(BOOL left, FLOAT side)
{
    return motion_val_square_init(left, side);
}

CONCMD_IF_PTR_TYPE ConMotion_InitMotionValCircle(BOOL cw, FLOAT radius)
{
    return motion_val_circle_init(cw, radius);
}

CONCMD_IF_PTR_TYPE ConMotion_InitMotionValOutAndBack(FLOAT distance)
{
    return motion_val_outandback_init(distance);
}

void ConMotion_Start(void)
{

}

