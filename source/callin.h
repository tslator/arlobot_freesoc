#ifndef CALIN_H
#define CALIN_H
    
#include <project.h>    
#include "cal.h"
    
CALIBRATION_TYPE *CalLin_Calibration;

void CalLin_Init();    
    
void CalibrateLinearBias();
void ValidateLinearBias();

#endif    