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

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "valpid.h"
#include "cal.h"
#include "conserial.h"
#include "nvstore.h"
#include "pid.h"
#include "pidleft.h"
#include "pidright.h"
#include "utils.h"
#include "encoder.h"
#include "motor.h"
#include "odom.h"
#include "control.h"
#include "time.h"
#include "debug.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;
#define VAL_NUM_PROFILE_DATA_POINTS (13)
#define MAX_RUN_TIME (3000)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef struct _tag_pid_validation
{
    PID_ENUM_TYPE pid_type;
    DIR_TYPE direction;
} PID_VALIDATION_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static UINT32 start_time;

static PID_VALIDATION_TYPE pid_val[2];

static FLOAT profile[VAL_NUM_PROFILE_DATA_POINTS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

static UINT8 profile_index;
static UINT8 pid_val_index;
static UINT8 pid_val_end;

static UINT8 max_num_points;
static UINT32 run_time;
static PID_VALIDATION_TYPE *p_pid_val;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: CalcValidationProfile
 * Description: Calculates a validation profile of velocities (in count/sec). 
 * Parameters: params - calibration/validation pid parameters
 *             low_percent - the lowest value in the profile
 *             high_percent - the highest value in the profile
 *             val_fwd_cps - the resulting profile
 * Return: FLOAT - velocity (count/second)
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalcValidationProfile(FLOAT low_percent, FLOAT high_percent, UINT8 num_points)
/* Use the min/max count/sec values captured during motor calibration
   Constrain the start and stop values to the lesser/greater of those values (depending on direction)
   Further constrain start/stop based on maximum PID values which are derived from the motor specification
   Further constrain to low/high range
*/
{
    FLOAT start;
    FLOAT stop;
    int ii;
    
    start = 0;
    stop = 0;

    if (p_pid_val->direction == DIR_FORWARD)
    {
        start = (FLOAT) min(Cal_GetMotorData(WHEEL_LEFT, DIR_FORWARD)->cps_min, Cal_GetMotorData(WHEEL_RIGHT, DIR_FORWARD)->cps_min);
        stop = (FLOAT) min(Cal_GetMotorData(WHEEL_LEFT, DIR_FORWARD)->cps_max, Cal_GetMotorData(WHEEL_RIGHT, DIR_FORWARD)->cps_max);
        stop = min(stop, (FLOAT) min(LEFTPID_MAX, RIGHTPID_MAX));
    }
    else if (p_pid_val->direction == DIR_BACKWARD)
    {
        /* Note: backward count/sec values are negative, i.e., swap min/max and negative PID max */
        start = (FLOAT) min(Cal_GetMotorData(WHEEL_LEFT, DIR_BACKWARD)->cps_max, Cal_GetMotorData(WHEEL_RIGHT, DIR_BACKWARD)->cps_max);
        stop = (FLOAT) max(Cal_GetMotorData(WHEEL_LEFT, DIR_BACKWARD)->cps_min, Cal_GetMotorData(WHEEL_RIGHT, DIR_BACKWARD)->cps_min);
        stop = max(stop, (FLOAT) max(-LEFTPID_MAX, -RIGHTPID_MAX));
    }
    
    start = low_percent * stop;
    stop = high_percent * stop;

    CalcTriangularProfile(num_points, start, stop, profile);
    for (ii = 0; ii < num_points; ++ii)
    {
        ConSer_WriteLine(FALSE, "%f, ", profile[ii]);
    }
    ConSer_WriteLine(TRUE, "");
}

/*---------------------------------------------------------------------------------------------------
 * Name: SetNextValidationVelocity
 * Description: Gets and sets the next validation velocity.
 * Parameters: p_pid_params - pointer to PID params
 * Return: 0 if all the velocities have been used; otherwise 1.
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 SetNextValidationVelocity()
{
    FLOAT velocity;
    
    velocity = profile[profile_index];    
    profile_index++;
    
    switch (p_pid_val->pid_type)
    {
        case PID_TYPE_LEFT:        
            Control_SetLeftRightVelocityCps(velocity, 0);
            break;

        case PID_TYPE_RIGHT:
            Control_SetLeftRightVelocityCps(0, velocity);
            break;
            
        default:
            ConSer_WriteLine(TRUE, "Unknown PID type");
            return CAL_COMPLETE;
            
    }
    
    //ConSer_WriteLine(TRUE, "Speed: %.2f %d", velocity, profile_index);
    
    return profile_index <= max_num_points ? 1 : 0;
}

static UINT8 RunValidation()
{
    UINT8 result;

    if (millis() - start_time < run_time)
    {
        return VAL_OK;    
    }
    start_time = millis();
    result = SetNextValidationVelocity();
    if (!result)
    {
        Control_SetLeftRightVelocityCps(0.0, 0.0);
        return INTERATION_DONE;
    }
    
    return VAL_OK;
}

static UINT8 SetNextValidation()
{
    pid_val_index++;

    p_pid_val = &pid_val[pid_val_index];
    profile_index = 0;

    return pid_val_index < pid_val_end ? VAL_OK : VAL_COMPLETE;
}

static void InitPidValidation(WHEEL_TYPE wheel, DIR_TYPE direction, FLOAT duration)
{
    switch (wheel)
    {
        case WHEEL_LEFT:
            pid_val[0].pid_type = PID_TYPE_LEFT;
            pid_val[0].direction = direction;
            pid_val_end = 1;
            break;

        case WHEEL_RIGHT:
            pid_val[0].pid_type = PID_TYPE_RIGHT;
            pid_val[0].direction = direction;
            pid_val_end = 1;
            break;

        case WHEEL_BOTH:
            pid_val[0].pid_type = PID_TYPE_LEFT;
            pid_val[0].direction = direction;
            pid_val[1].pid_type = PID_TYPE_RIGHT;
            pid_val[1].direction = direction;
            pid_val_end = 2;
            break;
    }

    profile_index = 0;
    pid_val_index = 0;
    run_time = min((UINT32)(duration*1000), MAX_RUN_TIME);
    p_pid_val = &pid_val[pid_val_index];
}

static void StartPidValidation()
{
    SetNextValidationVelocity();
    start_time = millis();
}

static UINT8 PerformPidValidation()
{
    UINT8 result;

    result = RunValidation();
    if ( result == INTERATION_DONE )
    {
        result = SetNextValidation();
    }
    
    return result;
}

static void EndPidValidation()
{
    /* Nothing to do at the moment, but who knows */
}

