#include <stdarg.h>
#include "conmotor.h"
#include "serial.h"
#include "consts.h"
#include "pwm.h"
#include "motor.h"
#include "types.h"
#include "utils.h"
#include "time.h"
#include "cal.h"
#include "control.h"
#include "pid.h"
#include "calmotor.h"
#include "valmotor.h"
#include "debug.h"

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

typedef enum {MOTOR_FIRST=0, 
              MOTOR_REPEAT=MOTOR_FIRST, 
              MOTOR_CAL, 
              MOTOR_VAL, 
              MOTOR_MOVE, 
              MOTOR_SHOW, 
              MOTOR_LAST} MOTOR_CMD_TYPE;

typedef struct _tag_motor_repeat
{
    BOOL is_first;
    WHEEL_TYPE wheel;
    FLOAT first;
    FLOAT second;
    FLOAT interval;
    UINT8 iterations;
    BOOL no_pid;
    BOOL no_accel;
} MOTOR_REPEAT_TYPE;

typedef struct _tag_motor_move
{
    FLOAT left;
    FLOAT right;
    FLOAT duration;
    BOOL no_pid;
    BOOL no_accel;
} MOTOR_MOVE_TYPE;

typedef struct _tag_motor_cal
{
    WHEEL_TYPE wheel;
    UINT8 iters;
} MOTOR_CAL_TYPE;

typedef struct _tag_motor_val
{
    WHEEL_TYPE wheel;
    DIR_TYPE direction;
    FLOAT min_percent;
    FLOAT max_percent;
    INT8 num_points;    
} MOTOR_VAL_TYPE;

typedef struct _tag_motor_show
{
    WHEEL_TYPE wheel;
    BOOL plain_text;
} MOTOR_SHOW_TYPE;


static BOOL is_running;

static MOTOR_REPEAT_TYPE motor_repeat;
static MOTOR_MOVE_TYPE motor_move;
static MOTOR_CAL_TYPE motor_cal;
static MOTOR_VAL_TYPE motor_val;
static MOTOR_SHOW_TYPE motor_show;

static UINT32 last_time;

static FLOAT velocity_cmd_linear;
static FLOAT velocity_cmd_angular;

static CONCMD_IF_TYPE cmd_if_array[MOTOR_LAST];


void VelocityCmd(FLOAT *linear, FLOAT *angular, UINT32 *timeout)
{
    *linear = velocity_cmd_linear;
    *angular = velocity_cmd_angular;
    *timeout = 0;
}

static void SetLeftRightSpeed(FLOAT left, FLOAT right)
{
    /* Convert m/s to r/s 
        m/s * r/m
    */
    left = left * WHEEL_RADIAN_PER_METER;
    right = right * WHEEL_RADIAN_PER_METER;
    DiffToUni(left, right, &velocity_cmd_linear, &velocity_cmd_angular);
}

static void SetWheelSpeed(WHEEL_TYPE wheel, FLOAT value)
{
    FLOAT left = 0.0;
    FLOAT right = 0.0;

    if (wheel == WHEEL_BOTH)
    {
        left = value;
        right = value;
    }
    else if (wheel == WHEEL_LEFT)
    {
        left = value;
    }
    else if (wheel == WHEEL_RIGHT)
    {
        right = value;
    }
        
    SetLeftRightSpeed(left, right);
}


