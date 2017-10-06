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
   Description: This module provides the implementation for calibrating the PID.
 *-------------------------------------------------------------------------------------------------*/    

#ifndef CALPID_H
#define CALPID_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <project.h>
#include "config.h"
#include "cal.h"

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef enum {PID_TYPE_LEFT, PID_TYPE_RIGHT} PID_ENUM_TYPE;

typedef struct cal_pid_params_tag
{
    char name[6];
    PID_ENUM_TYPE pid_type;
    DIR_TYPE direction;
    uint32 run_time;
} CAL_PID_PARAMS;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
CALIBRATION_TYPE *CalPid_LeftCalibration;
CALIBRATION_TYPE *CalPid_RightCalibration;
CALIBRATION_TYPE *CalPid_LeftValidation;
CALIBRATION_TYPE *CalPid_RightValidation;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void CalPid_Init();
void CalPid_Start();

#endif