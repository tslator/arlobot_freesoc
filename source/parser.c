

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <string.h>
#include "parser.h"
#include "serial.h"


#define MAX_NUM_ELMS (10)
#define MAX_STR_LEN (MAX_LINE_LENGTH)

static const char SPACE_STR[2] = " ";

static char args_strs[MAX_NUM_ELMS][MAX_STR_LEN];
static char* args_ptrs[MAX_NUM_ELMS];
static char **ppargs;

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
   
   /* get the first token */
   token = strtok(str, SPACE_STR);
   
   /* walk through other tokens */
   while( token != NULL ) {
      strcpy(ppargs[index], token);
      index++;
      token = strtok(NULL, SPACE_STR);
   }
   
   return index;
}

static BOOL EnforceArgumentMutualExclusion(BOOL arg1, BOOL arg2)
{
    if ((arg1 && arg2) || (!arg1 && !arg2))
    {
        return FALSE;
    }

    return TRUE;
}

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
    
    if (strcmp(line, "exit") == 0)
    {
        cmd->is_exit = TRUE;
    }
    
    num_args = args_str_array(line);      
    cmd->args = docopt(num_args, ppargs, 1, "0.1.0", &success); 
    Ser_PutStringFormat("\r\nSuccess: %d\r\n", success);

    switch (success)
    {
        case -1:
            Ser_PutString((CHAR * const) cmd->args.usage_pattern);
            cmd->is_parsed = FALSE;
            return;
    
    
        case 1:
            if (cmd->args.help)
            {
                if (cmd->args.motor)
                {
                    Ser_PutString((CHAR * const) cmd->args.motor_help_message);
                }
                else if (cmd->args.pid)
                {
                    Ser_PutString((CHAR * const) cmd->args.pid_help_message);
                }
                else if (cmd->args.config)
                {
                    Ser_PutString((CHAR * const) cmd->args.config_help_message);
                }
                else if (cmd->args.motion)
                {
                    Ser_PutString((CHAR * const) cmd->args.motion_help_message);
                }
                else
                {
                    Ser_PutString((CHAR * const) cmd->args.help_message);
                }

                cmd->is_parsed = FALSE;
                return;
            }
            else
            {
                cmd->is_parsed = TRUE;                
            }            
    }

    
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

    if (cmd->args.pid && cmd->args.cal)
    {
        cmd->is_parsed = EnforceArgumentMutualExclusion(cmd->args.left, cmd->args.right);
    }

    if (cmd->args.pid && cmd->args.val)
    {
        cmd->is_parsed = EnforceArgumentMutualExclusion(cmd->args.forward, cmd->args.backward);
    }

    if (cmd->args.config && cmd->args.debug && !cmd->args.show)
    {
        cmd->is_parsed = EnforceArgumentMutualExclusion(cmd->args.enable, cmd->args.disable);
    }

    if (cmd->args.motion && cmd->args.val)
    {
        if (cmd->args.square)
        {
            cmd->is_parsed = EnforceArgumentMutualExclusion(cmd->args.left, cmd->args.right);
        }
        else if (cmd->args.circle)
        {
            cmd->is_parsed = EnforceArgumentMutualExclusion(cmd->args.cw, cmd->args.ccw);
        }
        
    }
    
    Ser_PutStringFormat("Command Is Parsed: %d\r\n", cmd->is_parsed);
    
}

