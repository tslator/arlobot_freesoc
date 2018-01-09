#ifndef CONCMD_H
#define CONCMD_H

#include <math.h>
#include "freesoc.h"
#include "types.h"
#include "limits.h"
    
typedef struct _command_if
{
    BOOL (* update)();
    BOOL (* status)();
    void (* results)();
} CONCMD_IF_TYPE;

typedef CONCMD_IF_TYPE * const CONCMD_IF_PTR_TYPE;

#define GET_WHEEL(left, right) ( (left && right) || (!left && !right) ? WHEEL_BOTH : (left ? WHEEL_LEFT : (right ? WHEEL_RIGHT : -1)))
#define GET_DIRECTON(forward, backward) ((forward && backward) || (!forward && !backward) ? DIR_BOTH : (forward ? DIR_FORWARD : (backward ? DIR_BACKWARD : -1 )))

#define IS_SPECIFIED(value) (value != 0 ? TRUE : FALSE)
#define STR_TO_FLOAT(value)  (value && strlen(value) > 0 ? atof(value) : NAN)
#define STR_TO_INT(value)    (value && strlen(value) > 0 ? atoi(value) : INT_MIN)
#define IS_VALID_FLOAT(value)  (!isnan(value))
#define IS_VALID_INT(value) (value != INT_MIN)
    
#endif