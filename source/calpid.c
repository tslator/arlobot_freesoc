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
#include "calstore.h"
#include "conserial.h"
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

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef struct _tag_pid_cal
{
    WHEEL_TYPE wheel;
    UINT8 iterations;
    BOOL no_debug;
    BOOL interactive;
    FLOAT step;
    BOOL load_gains;
} PID_CAL_TYPE;

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
static PID_CAL_TYPE pid_cal;


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

static void GetCurrentGains(WHEEL_TYPE wheel, CAL_PID_PTR_TYPE const curr_gains)
{
    CAL_PID_PTR_TYPE p_gains;

    if (wheel == WHEEL_LEFT)
    {
        p_gains = Cal_GetPidGains(PID_TYPE_LEFT);
    }
    else if (wheel == WHEEL_RIGHT)
    {
        p_gains = Cal_GetPidGains(PID_TYPE_RIGHT);
    }

    *curr_gains = *p_gains;

    //ConSer_WriteLine(TRUE, "kp: %.3f ki: %.3f kd: %.3f kf: %.3f", curr_gains->kp, curr_gains->ki, curr_gains->kd, curr_gains->kf);
}


static void ReadGains(WHEEL_TYPE wheel, CAL_PID_PTR_TYPE const new_gains)
{
    CHAR prop_text[] = "Enter proportional gain (%.3f): ";
    CHAR integ_text[] = "Enter integral gain (%.3f): ";
    CHAR deriv_text[] = "Enter deriviative gain (%.3f): ";
    CHAR feedfwd_text[] = "Enter feedforward gain (%.3f): ";
    CHAR output[40];
    CAL_PID_TYPE curr_gains;

    GetCurrentGains(wheel, &curr_gains);
    
    ConSer_WriteLine(TRUE, "Type <enter> to keep the current gain value or enter a new value.");
    sprintf(output, prop_text, curr_gains.kp);
    ConSer_WriteLine(FALSE, output);
    new_gains->kp = Cal_ReadResponseWithDefault(curr_gains.kp);
    ConSer_WriteLine(TRUE, "");

    sprintf(output, integ_text, curr_gains.ki);
    ConSer_WriteLine(FALSE, output);
    new_gains->ki = Cal_ReadResponseWithDefault(curr_gains.ki);
    ConSer_WriteLine(TRUE, "");

    sprintf(output, deriv_text, curr_gains.kd);
    ConSer_WriteLine(FALSE, output);
    new_gains->kd = Cal_ReadResponseWithDefault(curr_gains.kd);
    ConSer_WriteLine(TRUE, "");

    sprintf(output, feedfwd_text, curr_gains.kf);
    ConSer_WriteLine(FALSE, output);
    new_gains->kf = Cal_ReadResponseWithDefault(curr_gains.kf);
    ConSer_WriteLine(TRUE, "");
}

static void UpdatePidGains(WHEEL_TYPE wheel, BOOL impulse)
{
    CAL_PID_TYPE gains;
    
    REQUIRE(wheel == WHEEL_LEFT || wheel == WHEEL_RIGHT);

    if (!impulse)
    {
        ReadGains(wheel, &gains);

        if (wheel == WHEEL_LEFT)
        {
            LeftPid_SetGains(gains.kp, gains.ki, gains.kd, gains.kf);
        }
                
        if (wheel == WHEEL_RIGHT)
        {
            RightPid_SetGains(gains.kp, gains.ki, gains.kd, gains.kf);
        }
    }    
}

