#ifndef PARSER_H
#define PARSER_H
    
#include "freesoc.h"
#include "disp.h"
    
void Parser_Init(void);
void Parser_Start(void);
void Parser_Parse(CHAR* const line, COMMAND_TYPE* const cmd);
    
#endif    