/*----------------------------------------------------------------------------
    Motor Repeat Routines
*/
static CONCMD_IF_PTR_TYPE motor_repeat_init(WHEEL_TYPE wheel, FLOAT first, FLOAT second, FLOAT interval, INT8 interations, BOOL no_pid, BOOL no_accel)
{
    Ser_WriteLine("Starting Motor Repeat", TRUE);
    
    /* Note: Parser requires all three parameters to be present so they will always have a value */
    if ( !in_range(first, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) ||
         !in_range(second, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) ||
         !in_range(interval, MIN_INTERVAL, MAX_INTERVAL))
    {
        return (CONCMD_IF_TYPE *) NULL;
    }
    
    motor_repeat.wheel = wheel;
    motor_repeat.first = first;
    motor_repeat.second = second;
    motor_repeat.interval = (UINT32)(interval * 1000);
    motor_repeat.iterations = interations;
    motor_repeat.no_pid = no_pid;
    motor_repeat.no_accel = no_accel;

    Ser_PutStringFormat("Motor Repeat: %d %f %f %f %d\r\n", 
        motor_repeat.wheel,
        motor_repeat.first,
        motor_repeat.second,
        interval,
        motor_repeat.iterations);

    Pid_BypassAll(motor_repeat.no_pid);
    Control_EnableAcceleration(!motor_repeat.no_accel);
    Control_SetCommandVelocityFunc(VelocityCmd);
                

    last_time = millis();
    Ser_WriteLine("Motor Repeat setting first speed", TRUE);
    SetWheelSpeed(motor_repeat.wheel, motor_repeat.first);
    motor_repeat.is_first = FALSE;

    is_running = TRUE;
    return &cmd_if_array[MOTOR_REPEAT];
}

static BOOL motor_repeat_update(void)
{
    UINT32 delta;

    delta = millis() - last_time;
    if (delta >= motor_repeat.interval)
    {
        last_time = millis();
        if (motor_repeat.is_first)
        {
            Ser_WriteLine("Motor Repeat setting first speed", TRUE);
            SetWheelSpeed(motor_repeat.wheel, motor_repeat.first);
            motor_repeat.is_first = FALSE;
        }
        else
        {
            Ser_WriteLine("Motor Repeat setting second speed", TRUE);
            SetWheelSpeed(motor_repeat.wheel, motor_repeat.second);
            motor_repeat.is_first = TRUE;

            Ser_PutStringFormat("Motor Repeat end iteration %d\r\n", motor_repeat.iterations);
            motor_repeat.iterations--;
            if (motor_repeat.iterations == 0)
            {
                SetWheelSpeed(0.0, 0.0);
                Pid_BypassAll(FALSE);
                Control_EnableAcceleration(TRUE);
                Control_RestoreCommandVelocityFunc();
                is_running = FALSE;
            }
        }
    }

    return is_running;
}

static BOOL motor_repeat_status(void)
{
    return is_running;
}
static void motor_repeat_results(void)
{
    Ser_WriteLine("Motor Repeat Results", TRUE);
}

/*----------------------------------------------------------------------------
    Motor Calibration Routines
*/
static CONCMD_IF_PTR_TYPE motor_cal_init(WHEEL_TYPE wheel, INT8 iters)
{
    Ser_WriteLine("Starting Motor Calibration", TRUE);
    
    if (!in_range(iters, MIN_CAL_ITER, MAX_CAL_ITER))
    {
        return FALSE;
    }
    
    motor_cal.wheel = wheel;
    motor_cal.iters = iters;

    Ser_WriteLine("Initialize motor calibration", TRUE);
    Debug_Store();
    Control_OverrideDebug(TRUE);
    Cal_ClearCalibrationStatusBit(CAL_MOTOR_BIT);
    Pid_Enable(FALSE, FALSE, FALSE);
    Ser_WriteLine("Performing motor calibration", TRUE);

    CalMotor_Init(motor_cal.wheel, motor_cal.iters);

    is_running = TRUE;
    return &cmd_if_array[MOTOR_CAL];
}
static BOOL motor_cal_update(void)
{
    UINT8 result;
    
    result = CalMotor_Update();
    is_running = result == CAL_OK;

    return is_running;
}
static BOOL motor_cal_status(void)
{
    return is_running;
}

static void motor_cal_results(void)
{
    Ser_WriteLine("Motor calibration complete", TRUE);
    Cal_SetCalibrationStatusBit(CAL_MOTOR_BIT);
    Debug_Restore();
    Ser_WriteLine("\r\nPrinting motor calibration results", TRUE);
    
    Cal_PrintMotorParams(motor_cal.wheel, TRUE);
}

