#ifndef CALPID_H
#define CALPID_H
    
#include <project.h>
#include "config.h"
#include "cal.h"

typedef enum {PID_TYPE_LEFT, PID_TYPE_RIGHT} PID_TYPE;

CALIBRATION_TYPE *CalPid_LeftCalibration;
CALIBRATION_TYPE *CalPid_RightCalibration;
CALIBRATION_TYPE *CalPid_Validation;

void CalPid_Init();

#endif