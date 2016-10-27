#ifndef CALANG_H
#define CALANG_H
    
#include <project.h>    
#include "cal.h"    
    
typedef struct _angular_params
{
    DIR_TYPE direction;
    uint32   run_time;
    float    distance;
    float    mps;
    float    rps;
} CAL_ANG_PARAMS;
    
CALIBRATION_TYPE *CalAng_Calibration;
CALIBRATION_TYPE *CalAng_Validation;

void CalAng_Init();

void CalibrateAngularBias();
void ValidateAngularBias();
    
#endif