/*----------------------------------------------------------------------------
    Motor Validation Routines
*/
static CONCMD_IF_PTR_TYPE motor_val_init(WHEEL_TYPE wheel, DIR_TYPE direction, FLOAT min_percent, FLOAT max_percent, INT8 num_points)
{
    Ser_WriteLine("Starting Motor Validation", TRUE);
    
    if (!in_range_float(min_percent, MIN_PERCENT, MAX_PERCENT) || 
        !in_range_float(max_percent, MIN_PERCENT, MAX_PERCENT) || 
        min_percent >= max_percent || 
        is_even(num_points) || 
        !in_range(num_points, MIN_NUM_POINTS, MAX_NUM_POINTS))
    {
        return (CONCMD_IF_TYPE *) NULL;
    }

    motor_val.wheel = wheel;
    motor_val.direction = direction;
    motor_val.min_percent = min_percent;
    motor_val.max_percent = max_percent;
    motor_val.num_points = num_points;
    

    Ser_WriteLine("Initialize motor validation", TRUE);
    Debug_Store();
    Control_OverrideDebug(TRUE);

    /* Setup the PIDs to be bypassed, i.e., they pass the target unchanged to the output. */
    Pid_BypassAll(TRUE);
    Control_SetLeftRightVelocityOverride(TRUE);
    Control_SetLeftRightVelocityCps(0, 0);

    Ser_WriteLine("Performing motor validation", TRUE);
    ValMotor_Init(motor_val.wheel, motor_val.direction, motor_val.min_percent, motor_val.max_percent, motor_val.num_points);
    
    is_running = TRUE;    
    return &cmd_if_array[MOTOR_VAL];
}

static BOOL motor_val_update(void)
{
    UINT8 result;

    result = ValMotor_Update();

    is_running = result == VAL_OK;;
    return is_running;
}

static BOOL motor_val_status(void)
{
    return is_running;
}

static void motor_val_results(void)
{
    Ser_PutString("Motor validation complete\r\n");
    Control_SetLeftRightVelocityCps(0, 0);
    Control_SetLeftRightVelocityOverride(FALSE);
    Pid_BypassAll(FALSE);
    Control_OverrideDebug(FALSE);
    Debug_Restore();    
}

/*----------------------------------------------------------------------------
    Motor Move Routines
*/
static CONCMD_IF_PTR_TYPE motor_move_init(FLOAT left, FLOAT right, FLOAT duration, BOOL no_pid, BOOL no_accel)
{    
    left = IS_VALID_FLOAT(left) ? left : 0.0;
    right = IS_VALID_FLOAT(right) ? right : 0.0;

    if (!in_range_float(left, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) ||
        !in_range_float(right, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) ||
        !in_range_float(duration, MIN_DURATION, MAX_DURATION))
    {
        return (CONCMD_IF_TYPE *) NULL;
    }
    
    motor_move.left = left;
    motor_move.right = right;
    motor_move.duration = (UINT32) (duration * 1000);
    motor_move.no_pid = no_pid;
    motor_move.no_accel = no_accel;
    
    Ser_PutStringFormat("Motor Move: %.3f %.3f %.3f %d %d\r\n", 
        motor_move.left,
        motor_move.right,
        motor_move.duration,
        motor_move.no_pid,
        motor_move.no_accel);

    Pid_BypassAll(motor_move.no_pid);
    Control_EnableAcceleration(!motor_move.no_accel);
    Control_SetCommandVelocityFunc(VelocityCmd);
    SetLeftRightSpeed(0.0, 0.0);
    SetLeftRightSpeed(motor_move.left, motor_move.right);
        
    last_time = millis();
    is_running = TRUE;
    
    return &cmd_if_array[MOTOR_MOVE];
}

static BOOL motor_move_update(void)
{
    UINT32 delta;
    
    delta = millis() - last_time;
    if (delta < motor_move.duration)
    {
        last_time = millis();
        motor_move.duration -= delta;
    }    
    else
    {
        SetLeftRightSpeed(0.0, 0.0);
        Control_EnableAcceleration(TRUE);
        Pid_BypassAll(FALSE);
        Control_RestoreCommandVelocityFunc();

        is_running = FALSE;
    }
    
    return is_running;
}
static BOOL motor_move_status(void)
{
    return is_running;
}
static void motor_move_results(void)
{
}

