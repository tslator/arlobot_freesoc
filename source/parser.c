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
#include <string.h>
#include "parser.h"
#include "conserial.h"
#include "conparser.h"
#include "utils.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;
#define MAX_NUM_ELMS (10)
#define MAX_STR_LEN (MAX_LINE_LENGTH)

static const CHAR SPACE_STR[2] = " ";

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static CHAR args_strs[MAX_NUM_ELMS][MAX_STR_LEN];
static CHAR* args_ptrs[MAX_NUM_ELMS];
static CHAR **ppargs;
static CHAR tmp_str_token[MAX_STRING_LENGTH];

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
static void init_args_storage()
{
    int ii;
    ppargs = &args_ptrs[0];
    
    for (ii = 0; ii < MAX_NUM_ELMS; ++ii)
    {
        ppargs[ii] = args_strs[ii];
    }
}

static int args_str_array(CHAR * const str)
{
   CHAR *token;
   int index = 0;

   /* Copy str to tmp buffer to discard const qualifier for strtok */
   strcpy(tmp_str_token, str);

   /* get the first token */
   token = strtok(tmp_str_token, SPACE_STR);
   
   /* walk through other tokens */
   while( token != NULL ) {
      strcpy(ppargs[index], token);
      index++;
      token = strtok(NULL, SPACE_STR);
   }
   
   return index;
}

static BOOL IsMutuallyExclusive(UINT16 mask)
{
    return is_power_of_two(mask);
}

static BOOL IsRequired(UINT16 mask)
{
    return is_set(mask);
}

static BOOL IsRequiredMutuallyExclusive(UINT16 mask)
{
    return IsRequired(mask) && IsMutuallyExclusive(mask);
}

static void DisplayHelpMessage(COMMAND_TYPE* const cmd)
{
    ConSer_WriteLine(TRUE, "");
    
    if (cmd->args.motor)
    {
        ConSer_WriteLine(TRUE, (CHAR * const) cmd->args.motor_help_message);
    }
    else if (cmd->args.pid)
    {
        ConSer_WriteLine(TRUE, (CHAR * const) cmd->args.pid_help_message);
    }
    else if (cmd->args.config)
    {
        ConSer_WriteLine(TRUE, (CHAR * const) cmd->args.config_help_message);
    }
    else if (cmd->args.motion)
    {
        ConSer_WriteLine(TRUE, (CHAR * const) cmd->args.motion_help_message);
    }
    else
    {
        ConSer_WriteLine(TRUE, (CHAR * const) cmd->args.help_message);
    }
}

