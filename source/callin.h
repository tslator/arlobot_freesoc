#ifndef CALIN_H
#define CALIN_H
    
#include <project.h>    
#include "cal.h"
    
typedef struct _linear_params
{
    DIR_TYPE direction;
    uint32   run_time;
    float    distance;
    float    mps;
} CAL_LIN_PARAMS;

    
CALIBRATION_TYPE *CalLin_Validation;

void CalLin_Init();    
    
void CalibrateLinearBias();
void ValidateLinearBias();

#endif    