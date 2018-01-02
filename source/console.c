#include "console.h"
#include "serial.h"
#include "conparser.h"
#include "parser.h"
#include "dispatch.h"
#include "control.h"


static BOOL is_console_active;
static UINT8 console_enable_count;
static COMMAND_TYPE command;

static void ResetCommand(void)
{
    memset(&command, 0, sizeof command);
}

void Console_Init()
{    
    console_enable_count = 0;
    
    ResetCommand();
    Parser_Init();
    Dispatch_Init();
}

void Console_Start()
{
    Parser_Start();
    Dispatch_Start();
}

void Console_Update()
{
    INT8 length;
    CHAR line[MAX_LINE_LENGTH];
    BOOL result;

    if (Dispatch_IsRunning())
    {
        result = Dispatch_Update();
        if (result)
        {
            return;
        }
        else
        {
            Dispatch_Results();
            ResetCommand();
            Ser_WriteLine("arlobot> ", FALSE);
        }        
    }
    
    
    length = Ser_ReadLine(line, TRUE, MAX_LINE_LENGTH);
    if (length == 0)
    {
        if (is_console_active == FALSE)
        {
            console_enable_count++;
            if (console_enable_count >= 3)
            {
                console_enable_count = 0;
                is_console_active = TRUE;                
            }
        }
        
        if (is_console_active == TRUE)
        {
            Ser_WriteLine("", TRUE);
            Ser_WriteLine("arlobot> ", FALSE);
        }        
    }
    else if (length > 0)
    {
        if (is_console_active)
        {
            Ser_WriteLine("", TRUE);
            
            if (strcmp(line, "exit") == 0)
            {
                is_console_active = FALSE;                
                Ser_WriteLine("goodbye", TRUE);
            }
            else
            {
                Parser_Parse(line, &command);
                if (command.is_parsed)
                {
                    Disp_Dispatch(&command);
                    
                    if (!command.is_valid)
                    {
                        Ser_WriteLine("invalid command parameters", TRUE);
                        Ser_WriteLine("arlobot> ", FALSE);
                    }
                }
                else
                {
                    Ser_WriteLine("unknown command", TRUE);
                    Ser_WriteLine("arlobot> ", FALSE);
                }                
            }
        }
    }    
}