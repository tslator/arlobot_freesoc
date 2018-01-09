/* 
MIT License

Copyright (c) 2017 Tim Slator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include "console.h"
#include "serial.h"
#include "conparser.h"
#include "parser.h"
#include "disp.h"
#include "control.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define MAX_ENABLE_COUNT (3)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static BOOL is_console_active;
static UINT8 console_enable_count;
static COMMAND_TYPE command;
static BOOL display_prompt;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static void ResetCommand(void)
{
    memset(&command, 0, sizeof command);
}

static void DisplayPrompt(void)
{
    Ser_WriteLine("", TRUE);
    Ser_WriteLine("arlobot> ", FALSE);    
}

static void DisplayUnknownCommand(void)
{
    Ser_WriteLine("", TRUE);            
    Ser_WriteLine("unknown command", TRUE);
}

static void DisplayInvalidCommandParameters(void)
{
    Ser_WriteLine("", TRUE);            
    Ser_WriteLine("invalid command parameters", TRUE);
}

static void DisplayWelcomeBanner(void)
{
    // Add something interesting here like random robot facts
}

static void DisplayGoodbyeBanner(void)
{
    Ser_WriteLine("", TRUE);            
    Ser_WriteLine("goodbye", TRUE);
    // Add something more substantial or witty here (maybe more random phrases about quitting)
}

static void HandleZeroLengthInput()
{
    if (is_console_active == FALSE)
    {
        console_enable_count++;
        if (console_enable_count >= MAX_ENABLE_COUNT)
        {
            console_enable_count = 0;
            is_console_active = TRUE; 
        }
    }
    
    /* Note: We want to act immediately on the console state change (so no else to the if above) */
    if (is_console_active == TRUE)
    {
        DisplayWelcomeBanner();
        DisplayPrompt();
    }        
}

static void HandleLineParsing(CHAR * const line)
{
    Parser_Parse(line, &command);
    if (command.is_exit)
    {
        is_console_active = FALSE;
        DisplayGoodbyeBanner();                    
    }                
    else if (command.is_parsed)
    {
        Disp_Dispatch(&command);
        
        if (!command.is_valid)
        {
            DisplayInvalidCommandParameters();
            DisplayPrompt();
        }
    }
    else
    {
        DisplayUnknownCommand();
        DisplayPrompt();
    }                
}

/*---------------------------------------------------------------------------------------------------
 * Module Interface
 *-------------------------------------------------------------------------------------------------*/
void Console_Init()
{   
    is_console_active = FALSE;
    display_prompt = FALSE;
    console_enable_count = 0;
    
    ResetCommand();
    Parser_Init();
    Disp_Init();
}

void Console_Start()
{
    Parser_Start();
    Disp_Start();
}

void Console_Update()
{
    INT8 length;
    CHAR line[MAX_LINE_LENGTH];
    BOOL result;

    /* Since all of the is_running interface routines are just returning is_running maybe there is no need for
       this function in the interface.  Maybe just dispatch has the routine for the purposes of tracking
       progress.  Or, maybe there is no need for Disp_IsRunning at all and Disp_Update can handle the 
       same functionality with different return values:
            -1 - nothing is active
            1 - still running
            0 - all done
    */
    if (Disp_IsRunning())
    {
        result = Disp_Update();
        if (result)
        {
            /* Only a single outstanding command is allowed - explicit return until dispatch is done */
            return;
        }
        else
        {
            Disp_Results();
            ResetCommand();
            DisplayPrompt();
        }        
    }
    
    length = Ser_ReadLine(line, TRUE, MAX_LINE_LENGTH);
    if (length == 0)
    {
        HandleZeroLengthInput();
    }
    else if (length > 0)
    {
        if (is_console_active)
        {
            HandleLineParsing(line);
        }
        else
        {
            /* Ignore all text typed when the console is inactive */
        }
    }
    else
    {
        /* Negative length means no input on serial */
    }
}