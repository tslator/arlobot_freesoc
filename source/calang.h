#ifndef CALANG_H
#define CALANG_H
    
#include <project.h>    
#include "cal.h"    
    
uint8 CalAng_Init(CAL_STAGE_TYPE stage);
uint8 CalAng_Start(CAL_STAGE_TYPE stage);
uint8 CalAng_Update(CAL_STAGE_TYPE stage);
uint8 CalAng_Stop(CAL_STAGE_TYPE stage);
uint8 CalAng_Results(CAL_STAGE_TYPE stage);
    
void CalibrateAngularBias();
void ValidateAngularBias();
    
#endif