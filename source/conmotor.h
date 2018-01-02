#ifndef CONMOTOR_H
#define CONMOTOR_H
    
#include "freesoc.h"
#include "concmd.h"

void ConMotor_Init(void);
void ConMotor_Start(void);
BOOL ConMotor_Assign(CONCMD_IF_TYPE *p_cmdif, ...);    
    
#endif
    