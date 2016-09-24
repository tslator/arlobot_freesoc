#ifndef CALANG_H
#define CALANG_H
    
#include <project.h>    
#include "cal.h"    
    
CALIBRATION_TYPE *CalAng_Calibration;

void CalAng_Init();

void CalibrateAngularBias();
void ValidateAngularBias();
    
#endif