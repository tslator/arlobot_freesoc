/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/*---------------------------------------------------------------------------------------------------
 * Description
 *-------------------------------------------------------------------------------------------------*/
// Add a description of the module

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdio.h>
#include "config.h"
#include "pid.h"
#include "leftpid.h"
#include "rightpid.h"
#include "time.h"
#include "utils.h"
#include "encoder.h"
#include "motor.h"
#include "diag.h"
#include "debug.h"
#include "odom.h"
#include "control.h"
#include "pidutil.h"
#include "debug.h"

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

static uint8 pid_enabled;


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
    pid_enabled = 0;
    
    LeftPid_Init();    
    RightPid_Init();
    //LinearPid_Init();
    //AngularPid_Init();
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
    //LinearPid_Start();
    //AngularPid_Start();    
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
        //LinearPid_Process();
        //AngularPid_Process();
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

/* [] END OF FILE */
