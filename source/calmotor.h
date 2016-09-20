#ifndef CALCPS2PWM_H
#define CALCPS2PWM_H
    
#include <project.h>
#include "cal.h"

CAL_DATA_TYPE * WHEEL_DIR_TO_CAL_DATA[2][2];
    
CALIBRATION_TYPE *CalMotor_Calibration;
CALIBRATION_TYPE *CalMotor_Validation;
    
void CalMotor_Init();    
    
#endif    