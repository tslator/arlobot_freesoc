#ifndef CONMOTION_H
#define CONMOTION_H

#include "freesoc.h"
#include "concmd.h"


void ConMotion_Init(void);
void ConMotion_Start(void);
BOOL ConMotion_Assign(CONCMD_IF_TYPE *p_cmdif, ...);


#endif