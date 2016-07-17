#ifndef CALCPS2PWM_H
#define CALCPS2PWM_H
    
#include <project.h>
#include "cal.h"

uint8 CalMotor_Init(CAL_STAGE_TYPE stage, void *params);
uint8 CalMotor_Start(CAL_STAGE_TYPE stage, void *params);
uint8 CalMotor_Update(CAL_STAGE_TYPE stage, void *params);
uint8 CalMotor_Stop(CAL_STAGE_TYPE stage, void *params);
uint8 CalMotor_Results(CAL_STAGE_TYPE stage, void *params);
    
void CalibrateMotorVelocity();    
void ValidateMotorVelocity();
void PrintMotorVelocity();

#endif    