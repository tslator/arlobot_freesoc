#include <stdarg.h>
#include "conmotion.h"

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
} MOTION_VAL_ANGULAR_TYPE;

typedef struct _tag_motion_val_linear
{
    FLOAT distance;
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


/*------------------------------------------------------------------------------------------
    Motion Calibration Linear
*/
static CONCMD_IF_TYPE * const motion_cal_linear_init(FLOAT distance)
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
static CONCMD_IF_TYPE * const motion_cal_angular_init(FLOAT angle)
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
static CONCMD_IF_TYPE * const motion_cal_umbmark_init(void)
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
static CONCMD_IF_TYPE * const motion_val_linear_init(FLOAT distance)
{
    motion_val_linear.distance = distance;
    is_running = TRUE;

    return &cmd_if_array[MOTION_VAL_LINEAR];
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
static CONCMD_IF_TYPE * const motion_val_angular_init(FLOAT angle)
{
    motion_val_angular.angle = angle;
    is_running = TRUE;

    return &cmd_if_array[MOTION_VAL_ANGULAR];
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
static CONCMD_IF_TYPE * const  motion_val_square_init(BOOL left, FLOAT side)
{
    motion_val_square.left = left;
    motion_val_square.side = side;

    is_running = TRUE;

    return &cmd_if_array[MOTION_VAL_SQUARE];
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
static CONCMD_IF_TYPE * const motion_val_circle_init(BOOL cw, FLOAT radius)
{
    motion_val_circle.cw = cw;
    motion_val_circle.radius = radius;
    
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
static CONCMD_IF_TYPE * const  motion_val_outandback_init(FLOAT distance)
{
    motion_val_outandback.distance = distance;
    is_running = TRUE;

    return &cmd_if_array[MOTION_VAL_OUTANDBACK];
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

CONCMD_IF_TYPE * const ConMotion_InitCalLinear(FLOAT distance)
{
    return motion_cal_linear_init(distance);
}
CONCMD_IF_TYPE * const ConMotion_InitMotionCalAngular(FLOAT angle)
{
    return motion_cal_angular_init(angle);
}
CONCMD_IF_TYPE * const ConMotion_InitMotionCalUmbmark(void)
{
    return motion_cal_umbmark_init();
}
CONCMD_IF_TYPE * const ConMotion_InitMotionValLinear(FLOAT distance)
{
    return motion_val_linear_init(distance);
}
CONCMD_IF_TYPE * const ConMotion_InitMotionValAngular(FLOAT angle)
{
    return motion_val_angular_init(angle);
}
CONCMD_IF_TYPE * const ConMotion_InitMotionValSquare(BOOL left, FLOAT side)
{
    return motion_val_square_init(left, side);
}
CONCMD_IF_TYPE * const ConMotion_InitMotionValCircle(BOOL cw, FLOAT radius)
{
    return motion_val_circle_init(cw, radius);
}
CONCMD_IF_TYPE * const ConMotion_InitMotionValOutAndBack(FLOAT distance)
{
    return motion_val_outandback_init(distance);
}

void ConMotion_Start(void)
{

}

