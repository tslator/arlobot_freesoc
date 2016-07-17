#ifndef CALIN_H
#define CALIN_H
    
#include <project.h>    
#include "cal.h"
    
uint8 CalLin_Init(CAL_STAGE_TYPE stage);
uint8 CalLin_Start(CAL_STAGE_TYPE stage);
uint8 CalLin_Update(CAL_STAGE_TYPE stage);
uint8 CalLin_Stop(CAL_STAGE_TYPE stage);
uint8 CalLin_Results(CAL_STAGE_TYPE stage);
    
void CalibrateLinearBias();
void ValidateLinearBias();

#endif    