/*----------------------------------------------------------------------------------------------------------------------
 * Calibration Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
 * Module Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------
 * Name: ValPid_Init
 * Description: Initializes the PID calibration module 
 * Parameters: None 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void ValPid_Init(WHEEL_TYPE wheel, DIR_TYPE direction, FLOAT min_percent, FLOAT max_percent, UINT8 num_points, FLOAT duration)
{
    UINT16 mask;

    max_num_points = min(num_points, VAL_NUM_PROFILE_DATA_POINTS);

    Debug_Store();
    Debug_DisableAll();
    Control_OverrideDebug(TRUE);
    mask = 0;
    mask |= wheel == WHEEL_LEFT ? DEBUG_LEFT_PID_ENABLE_BIT : 0;
    mask |= wheel == WHEEL_RIGHT ? DEBUG_RIGHT_PID_ENABLE_BIT : 0;
    Debug_Enable(mask);

    Control_SetLeftRightVelocityOverride(TRUE);
    Pid_Enable(TRUE, TRUE, FALSE);      
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();
    
    InitPidValidation(wheel, direction, duration);
    CalcValidationProfile(min_percent, max_percent, num_points);
    StartPidValidation();
}

BOOL ValPid_Update()
{
    UINT8 result;

    result = PerformPidValidation();
    if (result == VAL_COMPLETE)
    {
        EndPidValidation();
        return FALSE;
    }

    return TRUE;
}

void ValPid_Results()
{
    Control_SetLeftRightVelocityOverride(FALSE);
    Control_OverrideDebug(FALSE);
    Debug_Restore();

    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Printing PID validation results");
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */