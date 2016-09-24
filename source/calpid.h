#ifndef CALPID_H
#define CALPID_H
    
#include <project.h>
#include "config.h"
#include "cal.h"

typedef enum {PID_TYPE_LEFT, PID_TYPE_RIGHT} PID_TYPE;

typedef struct cal_pid_params_tag
{
    char name[6];
    PID_TYPE pid_type;
    DIR_TYPE direction;
    uint32 run_time;
} CAL_PID_PARAMS;

CALIBRATION_TYPE *CalPid_LeftCalibration;
CALIBRATION_TYPE *CalPid_RightCalibration;
CALIBRATION_TYPE *CalPid_LeftValidation;
CALIBRATION_TYPE *CalPid_RightValidation;

void CalPid_Init();

#endif