static void HandleArgumentMutualExclusion(COMMAND_TYPE* const cmd)
{
    /* Note: There are things that the C parser does not enforce that the Python parser does.  This is a just consequence
       of the implementation.  In particular, the C parser does not enforce required syntax, e.g. (blah1|blah2).  I
       had initially added checking in the ConXXX modules, but I think it would be better here, because we can then
       invalidate the parsing and provide a more helpful message.
    */
    
    /* The parser does not enforce required arguments.  Since the parser does not enforce this, we must do it here.
       In this case, 
            1. either left or right (but not both) must be specified and 
            2. either impulse or step (step has a default so it is always specified) must be specified
            3. with_debug is optional
    */
    
    if (cmd->args.pid)
    {
        if (cmd->args.val)
        {
            UINT16 mask = 0;

            mask |= cmd->args.forward ? 0x0001 : 0;
            mask |= cmd->args.backward ? 0x0002 : 0;
            cmd->is_parsed = IsRequiredMutuallyExclusive(mask);
        }
        else if (cmd->args.cal)
        {
            UINT16 mask = 0;

            mask |= cmd->args.set ? 0x0001 : 0;
            mask |= cmd->args.clear ? 0x0002 : 0;
            mask |= cmd->args.left ? 0x0004 : 0;
            mask |= cmd->args.right ? 0x0008 : 0;
            cmd->is_parsed = IsRequiredMutuallyExclusive(mask);
        }
    }

    if (cmd->args.motor)
    {
        if (cmd->args.cal)
        {
            cmd->is_parsed = TRUE;
        }
        else if (cmd->args.val)
        {
            UINT16 mask = 0;

            mask |= cmd->args.forward ? 0x0001 : 0;
            mask |= cmd->args.backward ? 0x0002 : 0;
            cmd->is_parsed = IsRequiredMutuallyExclusive(mask);
        }        
    }

    if (cmd->args.config)
    {
        if (cmd->args.show)
        {
            UINT16 mask = 0;
            
            mask |= cmd->args.motor ? 0x0001 : 0;
            mask |= cmd->args.pid ? 0x0002 : 0;
            mask |= cmd->args.bias ? 0x0004 : 0;
            mask |= cmd->args.debug ? 0x0008 : 0;
            mask |= cmd->args.status ? 0x0010 : 0;
            mask |= cmd->args.params ? 0x0020 : 0;
            
            cmd->is_parsed = IsRequiredMutuallyExclusive(mask);
        }
        else if (cmd->args.clear)
        {
            UINT16 mask = 0;

            mask |= cmd->args.motor ? 0x0001 : 0;
            mask |= cmd->args.pid ? 0x0002 : 0;
            mask |= cmd->args.bias ? 0x0004 : 0;
            mask |= cmd->args.debug ? 0x0008 : 0;
            mask |= cmd->args.all ? 0x0010 : 0;
            cmd->is_parsed = IsRequiredMutuallyExclusive(mask);
        }
        else if  (cmd->args.debug)
        {
            UINT16 mask = 0;

            mask |= cmd->args.enable ? 0x0001 : 0;
            mask |= cmd->args.disable ? 0x0002 : 0;
            BOOL enable_disable = IsRequiredMutuallyExclusive(mask);
            
            mask = 0;
            mask |= cmd->args.lmotor ? 0x0001 : 0;
            mask |= cmd->args.rmotor ? 0x0002 : 0;
            mask |= cmd->args.lenc ? 0x0004 : 0;
            mask |= cmd->args.renc ? 0x0008 : 0;
            mask |= cmd->args.lpid ? 0x0010 : 0;
            mask |= cmd->args.rpid ? 0x0020 : 0;
            mask |= cmd->args.odom ? 0x0040 : 0;
            mask |= cmd->args.all ? 0x0080 : 0;
            
            BOOL flags = IsRequiredMutuallyExclusive(mask);           
            
            cmd->is_parsed = enable_disable && (flags || cmd->args.mask);
        }
    }

    if (cmd->args.motion && cmd->args.val)
    {
        if (cmd->args.square)
        {
            UINT16 mask = 0;

            mask |= cmd->args.left ? 0x0001 : 0;
            mask |= cmd->args.right ? 0x0002 : 0;
            cmd->is_parsed = IsRequiredMutuallyExclusive(mask);
        }
        else if (cmd->args.circle)
        {
            UINT16 mask = 0;

            mask |= cmd->args.cw ? 0x0001 : 0;
            mask |= cmd->args.ccw ? 0x0002 : 0;
            cmd->is_parsed = IsRequiredMutuallyExclusive(mask);
        }
        
    }
}

/*---------------------------------------------------------------------------------------------------
 * Module Interface
 *-------------------------------------------------------------------------------------------------*/    
void Parser_Init(void)
{
    memset(args_strs, 0, sizeof args_strs);
    memset(args_ptrs, 0, sizeof args_ptrs);
    ppargs = 0;
}

void Parser_Start(void)
{
    init_args_storage();
}

void Parser_Parse(CHAR* const line, COMMAND_TYPE* const cmd)
{
    int success;
    int num_args;
    cmd->is_exit = FALSE;
    cmd->is_parsed = FALSE;
    cmd->is_valid = FALSE;
    
    
    if (strcmp(line, "exit") == 0)
    {
        cmd->is_exit = TRUE;
        return;
    }
    
    num_args = args_str_array(line);      
    
    cmd->args = docopt(num_args, ppargs, 1, "0.1.0", &success);
    
    switch (success)
    {
        case -1:
            ConSer_WriteLine(TRUE, "");
            ConSer_WriteLine(TRUE, (CHAR * const) cmd->args.usage_pattern);
            return;
    
        case 1:
            if (cmd->args.help)
            {
                DisplayHelpMessage(cmd);
                cmd->is_parsed = cmd->args.help ? TRUE : FALSE;
                return;
            }
            break;

        default:            
            break;
    }

    HandleArgumentMutualExclusion(cmd); 
    if (!cmd->is_parsed)
    {
        ConSer_WriteLine(TRUE, "");
        ConSer_WriteLine(TRUE, (CHAR * const) cmd->args.usage_pattern);
    }
}

