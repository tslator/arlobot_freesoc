#include <stdarg.h>
#include "conmotion.h"

    // console motion cal linear [--distance=<distance>]
    // console motion cal angular [--angle=<angle>]
    // console motion val linear [--distance=<distance>]
    // console motion val angular [--angle=<angle>]
    // console motion val square (left|right) [--side=<side>]
    // console motion val circle (cw|ccw) [--radius=<radius>]
    // console motion val out-and-back [--distance=<distance>]

static BOOL is_running;

/*------------------------------------------------------------------------------------------
    Motion Calibration Linear
*/
static void motion_cal_linear_init(FLOAT distance)
{
    is_running = TRUE;
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
static void motion_cal_angular_init(FLOAT angle)
{
    is_running = TRUE;
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
static void motion_cal_umbmark_init(void)
{
    is_running = TRUE;
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
static void motion_val_linear_init(FLOAT distance)
{
    is_running = TRUE;
}

static BOOL motion_val_linear_update(void)
{
    is_running = FALSE;
    return is_running;    
}

static BOOL motion_val_linear_status(void)
{
    return is_running;
}

static void motion_val_linear_results(void)
{

}

/*------------------------------------------------------------------------------------------
    Motion Validation Angular
*/
static void motion_val_angular_init(FLOAT angle)
{
    is_running = TRUE;
}

static BOOL motion_val_angular_update(void)
{
    is_running = FALSE;
    return is_running;
}

static BOOL motion_val_angular_status(void)
{
    return is_running;
}

static void motion_val_angular_results(void)
{

}

/*------------------------------------------------------------------------------------------
    Motion Validation Square
*/
static void motion_val_square_init(BOOL left_or_right, FLOAT side)
{
    is_running = TRUE;
}

static BOOL motion_val_square_update(void)
{
    is_running = FALSE;
    return is_running;
}

static BOOL motion_val_square_status(void)
{
    return is_running;
}

static void motion_val_square_results(void)
{

}

/*------------------------------------------------------------------------------------------
    Motion Validation Circle
*/
static void motion_val_circle_init(BOOL cw_or_ccw, FLOAT radius)
{
    is_running = TRUE;
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
static void motion_val_outandback_init(FLOAT distance)
{
    is_running = TRUE;
}

static BOOL motion_val_outandback_update(void)
{
    is_running = FALSE;
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
    is_running = FALSE;
}

void ConMotion_Start(void)
{

}

BOOL ConMotion_Assign(COMMAND_IF_TYPE *p_cmdif, ...)
{
        va_list valist;
    BOOL result = FALSE;
    int cmd;

    va_start(valist, p_cmdif);
    cmd = va_arg(valist, int);
    
    switch (cmd)
    {
        case MOTION_CAL_LINEAR:
        {
            FLOAT distance = va_arg(valist, double);

            motion_cal_linear_init(distance);
            p_cmdif->update = motion_cal_linear_update;
            p_cmdif->status = motion_cal_linear_status;
            p_cmdif->results = motion_cal_linear_results;

            p_cmdif->is_assigned = TRUE;
            result = TRUE;

            break;
        }

        case MOTION_CAL_ANGULAR:
        {
            FLOAT angle = va_arg(valist, double);

            motion_cal_angular_init(angle);
            p_cmdif->update = motion_cal_angular_update;
            p_cmdif->status = motion_cal_angular_status;
            p_cmdif->results = motion_cal_angular_results;

            p_cmdif->is_assigned = TRUE;
            result = TRUE;
            
            break;
        }

        case MOTION_CAL_UMB:
        {
            motion_cal_umbmark_init();
            p_cmdif->update = motion_cal_umbmark_update;
            p_cmdif->status = motion_cal_umbmark_status;
            p_cmdif->results = motion_cal_umbmark_results;

            p_cmdif->is_assigned = TRUE;
            result = TRUE;
            
            break;
        }

        case MOTION_VAL_LINEAR:
        {
            FLOAT distance = va_arg(valist, double);

            motion_val_linear_init(distance);
            p_cmdif->update = motion_val_linear_update;
            p_cmdif->status = motion_val_linear_status;
            p_cmdif->results = motion_val_linear_results;

            p_cmdif->is_assigned = TRUE;
            result = TRUE;

            break;
        }

        case MOTION_VAL_ANGULAR:
        {
            FLOAT angle = va_arg(valist, double);

            motion_val_angular_init(angle);
            p_cmdif->update = motion_val_angular_update;
            p_cmdif->status = motion_val_angular_status;
            p_cmdif->results = motion_val_angular_results;

            p_cmdif->is_assigned = TRUE;
            result = TRUE;
            
            break;
        }

        case MOTION_VAL_SQUARE:
        {
            BOOL left_or_right = va_arg(valist, int);
            FLOAT side = va_arg(valist, double);

            motion_val_square_init(left_or_right, side);
            p_cmdif->update = motion_val_square_update;
            p_cmdif->status = motion_val_square_status;
            p_cmdif->results = motion_val_square_results;

            p_cmdif->is_assigned = TRUE;
            result = TRUE;
            
            break;
        }

        case MOTION_VAL_CIRCLE:
        {
            BOOL cw_or_ccw = va_arg(valist, int);
            FLOAT radius = va_arg(valist, double);

            motion_val_circle_init(cw_or_ccw, radius);
            p_cmdif->update = motion_val_circle_update;
            p_cmdif->status = motion_val_circle_status;
            p_cmdif->results = motion_val_circle_results;

            p_cmdif->is_assigned = TRUE;
            result = TRUE;
            
            break;
        }

        case MOTION_VAL_OUTANDBACK:
        {
            FLOAT distance = va_arg(valist, double);

            motion_val_outandback_init(distance);
            p_cmdif->update = motion_val_outandback_update;
            p_cmdif->status = motion_val_outandback_status;
            p_cmdif->results = motion_val_outandback_results;

            p_cmdif->is_assigned = TRUE;
            result = TRUE;
            
            break;
        }
    };

    /* clean memory reserved for valist */
    va_end(valist);
    
    return result;
    
}
