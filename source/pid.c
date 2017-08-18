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
   Description: This module provides a general abstraction for PID control.  There are PIDs for each
   wheel (left, right) and for linear and angular velocity.
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdio.h>
#include "config.h"
#include "time.h"
#include "control.h"
#include "encoder.h"
#include "motor.h"
#include "odom.h"
#include "pid.h"
#include "leftpid.h"
#include "rightpid.h"
#include "unipid.h"
#include "utils.h"
#include "pidutil.h"
#include "debug.h"
#include "diag.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef PID_UPDATE_DELTA_ENABLED
#define PID_DEBUG_DELTA(delta)  DEBUG_DELTA_TIME("pid", delta)
#else
#define PID_DEBUG_DELTA(delta)
#endif


/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    



/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    


/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Init
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Init()
{
    LeftPid_Init();    
    RightPid_Init();
    UniPid_Init();
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Start
 * Description: Obtains the left/right PID gains and sets them into the left/right PID controller.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Start()
{
    LeftPid_Start();    
    RightPid_Start();
    UniPid_Start();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Update
 * Description: Updates the left/right PID fields.  This function is called from the main loop and
 *              enforces the PID sampling rate.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Update()
{
    static uint32 last_update_time = PID_SCHED_OFFSET;
    uint32 delta_time;
    
    PID_UPDATE_START();

    delta_time = millis() - last_update_time;
    PID_DEBUG_DELTA(delta_time);
    if (delta_time >= PID_SAMPLE_TIME_MS)
    {    
        last_update_time = millis();
        
        LeftPid_Process();
        RightPid_Process();
        //UniPid_Process();
    }
    
    PID_UPDATE_END();    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_SetLeftRightTarget
 * Description: Sets the left/right get_target fields to a different function.
 *              Note: This is done during calibration to allow internal control of the left/right
 *              wheel speed.
 * Parameters: left_target - the function that will be used to get the left target speed
 *             right_target - the function that will be used to get the right target speed
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_SetLeftRightTarget(GET_TARGET_FUNC_TYPE left_target, GET_TARGET_FUNC_TYPE right_target)
{
    LeftPid_SetTarget(left_target);
    RightPid_SetTarget(right_target);
}

void Pid_SetUniTarget(GET_TARGET_FUNC_TYPE linear_target, GET_TARGET_FUNC_TYPE angular_target)
{
    UniPid_SetTarget(linear_target, angular_target);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_RestoreLeftRightTarget
 * Description: Restores the left/right get_target fields to the default function.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_RestoreLeftRightTarget()
{
    LeftPid_RestoreTarget();
    RightPid_RestoreTarget();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Reset
 * Description: Resets the left/right PID fields.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Reset()
{
    LeftPid_Reset();
    RightPid_Reset();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Enable
 * Description: Enables/Disables the PID.  This is needed for motor and PID calibration.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Enable(uint8 enable)
{
    LeftPid_Enable(enable);
    RightPid_Enable(enable);
}

void Pid_Bypass(uint8 bypass)
{
    LeftPid_Bypass(bypass);
    RightPid_Bypass(bypass);
}

/* [] END OF FILE */
