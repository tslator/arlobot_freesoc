#ifndef CALPID_H
#define CALPID_H
    
#include <project.h>
#include "config.h"
#include "cal.h"

CALIBRATION_TYPE *CalPid_LeftCalibration;
CALIBRATION_TYPE *CalPid_RightCalibration;
CALIBRATION_TYPE *CalPid_LinearCalibration;
CALIBRATION_TYPE *CalPid_AngularCalibration;

void CalPid_Init();

#endif