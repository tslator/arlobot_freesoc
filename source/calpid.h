#ifndef CALPID_H
#define CALPID_H
    
#include <project.h>
    
void CalibrateLeftPid(float *gains);
void CalibrateRightPid(float *gains);    
void ValidatePid();

#endif