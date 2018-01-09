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
#include <stdio.h>
#include "control.h"
#include "calpid.h"
#include "cal.h"
#include "serial.h"
#include "nvstore.h"
#include "pid.h"
#include "pidleft.h"
#include "pidright.h"
#include "utils.h"
#include "encoder.h"
#include "time.h"
#include "motor.h"
#include "debug.h"
#include "control.h"
#include "odom.h"
#include "assertion.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define STEP_VELOCITY_PERCENT  (0.8)    // 80% of maximum velocity

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static UINT32 start_time;

static CALVAL_PID_PARAMS left_pid_params = {"left", PID_TYPE_LEFT, DIR_FORWARD, 3000};
static CALVAL_PID_PARAMS right_pid_params = {"right", PID_TYPE_RIGHT, DIR_FORWARD, 3000};

static FLOAT max_cps;
static CALVAL_PID_PARAMS *p_pid_params;
static FLOAT step_velocity;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static void SetupDebug(WHEEL_TYPE wheel, BOOL no_debug)
{
    UINT16 mask;

    Control_OverrideDebug(TRUE);
    Debug_Store();
    Debug_DisableAll();
    mask = no_debug ? 0 : (wheel == WHEEL_LEFT) ? DEBUG_LEFT_PID_ENABLE_BIT : DEBUG_RIGHT_PID_ENABLE_BIT;
    Debug_Enable(mask);
}

static void GetCurrentGains(WHEEL_TYPE wheel, CAL_PID_TYPE* curr_gains)
{
    CAL_PID_TYPE *p_gains;

    if (wheel == WHEEL_LEFT)
    {
        p_gains = Cal_GetPidGains(PID_TYPE_LEFT);
    }
    else if (wheel == WHEEL_RIGHT)
    {
        p_gains = Cal_GetPidGains(PID_TYPE_RIGHT);
    }

    *curr_gains = *p_gains;

    //Ser_PutStringFormat("kp: %.3f ki: %.3f kd: %.3f kf: %.3f\r\n", curr_gains->kp, curr_gains->ki, curr_gains->kd, curr_gains->kf);
}


static void ReadGains(WHEEL_TYPE wheel, CAL_PID_TYPE* new_gains)
{
    CHAR prop_text[] = "Enter proportional gain (%.3f): ";
    CHAR integ_text[] = "Enter integral gain (%.3f): ";
    CHAR deriv_text[] = "Enter deriviative gain (%.3f): ";
    CHAR feedfwd_text[] = "Enter feedforward gain (%.3f): ";
    CHAR output[40];
    CAL_PID_TYPE curr_gains;

    GetCurrentGains(wheel, &curr_gains);
    
    Ser_WriteLine("Type <enter> to keep the current gain value or enter a new value.", TRUE);
    sprintf(output, prop_text, curr_gains.kp);
    Ser_WriteLine(output, FALSE);
    new_gains->kp = Cal_ReadResponseWithDefault(curr_gains.kp);
    Ser_WriteLine("", TRUE);

    sprintf(output, integ_text, curr_gains.ki);
    Ser_WriteLine(output, FALSE);
    new_gains->ki = Cal_ReadResponseWithDefault(curr_gains.ki);
    Ser_WriteLine("", TRUE);

    sprintf(output, deriv_text, curr_gains.kd);
    Ser_WriteLine(output, FALSE);
    new_gains->kd = Cal_ReadResponseWithDefault(curr_gains.kd);
    Ser_WriteLine("", TRUE);

    sprintf(output, feedfwd_text, curr_gains.kf);
    Ser_WriteLine(output, FALSE);
    new_gains->kf = Cal_ReadResponseWithDefault(curr_gains.kf);
    Ser_WriteLine("", TRUE);
}

static void UpdatePidGains(WHEEL_TYPE wheel, BOOL impulse)
{
    CAL_PID_TYPE gains;

    if (!impulse)
    {
        ReadGains(wheel, &gains);

        switch (wheel)
        {
            case WHEEL_LEFT:
                LeftPid_SetGains(gains.kp, gains.ki, gains.kd, gains.kf);
                break;
                
            case WHEEL_RIGHT:
                RightPid_SetGains(gains.kp, gains.ki, gains.kd, gains.kf);
                break;  
                
            default:
            case WHEEL_BOTH:
                ASSERTION(FALSE, "Unexpected wheel");
                break;
        }
    }    
}

static void SetVelocity(WHEEL_TYPE wheel, FLOAT step)
{
    FLOAT left_cps;
    FLOAT right_cps;

    max_cps = Cal_CalcMaxCps();
    step_velocity = max_cps * step;
    
    //Ser_PutStringFormat("Setting step velocity: %.3f\r\n", step_velocity);
    left_cps = 0.0;
    right_cps = 0.0;

    switch (wheel)
    {
        case WHEEL_LEFT:
            p_pid_params = &left_pid_params;
            left_cps = step_velocity;
            break;
            
        case WHEEL_RIGHT:
            p_pid_params = &right_pid_params;
            right_cps = step_velocity;
            break;  
            
        default:
        case WHEEL_BOTH:
            return;
    }
    
    Control_SetLeftRightVelocityCps(left_cps, right_cps);
    
}

/*----------------------------------------------------------------------------------------------------------------------
 * Module Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
 * Name: CalPid_Init
 * Description: Initializes the PID calibration module 
 * Parameters: None 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void CalPid_Init(WHEEL_TYPE wheel, BOOL impulse, FLOAT step, BOOL no_debug)
{
    Ser_PutStringFormat("%s PID calibration\r\n", WheelToString(wheel, FORMAT_LOWER));

    SetupDebug(wheel, no_debug);
    Cal_ClearCalibrationStatusBit(CAL_PID_BIT);
    Control_SetLeftRightVelocityOverride(TRUE);
    Control_SetLeftRightVelocityCps(0.0, 0.0);
    UpdatePidGains(wheel, impulse);
    Pid_BypassAll(!impulse);
    Pid_Enable(!impulse, !impulse, FALSE);
    Pid_Reset();
    Encoder_Reset();
    Odom_Reset();

    SetVelocity(wheel, step);    

    start_time = millis();
}


/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: None 
 * Return: UINT8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT8 CalPid_Update()
{
    UINT32 delta;

    delta = millis() - start_time;
    if (delta < p_pid_params->run_time)
    {
        return CAL_OK;
    }

    return CAL_COMPLETE;
}


/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */