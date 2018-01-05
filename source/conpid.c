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

typedef enum {PID_SHOW, PID_CAL, PID_VAL, PID_LAST} PID_CMD_TYPE;
    

static BOOL is_running;
static PID_SHOW_TYPE pid_show;
static PID_CAL_TYPE pid_cal;
static PID_VAL_TYPE pid_val;

/*----------------------------------------------------------------------------
    PID Show Routines
*/
static BOOL pid_show_init(PID_SHOW_TYPE *p_data)
{
    Ser_WriteLine("PID Show Init", TRUE);
    
    is_running = TRUE;
    return is_running;
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
static BOOL pid_cal_init(PID_CAL_TYPE *p_data)
{
    Ser_WriteLine("PID Calibration Init", TRUE);
    
    is_running = TRUE;
    return is_running;
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
static BOOL pid_val_init(PID_VAL_TYPE *p_data)
{
    Ser_WriteLine("PID Validation Init", TRUE);
    
    is_running = TRUE;
    return is_running;
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

CONCMD_IF_TYPE * const ConPid_InitPidShow(WHEEL_TYPE wheel, BOOL plain_text)
{
    return 0;
}

CONCMD_IF_TYPE * const ConPid_InitPidCal(WHEEL_TYPE wheel, BOOL impulse, FLOAT step, BOOL with_debug)
{
    return 0;
}

CONCMD_IF_TYPE * const ConPid_InitPidVal(WHEEL_TYPE wheel, DIR_TYPE direction,
            FLOAT min_percent,
            FLOAT max_percent,
            INT8 num_points)
{
    return 0;
}


