#ifndef DISPATCH_H
#define DISPATCH_H
    
#include "freesoc.h"
#include "conparser.h"
    
typedef struct _tag_command
{
    BOOL is_valid;
    BOOL is_parsed;
    DocoptArgs args;
} COMMAND_TYPE;
    
    
void Dispatch_Init(void);
void Dispatch_Start(void);
BOOL Dispatch_IsRunning(void);
void Dispatch_Results();
BOOL Dispatch_Update();
void Disp_Dispatch(COMMAND_TYPE* const command);
    
#endif