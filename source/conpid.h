#ifndef CONPID_H
#define CONPID_H
    
#include "freesoc.h"
#include "concmd.h"
    
void ConPid_Init(void);
void ConPid_Start(void);

CONCMD_IF_TYPE * const ConPid_InitPidShow(WHEEL_TYPE wheel, 
                                          BOOL plain_text);
CONCMD_IF_TYPE * const ConPid_InitPidCal(WHEEL_TYPE wheel, 
                                         BOOL impulse, 
                                         FLOAT step, 
                                         BOOL with_debug);
CONCMD_IF_TYPE * const ConPid_InitPidVal(WHEEL_TYPE wheel, DIR_TYPE direction,
                                         FLOAT min_percent,
                                         FLOAT max_percent,
                                         INT8 num_points);
    
#endif