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
#include "concmdif.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static CONCMD_IF_TYPE concmd_interface;
static BOOL is_active;

/*---------------------------------------------------------------------------------------------------
 * Module Interface
 *-------------------------------------------------------------------------------------------------*/
void ConCmdIf_Init(void)
{
    is_active = FALSE;
}

void ConCmdIf_Start(void)
{
    concmd_interface.update = NULL_UPDATE;
    concmd_interface.status = NULL_STATUS;
    concmd_interface.results = NULL_RESULTS;
}

void ConCmdIf_SetIfaceFuncs(CONCMD_UPDATE_FUNC_PTR update, CONCMD_STATUS_FUNC_PTR status, CONCMD_RESULTS_FUNC_PTR results)
{
    concmd_interface.update = update;
    concmd_interface.status = status;
    concmd_interface.results = results;
    is_active = TRUE;
}

BOOL ConCmdIf_GetIfacePtr(CONCMD_IF_PTR_TYPE *interface)
{
    *interface = (CONCMD_IF_PTR_TYPE) &concmd_interface;
    
    return is_active;
}

void ConCmdIf_ReleaseIfacePtr(void)
{
    is_active = FALSE;
}