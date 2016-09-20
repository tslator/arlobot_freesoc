#ifndef PIDUTIL_H
#define PIDUTIL_H
   
#include <project.h>
#include "pid_controller.h"
    
void DumpPid(char *name, PIDControl *pid, uint16 pwm);
    
#endif