static void SetVelocity(WHEEL_TYPE wheel, FLOAT step)
{
    FLOAT left_cps;
    FLOAT right_cps;

    max_cps = Cal_CalcMaxCps();
    step_velocity = max_cps * step;
    
    //ConSer_WriteLine(TRUE, "Setting step velocity: %.3f", step_velocity);
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
void CalPid_Init(WHEEL_TYPE wheel, BOOL impulse, FLOAT step, UINT8 iterations)
{
    if (!Cal_GetCalibrationStatusBit(CAL_MOTOR_BIT))
    {
        ConSer_WriteLine(TRUE, "Motor calibration not performed (%02x)", Cal_GetStatus());
        return;
    }
    
    pid_cal.wheel = wheel;
    pid_cal.iterations = iterations;

    ConSer_WriteLine(TRUE, "%s PID calibration", WheelToString(wheel, FORMAT_LOWER));
    
    SetupDebug(wheel, pid_cal.no_debug);
    Cal_ClearCalibrationStatusBit(CAL_PID_BIT);
    Control_SetLeftRightVelocityOverride(TRUE);
    Control_SetLeftRightVelocityCps(0.0, 0.0);
    UpdatePidGains(wheel, impulse);
    Pid_BypassAll(!impulse);
    Pid_Enable(!impulse, !impulse, FALSE);
    Pid_Reset();
    Encoder_Reset();
    Odom_Reset();

    SetVelocity(wheel, pid_cal.step > 0.0 ? pid_cal.step : step);    

    start_time = millis();
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalPid_Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: None 
 * Return: BOOL - TRUE indicates the operation is still running; FALSE indicates the operation has
 *                completed.
 *-------------------------------------------------------------------------------------------------*/
BOOL CalPid_Update()
{
    UINT32 delta;

    delta = millis() - start_time;
    if (delta < p_pid_params->run_time)
    {
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalPid_Results
 * Description: PID calibration interface Results function.  Called after operation completes.
 * Parameters: None 
 * Return: None
 *-------------------------------------------------------------------------------------------------*/
void CalPid_Results(void)
{
    FLOAT gains[4];

    REQUIRE(pid_cal.wheel == WHEEL_LEFT || pid_cal.wheel == WHEEL_RIGHT);
    
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "%s PID calibration complete", pid_cal.wheel == WHEEL_LEFT ? "Left" : "Right");
    Control_SetLeftRightVelocityCps(0, 0);
    Control_SetLeftRightVelocityOverride(FALSE);
    Debug_Restore();    


    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Printing PID calibration results");
    switch (pid_cal.wheel)
    {
        case WHEEL_LEFT:
            Cal_PrintLeftPidGains(TRUE);
            break;
        
        case WHEEL_RIGHT:
            Cal_PrintRightPidGains(TRUE);
            break;
            
        default:
            break;
    }    

    /* Note: I want to calculate these parameters after each calibration run
    https://www.mathworks.com/help/control/ref/stepinfo.html?requestedDomain=www.mathworks.com    
    http://www.mee.tcd.ie/~corrigad/3c1/control_ho2_2012_students.pdf
    */    

    /* Ask if the gains should be committed */
    ConSer_WriteLine(FALSE, "Store gains (y/n)? ");
    CHAR value = Cal_ReadResponseChar();
    if (value == 'y')
    {
        if (pid_cal.wheel == WHEEL_LEFT)
        {
            LeftPid_GetGains(&gains[0], &gains[1], &gains[2], &gains[3]);
            Cal_SetGains(PID_TYPE_LEFT, gains);
        }
        
        if (pid_cal.wheel == WHEEL_RIGHT)
        {
            RightPid_GetGains(&gains[0], &gains[1], &gains[2], &gains[3]);
            Cal_SetGains(PID_TYPE_RIGHT, gains);
        }

        Cal_SetCalibrationStatusBit(CAL_PID_BIT);
    }
}

void CalPid_SetStep(FLOAT step)
{
    pid_cal.step = step;
}

void CalPid_SetDebug(BOOL no_debug)
{
    pid_cal.no_debug = no_debug;
}

void CalPid_SetInteractive()
{
    pid_cal.interactive = TRUE;
}

void CalPid_SetLoadGains()
{
    pid_cal.load_gains = TRUE;
}

void CalPid_Clear()
{
    pid_cal.step = 0.0;
    pid_cal.no_debug = FALSE;
    pid_cal.interactive = FALSE;
    pid_cal.load_gains = FALSE;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */