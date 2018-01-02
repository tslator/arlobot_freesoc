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
#include "debug.h"

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

static BOOL is_running;

static MOTOR_REPEAT_TYPE motor_repeat;
static MOTOR_MOVE_TYPE motor_move;
static MOTOR_CAL_TYPE motor_cal;

static UINT32 last_time;

static FLOAT velocity_cmd_linear;
static FLOAT velocity_cmd_angular;

void VelocityCmd(FLOAT *linear, FLOAT *angular, UINT32 *timeout)
{
    *linear = velocity_cmd_linear;
    *angular = velocity_cmd_angular;
    *timeout = 0;
}

/*----------------------------------------------------------------------------
    Motor Repeat Routines
*/
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

static void motor_repeat_init(INT8 wheel, FLOAT first, FLOAT second, FLOAT interval, UINT8 iterations, BOOL no_pid, BOOL no_accel)
{
    Ser_WriteLine("Starting Motor Repeat", TRUE);
    
    motor_repeat.wheel = wheel;
    motor_repeat.first = first;
    motor_repeat.second = second;
    motor_repeat.interval = (UINT32)(interval * 1000);
    motor_repeat.iterations = iterations;
    motor_repeat.no_pid = no_pid;
    motor_repeat.no_accel = no_accel;

    Ser_PutStringFormat("Motor Repeat: %d %f %f %f %d\r\n", 
        motor_repeat.wheel,
        motor_repeat.first,
        motor_repeat.second,
        motor_repeat.interval,
        motor_repeat.iterations);

    Pid_BypassAll(motor_repeat.no_pid);
    Control_EnableAcceleration(!motor_repeat.no_accel);
    Control_SetCommandVelocityFunc(VelocityCmd);
                

    last_time = millis();
    is_running = TRUE;

    last_time = millis();
    Ser_WriteLine("Motor Repeat setting first speed", TRUE);
    SetWheelSpeed(motor_repeat.wheel, motor_repeat.first);
    motor_repeat.is_first = FALSE;

    is_running = TRUE;
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
static void motor_cal_init(WHEEL_TYPE wheel, INT8 iters)
{
    Ser_WriteLine("Starting Motor Calibration", TRUE);
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

    if (motor_cal.wheel == WHEEL_LEFT)
    {
        Cal_PrintLeftMotorParams(TRUE);
    }
    else if (motor_cal.wheel == WHEEL_RIGHT)
    {
        Cal_PrintRightMotorParams(TRUE);
    }
    else // WHEEL_BOTH
    {
        Cal_PrintAllMotorParams(TRUE);
    }
}

/*----------------------------------------------------------------------------
    Motor Validation Routines
*/
static void motor_val_init(FLOAT min_percent, FLOAT max_percent, INT8 num_points)
{
    is_running = TRUE;
    Ser_WriteLine("Starting Motor Validation", TRUE);
}
static BOOL motor_val_update(void)
{
    Ser_WriteLine("Motor Validation Update", TRUE);
    is_running = FALSE;
    return is_running;
}
static BOOL motor_val_status(void)
{
    Ser_WriteLine("Motor Validation Running ...", TRUE);
    return is_running;
}
static void motor_val_results(void)
{
    Ser_WriteLine("Motor Validation Results", TRUE);
}

/*----------------------------------------------------------------------------
    Motor Move Routines
*/
static void motor_move_init(FLOAT left_speed, FLOAT right_speed, FLOAT duration, BOOL no_pid, BOOL no_accel)
{
    Ser_WriteLine("Starting Motor Move", TRUE);
    
    motor_move.left = left_speed;
    motor_move.right = right_speed;
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
    Ser_WriteLine("Motor Move Results", TRUE);
}

/*----------------------------------------------------------------------------
    Motor Show Routines
*/
static void motor_show_init(WHEEL_TYPE wheel, BOOL plain_text)
{
    is_running = TRUE;
    Ser_WriteLine("Motor Show Init", TRUE);
}

static BOOL motor_show_update(void)
{
    Ser_WriteLine("Motor Show Update", TRUE);
    is_running = FALSE;
    return is_running;
}
static BOOL motor_show_status(void)
{
    Ser_WriteLine("Motor Show Running ...", TRUE);
    return is_running;
}
static void motor_show_results(void)
{
    Ser_WriteLine("Motor Show Results", TRUE);
}

/*----------------------------------------------------------------------------
    ConMotor Module
*/

void ConMotor_Init(void)
{
    is_running = FALSE;
}

void ConMotor_Start(void)
{

}

BOOL ConMotor_Assign(COMMAND_IF_TYPE *p_cmdif, ...)
{
    va_list valist;
    BOOL result = FALSE;
    int cmd;

    va_start(valist, p_cmdif);
    cmd = va_arg(valist, int);
    
    switch (cmd)
    {
        case MOTOR_SHOW:
        {
            INT8 int_wheel = va_arg(valist, int);
            BOOL plain_text = va_arg(valist, int);

            if (int_wheel < 0)
            {
                break;
            }
            
            WHEEL_TYPE wheel = (WHEEL_TYPE) int_wheel;
            
            motor_show_init(wheel, plain_text);
            p_cmdif->update = motor_show_update;
            p_cmdif->status = motor_show_status;
            p_cmdif->results = motor_show_results;
            
            p_cmdif->is_assigned = TRUE;
            result = TRUE;
            break;
        }    
        
        case MOTOR_REPEAT:
        {
            /* initialize valist for num number of arguments */
            INT8 int_wheel = va_arg(valist, int);
            FLOAT first = va_arg(valist, double);
            FLOAT second = va_arg(valist, double);
            FLOAT interval = va_arg(valist, double);
            INT8  iters = va_arg(valist, int);
            BOOL  no_pid = va_arg(valist, int);
            BOOL  no_accel = va_arg(valist, int);

            if (int_wheel < 0)
            {
                break;
            }
            
            WHEEL_TYPE wheel = (WHEEL_TYPE) int_wheel;

            /* Validate the values */
            
            /* Note: Parser requires all three parameters to be present so they will always have a value */
            if ( in_range(first, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) && 
                 in_range(second, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) && 
                 in_range(interval, 0.001, 10))
            {            
                /* first, second, interval */
                motor_repeat_init(wheel, first, second, interval, iters, no_pid, no_accel);
                p_cmdif->update = motor_repeat_update;
                p_cmdif->status = motor_repeat_status;
                p_cmdif->results = motor_repeat_results;
                
                p_cmdif->is_assigned = TRUE;
                
                result = TRUE;
            }            
            break;
        }
        
        case MOTOR_CAL:
        {
            INT8 int_wheel = va_arg(valist, int);
            INT8 iters = va_arg(valist, int);
            
            if (int_wheel < 0)
            {
                break;
            }
            
            WHEEL_TYPE wheel = (WHEEL_TYPE) int_wheel;
            
            if (in_range(iters, 1, 10))
            {
                motor_cal_init(wheel, iters);
                p_cmdif->update = motor_cal_update;
                p_cmdif->status = motor_cal_status;
                p_cmdif->results = motor_cal_results;
                
                p_cmdif->is_assigned = TRUE;
                result = TRUE;
            }
            break;
        }
        
        case MOTOR_VAL:
        {
            FLOAT min_percent = va_arg(valist, double);
            FLOAT max_percent = va_arg(valist, double);
            INT8 num_points = va_arg(valist, int);
            if (in_range(min_percent, 0.2, 0.8) && in_range(max_percent, 0.2, 0.8) && min_percent < max_percent &&
            num_points >= 3 && num_points % 2 != 0)
            {
                motor_val_init(min_percent, max_percent, num_points);
                p_cmdif->update = motor_val_update;
                p_cmdif->status = motor_val_status;
                p_cmdif->results = motor_val_results;

                p_cmdif->is_assigned = TRUE;
                result = TRUE;
            }
            break;
        }
        
        case MOTOR_MOVE:
        {
            FLOAT left_speed = va_arg(valist, double);
            FLOAT right_speed = va_arg(valist, double);
            FLOAT duration = va_arg(valist, double);
            BOOL no_pid = va_arg(valist, int);
            BOOL no_accel = va_arg(valist, int);
                        
            if (!IS_SPECIFIED(left_speed))
            {
                left_speed = 0;
            }
            
            if (!IS_SPECIFIED(right_speed))
            {
                right_speed = 0;
            }
  
            if (in_range(left_speed, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) &&
                in_range(right_speed, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY) &&
                in_range(duration, 0.001, 60))
            {            
                motor_move_init(left_speed, right_speed, duration, no_pid, no_accel);
                p_cmdif->update = motor_move_update;
                p_cmdif->status = motor_move_status;
                p_cmdif->results = motor_move_results;

                p_cmdif->is_assigned = TRUE;
                result = TRUE;
            }
            
            break;
        }
    };

    /* clean memory reserved for valist */
    va_end(valist);
    
    return result;
}
