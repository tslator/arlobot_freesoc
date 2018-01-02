#ifndef CONPID_H
#define CONPID_H
    
#include "freesoc.h"
#include "concmd.h"
    
    
void ConPid_Init(void);
void ConPid_Start(void);
BOOL ConPid_Assign(COMMAND_IF_TYPE *p_cmdif, ...);
    
#endif