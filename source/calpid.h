#ifndef CALPID_H
#define CALPID_H
    
#include <project.h>
#include "config.h"
#include "cal.h"
    
typedef struct cal_pid_params_tag
{
    char name[6];
    WHEEL_TYPE wheel;
} CAL_PID_PARAMS;


uint8 CalPid_Init(CAL_STAGE_TYPE stage, void *params);
uint8 CalPid_Start(CAL_STAGE_TYPE stage, void *params);
uint8 CalPid_Update(CAL_STAGE_TYPE stage, void *params);
uint8 CalPid_Stop(CAL_STAGE_TYPE stage, void *params);
uint8 CalPid_Results(CAL_STAGE_TYPE stage, void *params);

void CalibrateLeftPid(float *gains);
void CalibrateRightPid(float *gains);    
void ValidatePid();

#endif