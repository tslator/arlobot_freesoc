#include <stdlib.h>
#include <stdarg.h>
#include "conpid.h"
#include "serial.h"
#include "cal.h"
#include "calstore.h"
#include "calpid.h"
#include "control.h"
#include "encoder.h"
#include "odom.h"
#include "pid.h"
#include "pidleft.h"
#include "pidright.h"
#include "valpid.h"
#include "utils.h"
#include "debug.h"
#include "assertion.h"

typedef struct _tag_pid_show
{
    WHEEL_TYPE wheel;
    BOOL as_json;
} PID_SHOW_TYPE;

typedef struct _tag_pid_cal
{
    WHEEL_TYPE wheel;
    BOOL impulse;
    FLOAT step;
    BOOL with_debug;
} PID_CAL_TYPE;

typedef struct _tag_pid_val
{
    WHEEL_TYPE wheel;            
    DIR_TYPE direction;
    FLOAT min_percent;
    FLOAT max_percent;
    UINT8 num_points;
} PID_VAL_TYPE;

typedef enum {PID_SHOW, PID_CAL, PID_VAL, PID_LAST} PID_CMD_TYPE;
    

static BOOL is_running;
static PID_SHOW_TYPE pid_show;
static PID_CAL_TYPE pid_cal;
static PID_VAL_TYPE pid_val;

static CONCMD_IF_TYPE cmd_if_array[PID_LAST];


/*----------------------------------------------------------------------------
    PID Show Routines
*/
static CONCMD_IF_PTR_TYPE pid_show_init(WHEEL_TYPE wheel, BOOL plain_text)
{
    Ser_PutStringFormat("Wheel: %d, Plain Text: %d\r\n", wheel, plain_text);

    pid_show.wheel = wheel;
    pid_show.as_json = !plain_text;
    
    is_running = TRUE;
    return &cmd_if_array[PID_SHOW];
}

static BOOL pid_show_update(void)
{
    is_running = FALSE;
    return is_running;
}

static BOOL pid_show_status(void)
{
    return is_running;
}

static void pid_show_results(void)
{
    switch (pid_show.wheel)
    {
        case WHEEL_LEFT:
            Cal_PrintLeftPidGains(pid_show.as_json);
            break;

        case WHEEL_RIGHT:
            Cal_PrintRightPidGains(pid_show.as_json);
            break;

        case WHEEL_BOTH:
        default:
            Cal_PrintAllPidGains(pid_show.as_json);
    }
}

/*----------------------------------------------------------------------------
    PID Calibration Routines
*/
static CONCMD_IF_PTR_TYPE pid_cal_init(WHEEL_TYPE wheel, BOOL impulse, FLOAT step, BOOL with_debug)
{
    Ser_WriteLine("PID Calibration Init", TRUE);

    if (!Cal_GetCalibrationStatusBit(CAL_MOTOR_BIT))
    {
        Ser_PutStringFormat("Motor calibration not performed (%02x)\r\n", Cal_GetStatus());
        return (CONCMD_IF_TYPE *) NULL;
    }

    pid_cal.wheel = wheel;
    pid_cal.impulse = impulse;
    pid_cal.step = step;
    pid_cal.with_debug = TRUE; //with_debug;

    Ser_PutStringFormat("wheel: %s impulse: %d, step: %.3f, with_debug: %d\r\n", 
                        WheelToString(pid_cal.wheel, FORMAT_LOWER), 
                        pid_cal.impulse, 
                        pid_cal.step, 
                        pid_cal.with_debug); 

    /* Note: By default, calibration should always have debug.  Change the console arguments to use --no-debug instead of --with-debug */
    CalPid_Init(wheel, impulse, step, !pid_cal.with_debug);

    is_running = TRUE;
    return &cmd_if_array[PID_CAL];
}

static BOOL pid_cal_update(void)
{
    UINT8 result;
    
    result = CalPid_Update();
    is_running = result == CAL_OK;
    
    return is_running;
}

static BOOL pid_cal_status(void)
{
    //Ser_WriteLine("Do you want the gains to be stored (y/n)? ", TRUE);
    return is_running;
}

