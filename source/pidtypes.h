#ifndef PIDTYPES_H
#define PIDTYPES_H

#include "pid_controller.h"
    
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef FLOAT (*GET_TARGET_FUNC_TYPE)();
typedef FLOAT (*GET_INPUT_FUNC_TYPE)();
typedef FLOAT (*PID_UPDATE_TYPE)(FLOAT target, FLOAT input);

typedef struct _pid_tag
{
    char name[8];
    PIDControl pid;
    int sign;
    GET_TARGET_FUNC_TYPE get_target;
    GET_INPUT_FUNC_TYPE get_input;
    PID_UPDATE_TYPE update;
} PID_TYPE;

typedef enum {PID_TYPE_LEFT, PID_TYPE_RIGHT, PID_TYPE_LINEAR, PID_TYPE_ANGULAR} PID_ENUM_TYPE;

#endif