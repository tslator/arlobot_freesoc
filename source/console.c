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
#include "conserial.h"
#include "conparser.h"
#include "parser.h"
#include "disp.h"
#include "control.h"
#include "utils.h"
#include "consts.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;
#define MAX_ENABLE_COUNT (3)

static char const robot_facts[15][270] = {
    "The word 'robot' comes from a Czech word 'robota',\r\nwhich means 'drudgery'.",
    "The first known robot in recent history was created\r\nin the 5th century BC by Archytas of Tarentum. He\r\ncreated the mechanical doves.",
    "In 1495 Leonardo da Vinci drew plans for a type of\r\nrobotic machine. It wasan armored humanoid. A\r\nfunctional, but miniature version has been created\r\nby Mark Rosheim, an engineer, to colonize Mars for NASA.",
    "Elektro was the first humanoid robot. It was built\r\nin 1939 by Westinghouse. It was seven feet tall and\r\nhad a vocabulary of 700 words.",
    "A robot killed a factory worker in 1981. A robotic\r\nfactory arm crushed an\r\nemployee at the Kawasaki plant.",
    "It is estimated that more than one million industrial\r\nrobots are in operation\r\ntoday, with the majority being\r\nin Japan.",
    "There are two robots on Mars - Spirit and Opportunity.\r\nThey were built to last 90 days on Mars but they lasted\r\nfor several years. Curiosity is the latest rover to land\r\non Mars to explore the planet.",
    "The United States military has 4,000 robots that are\r\nused for scouting in dangerous countries like Iraq and\r\nAfghanistan where roadside bombs are common. The Taliban\r\nfighters have discovered that they can disable these\t\nrobots by flipping ladders on top of them.",
    "A robot vacuum cleaner was invented that is capable\r\nof sensing its environment and cleaning the floor\r\nwithout human help to move the device.",
    "Star Wars the movie franchise made robots even more\r\npopular in the imagination of people with characters\r\nsuch as R2-D2 and C-3PO.",
    "Work is underway to develop a microrobot that will\r\nbe capable of performing a biopsy of a patient from the\r\ninside of that patient. It is being built to swim similar\r\nto the way E. coli bacteria moves.",
    "Some people fear that robots will eventually take\r\nover the world because they will become their own species.",
    "A robot has been created that uses bacteria-filled\r\n fuel cells. The robot is capable of producing electricity\r\nfrom dead flies to power its battery. It can also use\r\nrotten apples to create fuel.",
    "The smallest robot is called a nanobot. It is less\r\nthan one-thousandth of a millimeter.",
    "Japan's NES System Technologies recently built a\r\nwinebot, capable of identifying many different types\r\nof wines and cheeses. It made a mistake at one point\r\nand named a reporter's hand as prosciutto."
};

static char const quitting[5][60] = {
    "Pain is temporary. Quitting lasts forever.",
    "If you quit ONCE it becomes a habit.Never quit!!!",
    "It's always to soon to quit!",
    "Things can always get worse, but only quitters quit!",
    "Age wrinkles the body. Quitting wrinkles the soul."
};

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static BOOL is_console_active;
static UINT8 console_enable_count;
static COMMAND_TYPE command;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static void ResetCommand(void)
{
    memset(&command, 0, sizeof command);
}

static void DisplayPrompt(void)
{
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(FALSE, "arlobot> ");    
}

static void DisplayUnknownCommand(void)
{
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "unknown command");
}

static void DisplayInvalidCommandParameters(void)
{
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "invalid command parameters");
}

static void DisplayWelcomeBanner(void)
{
    UINT8 index = (UINT8) GetRandomValue(0, 15);
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "------------------- Robot Facts -------------------");
    ConSer_WriteLine(TRUE, robot_facts[index]);
    ConSer_WriteLine(TRUE, "---------------------------------------------------");
}

static void DisplayGoodbyeBanner(void)
{
    UINT8 index = (UINT8) GetRandomValue(0, 5);
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(FALSE, quitting[index]);
    ConSer_WriteLine(TRUE, "  Goodbye");
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
            DisplayWelcomeBanner();
        }
    }
    
    /* Note: We want to act immediately on the console state change (so no else to the if above) */
    if (is_console_active == TRUE)
    {
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
        if (command.args.help)
        {
            DisplayPrompt();
            return;
        }
        
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
    
    length = ConSer_ReadLine(line, TRUE, MAX_LINE_LENGTH);
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