static void pid_cal_results(void)
{
    FLOAT gains[4];

    Ser_PutStringFormat("\r\n%s PID calibration complete\r\n", pid_cal.wheel == WHEEL_LEFT ? "Left" : "Right");
    Control_SetLeftRightVelocityCps(0, 0);
    Control_SetLeftRightVelocityOverride(FALSE);
    Debug_Restore();    

    /* Ask if the gains should be committed */
    switch (pid_cal.wheel)
    {
        case WHEEL_LEFT:
            LeftPid_GetGains(&gains[0], &gains[1], &gains[2], &gains[3]);
            Cal_SetGains(PID_TYPE_LEFT, gains);
            break;
            
        case WHEEL_RIGHT:
            RightPid_GetGains(&gains[0], &gains[1], &gains[2], &gains[3]);
            Cal_SetGains(PID_TYPE_RIGHT, gains);
            break;

        default:
            ASSERTION(FALSE, "Wheel option not allowed");
            Ser_WriteLine("Invalid option", TRUE);
            break;
    }

    Cal_SetCalibrationStatusBit(CAL_PID_BIT);

    Ser_PutString("\r\nPrinting PID calibration results\r\n");
    switch (pid_cal.wheel)
    {
        case WHEEL_LEFT:
            Cal_PrintLeftPidGains(TRUE);
            break;
        
        case WHEEL_RIGHT:
            Cal_PrintRightPidGains(TRUE);
            break;
            
        default:
            break;
    }    

    /* Note: I want to calculate these parameters after each calibration run
    https://www.mathworks.com/help/control/ref/stepinfo.html?requestedDomain=www.mathworks.com    
    http://www.mee.tcd.ie/~corrigad/3c1/control_ho2_2012_students.pdf
    */
}

/*----------------------------------------------------------------------------
    PID Validation Routines
*/
static CONCMD_IF_PTR_TYPE pid_val_init(WHEEL_TYPE wheel, DIR_TYPE direction,
            FLOAT min_percent,
            FLOAT max_percent,
            INT8 num_points)
{
    if (!in_range_float(min_percent, 0.2, 0.8) ||
        !in_range_float(max_percent, 0.2, 0.8) || 
        is_even(num_points) ||
        !in_range(num_points, 3, 13))
    {
        return (CONCMD_IF_TYPE *) NULL;
    }

    pid_val.wheel = wheel;
    pid_val.direction = direction;
    pid_val.min_percent = min_percent;
    pid_val.max_percent = max_percent;
    pid_val.num_points = num_points;

    ValPid_Init(pid_val.wheel, pid_val.direction, pid_val.min_percent, pid_val.max_percent, pid_val.num_points);    
    
    is_running = TRUE;
    return &cmd_if_array[PID_VAL];
}

static BOOL pid_val_update(void)
{
    UINT8 result;

    result = ValPid_Update();
    is_running = result == VAL_OK;
    return is_running;
}
static BOOL pid_val_status(void)
{
    return is_running;
}
static void pid_val_results(void)
{
    ValPid_Results();
}

/*----------------------------------------------------------------------------
    ConPid Module
*/

void ConPid_Init(void)
{
    cmd_if_array[PID_SHOW].update = pid_show_update;
    cmd_if_array[PID_SHOW].status = pid_show_status;
    cmd_if_array[PID_SHOW].results = pid_show_results;
    cmd_if_array[PID_CAL].update = pid_cal_update;
    cmd_if_array[PID_CAL].status = pid_cal_status;
    cmd_if_array[PID_CAL].results = pid_cal_results;
    cmd_if_array[PID_VAL].update = pid_val_update;
    cmd_if_array[PID_VAL].status = pid_val_status;
    cmd_if_array[PID_VAL].results = pid_val_results;

    memset(&pid_show, 0, sizeof pid_show);
    memset(&pid_cal, 0, sizeof pid_cal);
    memset(&pid_val, 0, sizeof pid_val);
    
    is_running = FALSE;
}

void ConPid_Start(void)
{

}

CONCMD_IF_PTR_TYPE ConPid_InitPidShow(WHEEL_TYPE wheel, BOOL plain_text)
{
    return pid_show_init(wheel, plain_text);
}

CONCMD_IF_PTR_TYPE ConPid_InitPidCal(WHEEL_TYPE wheel, BOOL impulse, FLOAT step, BOOL with_debug)
{
    return pid_cal_init(wheel, impulse, step, with_debug);
}

CONCMD_IF_PTR_TYPE ConPid_InitPidVal(WHEEL_TYPE wheel, DIR_TYPE direction,
            FLOAT min_percent,
            FLOAT max_percent,
            INT8 num_points)
{
    return pid_val_init(wheel, direction, min_percent, max_percent, num_points);
}


