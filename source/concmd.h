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
   Description: This module provides types, macros and constants used for process console commands.
 *-------------------------------------------------------------------------------------------------*/    

#ifndef CONCMD_H
#define CONCMD_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <math.h>
#include "freesoc.h"
#include "limits.h"
    
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef BOOL (*CONCMD_UPDATE_FUNC_PTR)(void);
typedef BOOL (*CONCMD_STATUS_FUNC_PTR)(void);
typedef void (*CONCMD_RESULTS_FUNC_PTR)(void);

typedef struct _command_if
{
    CONCMD_UPDATE_FUNC_PTR update;
    CONCMD_STATUS_FUNC_PTR status;
    CONCMD_RESULTS_FUNC_PTR results;
} CONCMD_IF_TYPE;

typedef CONCMD_IF_TYPE * CONCMD_IF_PTR_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define NULL_UPDATE ((CONCMD_UPDATE_FUNC_PTR) 0)
#define NULL_STATUS ((CONCMD_STATUS_FUNC_PTR) 0)
#define NULL_RESULTS ((CONCMD_RESULTS_FUNC_PTR) 0)

#define NULL_CONCMD_INTERFACE ((CONCMD_IF_PTR_TYPE) 0)

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/
#define GET_WHEEL(left, right) ( (left && right) || (!left && !right) ? WHEEL_BOTH : (left ? WHEEL_LEFT : (right ? WHEEL_RIGHT : -1)))
#define GET_DIRECTON(forward, backward) ((forward && backward) || (!forward && !backward) ? DIR_BOTH : (forward ? DIR_FORWARD : (backward ? DIR_BACKWARD : -1 )))

#define IS_SPECIFIED(value) (value != 0 ? TRUE : FALSE)
#define STR_TO_FLOAT(value)  (value && strlen(value) > 0 ? strtof(value, NULL) : NAN)
#define STR_TO_INT(value)    (value && strlen(value) > 0 ? strtoul(value, NULL, 0) : INT_MIN)
#define IS_VALID_FLOAT(value)  (!isnan(value))
#define IS_VALID_INT(value) (value != INT_MIN)
    
#endif