#include <stdarg.h>
#include "conpid.h"
#include "serial.h"

typedef struct _tag_pid_show
{
    WHEEL_TYPE wheel;
    BOOL plain_text;
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

static BOOL is_running;
static PID_SHOW_TYPE pid_show;
static PID_CAL_TYPE pid_cal;
static PID_VAL_TYPE pid_val;

/*----------------------------------------------------------------------------
    PID Show Routines
*/
static void pid_show_init(WHEEL_TYPE wheel, BOOL plain_text)
{
    is_running = TRUE;
    Ser_WriteLine("PID Show Init", TRUE);
    
}
static BOOL pid_show_update(void)
{
    is_running = FALSE;
    Ser_WriteLine("PID Show Update", TRUE);
    return is_running;
}
static BOOL pid_show_status(void)
{
    Ser_WriteLine("PID Show Status", TRUE);
    return is_running;
}
static void pid_show_results(void)
{
    Ser_WriteLine("PID Show Results", TRUE);
}

/*----------------------------------------------------------------------------
    PID Calibration Routines
*/
static void pid_cal_init(WHEEL_TYPE wheel, BOOL impulse, BOOL step, BOOL with_debug)
{
    is_running = TRUE;
    Ser_WriteLine("PID Calibration Init", TRUE);
    
}
static BOOL pid_cal_update(void)
{
    is_running = FALSE;
    Ser_WriteLine("PID Calibration Update", TRUE);
    return is_running;
}
static BOOL pid_cal_status(void)
{
    Ser_WriteLine("PID Calibration Status", TRUE);
    return is_running;
}
static void pid_cal_results(void)
{
    Ser_WriteLine("PID Calibration Results", TRUE);
}

/*----------------------------------------------------------------------------
    PID Validation Routines
*/
static void pid_val_init(WHEEL_TYPE wheel, DIR_TYPE direction, FLOAT min_percent, FLOAT max_percent, INT8 num_points)
{
    is_running = TRUE;
    Ser_WriteLine("PID Validation Init", TRUE);
    
}
static BOOL pid_val_update(void)
{
    is_running = FALSE;
    Ser_WriteLine("PID Validation Update", TRUE);
    return is_running;
}
static BOOL pid_val_status(void)
{
    Ser_WriteLine("PID Validation Status", TRUE);
    return is_running;
}
static void pid_val_results(void)
{
    Ser_WriteLine("PID Validation Results", TRUE);
}

/*----------------------------------------------------------------------------
    ConPid Module
*/

void ConPid_Init(void)
{
}

void ConPid_Start(void)
{

}

BOOL ConPid_Assign(CONCMD_IF_TYPE *p_cmdif, ...)
{
    va_list valist;
    BOOL result = FALSE;
    int cmd;

    va_start(valist, p_cmdif);
    cmd = va_arg(valist, int);
    
    switch (cmd)
    {
        case PID_SHOW:
        {
            INT8 wheel = va_arg(valist, int);
            BOOL plain_text = va_arg(valist, int);
            
            pid_show_init(wheel, plain_text);
            p_cmdif->update = pid_show_update;
            p_cmdif->status = pid_show_status;
            p_cmdif->results = pid_show_results;
            
            p_cmdif->is_assigned = TRUE;
            result = TRUE;
            break;
        }
        
        case PID_CAL:
        {
            INT8 int_wheel = va_arg(valist, int);
            BOOL impulse = va_arg(valist, int);
            BOOL step = va_arg(valist, int);
            BOOL with_debug = va_arg(valist, int);
            
            WHEEL_TYPE wheel = (WHEEL_TYPE) int_wheel;
            
            pid_cal_init(wheel, impulse, step, with_debug);
            p_cmdif->update = pid_cal_update;
            p_cmdif->status = pid_cal_status;
            p_cmdif->results = pid_cal_results;
            
            p_cmdif->is_assigned = TRUE;
            result = TRUE;
            
            break;
        }

        case PID_VAL:
        {
            INT8 int_wheel = va_arg(valist, int);            
            INT8 int_direction = va_arg(valist, int);
            FLOAT min_percent = va_arg(valist, double);
            FLOAT max_percent = va_arg(valist, double);
            INT8 num_points = va_arg(valist, int);

            WHEEL_TYPE wheel = (WHEEL_TYPE) int_wheel;
            DIR_TYPE direction = (DIR_TYPE) int_direction;
            
            pid_val_init(wheel, direction, min_percent, max_percent, num_points);
            p_cmdif->update = pid_val_update;
            p_cmdif->status = pid_val_status;
            p_cmdif->results = pid_val_results;

            p_cmdif->is_assigned = TRUE;
            result = TRUE;

            break;
        }
    };

    /* clean memory reserved for valist */
    va_end(valist);
    
    return result;
}
