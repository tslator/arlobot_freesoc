#ifndef CONCMD_H
#define CONCMD_H

#include <math.h>
#include "freesoc.h"
#include "types.h"
    
typedef struct _command_if
{
    BOOL is_assigned;
    BOOL (*update)();
    BOOL (*status)();
    void (*results)();
} COMMAND_IF_TYPE;

#define GET_WHEEL(left, right) ( (left && right) || (!left && !right) ? WHEEL_BOTH : (left ? WHEEL_LEFT : (right ? WHEEL_RIGHT : -1)))

#define STR_TO_FLOAT(value)  (strlen(value) > 0 ? atof(value) : NAN)
#define STR_TO_INT(value)    (strlen(value) > 0 ? atoi(value) : -127)
#define IS_SPECIFIED(value)  (!isnan(value))
#define in_range(value, lower, upper) value >= lower ? (value <= upper ? TRUE : FALSE) : FALSE


typedef enum {MOTOR_REPEAT, MOTOR_CAL, MOTOR_VAL, MOTOR_MOVE, MOTOR_SHOW} MOTOR_CMD_TYPE;
typedef enum {PID_SHOW, PID_CAL, PID_VAL} PID_CMD_TYPE;
typedef enum {CONFIG_DEBUG, CONFIG_CLEAR, CONFIG_SHOW} CONFIG_CMD_TYPE;
typedef enum {MOTION_CAL_LINEAR, MOTION_CAL_ANGULAR, MOTION_CAL_UMB, MOTION_VAL_LINEAR, MOTION_VAL_ANGULAR, MOTION_VAL_CIRCLE,
    MOTION_VAL_SQUARE, MOTION_VAL_OUTANDBACK} MOTION_CMD_TYPE;

    
#endif