/*----------------------------------------------------------------------------
    Motor Show Routines
*/
static CONCMD_IF_PTR_TYPE motor_show_init(WHEEL_TYPE wheel, BOOL plain_text)
{
    motor_show.wheel = wheel;
    motor_show.plain_text = plain_text;
    
    is_running = TRUE;
    Ser_WriteLine("Motor Show Init", TRUE);

    return &cmd_if_array[MOTOR_SHOW];
}

static BOOL motor_show_update(void)
{
    is_running = FALSE;
    return is_running;
}
static BOOL motor_show_status(void)
{
    return is_running;
}
static void motor_show_results(void)
{
    Cal_PrintMotorParams(motor_show.wheel, !motor_show.plain_text);
}

/*----------------------------------------------------------------------------
    ConMotor Module
*/

void ConMotor_Init(void)
{
    cmd_if_array[MOTOR_REPEAT].update = motor_repeat_update;
    cmd_if_array[MOTOR_REPEAT].status = motor_repeat_status;
    cmd_if_array[MOTOR_REPEAT].results = motor_repeat_results;
    cmd_if_array[MOTOR_CAL].update = motor_cal_update;
    cmd_if_array[MOTOR_CAL].status = motor_cal_status;
    cmd_if_array[MOTOR_CAL].results = motor_cal_results;
    cmd_if_array[MOTOR_VAL].update = motor_val_update;
    cmd_if_array[MOTOR_VAL].status = motor_val_status;
    cmd_if_array[MOTOR_VAL].results = motor_val_results;
    cmd_if_array[MOTOR_MOVE].update = motor_move_update;
    cmd_if_array[MOTOR_MOVE].status = motor_move_status;
    cmd_if_array[MOTOR_MOVE].results = motor_move_results;
    cmd_if_array[MOTOR_SHOW].update = motor_show_update;
    cmd_if_array[MOTOR_SHOW].status = motor_show_status;
    cmd_if_array[MOTOR_SHOW].results = motor_show_results;

    memset(&motor_repeat, 0, sizeof motor_repeat);
    memset(&motor_move, 0, sizeof motor_move);
    memset(&motor_cal, 0, sizeof motor_cal);
    memset(&motor_val, 0, sizeof motor_val);

    is_running = FALSE;
}

void ConMotor_Start(void)
{

}

CONCMD_IF_PTR_TYPE ConMotor_InitMotorShow(WHEEL_TYPE wheel, BOOL plain_text)
{
    return motor_show_init(wheel, plain_text);
}

CONCMD_IF_PTR_TYPE ConMotor_InitMotorRepeat(
            WHEEL_TYPE wheel,
            FLOAT first,
            FLOAT second,
            FLOAT intvl,
            INT8 iters,
            BOOL no_pid,
            BOOL no_accel)
{
    return motor_repeat_init(wheel, first, second, intvl, iters, no_pid, no_accel);
}

CONCMD_IF_PTR_TYPE ConMotor_InitMotorCal(
            WHEEL_TYPE wheel,
            INT8 iters)
{
    return motor_cal_init(wheel, iters);
}
            
CONCMD_IF_PTR_TYPE ConMotor_InitMotorVal(
            WHEEL_TYPE wheel,
            DIR_TYPE direction,
            FLOAT min_percent,
            FLOAT max_percent,
            INT8 num_points)
{
    return motor_val_init(wheel, direction, min_percent, max_percent, num_points);
}
            
CONCMD_IF_PTR_TYPE ConMotor_InitMotorMove(
            FLOAT left_speed,
            FLOAT right_speed,
            FLOAT duration,
            BOOL no_pid,
            BOOL no_accel)
{
    return motor_move_init(left_speed, right_speed, duration, no_pid, no_accel);
}