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
#include "pidang.h"
#include "pidleft.h"
#include "pidright.h"
#include "utils.h"
#include "debug.h"
#include "diag.h"
#include "consts.h"

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
#define PID_SAMPLE_TIME_MS  SAMPLE_TIME_MS(PID_SAMPLE_RATE)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

#if defined (LEFT_PID_DUMP_ENABLED) ||  \
    defined (RIGHT_PID_DUMP_ENABLED) || \
    defined (UNIPID_DUMP_ENABLED) ||    \
    defined (ANGPID_DUMP_ENABLED)

void DumpPid(char* const name, PIDControl* const pid)
{
    if ( Debug_IsEnabled(DEBUG_LEFT_PID_ENABLE_BIT|DEBUG_RIGHT_PID_ENABLE_BIT|DEBUG_UNIPID_ENABLE_BIT|DEBUG_ANGPID_ENABLE_BIT) )
    {
#ifdef JSON_OUTPUT_ENABLE
        DEBUG_PRINT_ARG("{ \"%s pid\": {\"set_point\":%.3f, \"input\":%.3f, \"error\":%.3f, \"last_input\":%.3f, \"iterm\":%.3f, \"output\":%.3f }}\r\n",
            name, 
            IS_NAN_DEFAULT(pid->setpoint, 0), 
            IS_NAN_DEFAULT(pid->input, 0), 
            IS_NAN_DEFAULT(pid->setpoint - pid->input, 0), 
            IS_NAN_DEFAULT(pid->lastInput, 0), 
            IS_NAN_DEFAULT(pid->iTerm, 0), 
            IS_NAN_DEFAULT(pid->output, 0)
        );
#else
        DEBUG_PRINT_ARG("%s pid: %f %f %f %f %f %f\r\n", 
            name, 
            IS_NAN_DEFAULT(pid->setpoint, 0), 
            IS_NAN_DEFAULT(pid->input, 0), 
            IS_NAN_DEFAULT(pid->setpoint - pid->input, 0), 
            IS_NAN_DEFAULT(pid->lastInput, 0), 
            IS_NAN_DEFAULT(pid->iTerm, 0), 
            IS_NAN_DEFAULT(pid->output, 0)
        );
#endif        
    }
}

#endif


/*---------------------------------------------------------------------------------------------------
 * Name: Pid_Init
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Pid_Init()
{
    //AngPid_Init();
    LeftPid_Init();    
    RightPid_Init();
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
    //AngPid_Start();
    LeftPid_Start();    
    RightPid_Start();
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
    static UINT32 last_update_time = PID_SCHED_OFFSET;
    UINT32 delta_time;
    
    PID_UPDATE_START();

    delta_time = millis() - last_update_time;
    PID_DEBUG_DELTA(delta_time);
    if (delta_time >= PID_SAMPLE_TIME_MS)
    {    
        last_update_time = millis();
        
        //AngPid_Process();
        LeftPid_Process();
        RightPid_Process();
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
    AngPid_Reset();
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
void Pid_Enable(BOOL left, BOOL right, BOOL uni)
{
    //AngPid_Enable(uni);
    LeftPid_Enable(left);
    RightPid_Enable(right);
}

void Pid_Bypass(BOOL left, BOOL right, BOOL uni)
{
    LeftPid_Bypass(left);
    RightPid_Bypass(right);
    //AngPid_Bypass(uni);
}

void Pid_BypassAll(BOOL bypass)
{
    //AngPid_Bypass(bypass);
    LeftPid_Bypass(bypass);
    RightPid_Bypass(bypass);
}

BOOL Pid_SetGains(PIDControl* const p_pid, CAL_PID_TYPE* const p_gains)
{
    BOOL result = FALSE;

    // Note: the PID gains are stored in EEPROM.  The EEPROM cannot be accessed until the EEPROM
    // component is started which is handled in the Nvstore module.  
    // Pid_Start is called after Nvstore_Start.
    
    if (Cal_GetCalibrationStatusBit(CAL_PID_BIT))
    {
        PIDTuningsSet(p_pid, p_gains->kp, p_gains->ki, p_gains->kd, p_gains->kf);
        result = TRUE;
    }
    else
    {
        Ser_PutString("No valid PID calibration\r\n");        
    }
    
    return result;
}


/* [] END OF FILE */
