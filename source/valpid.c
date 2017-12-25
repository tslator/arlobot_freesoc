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
#include "config.h"
#include "valpid.h"
#include "cal.h"
#include "serial.h"
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
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#define MAX_NUM_VELOCITIES     (7)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static UINT32 start_time;

static CALVAL_PID_PARAMS left_pid_params = {"left", PID_TYPE_LEFT, DIR_FORWARD, 3000};
static CALVAL_PID_PARAMS right_pid_params = {"right", PID_TYPE_RIGHT, DIR_FORWARD, 3000};

static UINT8 Init();
static UINT8 Start();
static UINT8 Update();
static UINT8 Stop();
static UINT8 Results();

static CALVAL_INTERFACE_TYPE left_pid_validation = {CAL_INIT_STATE,
                                               CAL_VALIDATE_STAGE,
                                                &left_pid_params,
                                                Init,
                                                Start,
                                                Update,
                                                Stop,
                                                Results};

static CALVAL_INTERFACE_TYPE right_pid_validation = {CAL_INIT_STATE,
                                                 CAL_VALIDATE_STAGE,
                                                 &right_pid_params,
                                                 Init,
                                                 Start,
                                                 Update,
                                                 Stop,
                                                 Results};

static FLOAT profile_cps[MAX_NUM_VELOCITIES] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static UINT8 vel_index = 0;


static FLOAT max_cps;
static CALVAL_PID_PARAMS *p_pid_params;

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
static void CalcValidationProfile(FLOAT low_percent, FLOAT high_percent)
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

    if (p_pid_params->direction == DIR_FORWARD)
    {
        start = (FLOAT) min(Cal_GetMotorData(WHEEL_LEFT, DIR_FORWARD)->cps_min, Cal_GetMotorData(WHEEL_RIGHT, DIR_FORWARD)->cps_min);
        stop = (FLOAT) min(Cal_GetMotorData(WHEEL_LEFT, DIR_FORWARD)->cps_max, Cal_GetMotorData(WHEEL_RIGHT, DIR_FORWARD)->cps_max);
        stop = min(stop, (FLOAT) min(LEFTPID_MAX, RIGHTPID_MAX));
    }
    else if (p_pid_params->direction == DIR_BACKWARD)
    {
        /* Note: backward count/sec values are negative, i.e., swap min/max and negative PID max */
        start = (FLOAT) min(Cal_GetMotorData(WHEEL_LEFT, DIR_BACKWARD)->cps_max, Cal_GetMotorData(WHEEL_RIGHT, DIR_BACKWARD)->cps_max);
        stop = (FLOAT) max(Cal_GetMotorData(WHEEL_LEFT, DIR_BACKWARD)->cps_min, Cal_GetMotorData(WHEEL_RIGHT, DIR_BACKWARD)->cps_min);
        stop = max(stop, (FLOAT) max(-LEFTPID_MAX, -RIGHTPID_MAX));
    }
    
    start = low_percent * stop;
    stop = high_percent * stop;

    CalcTriangularProfile(MAX_NUM_VELOCITIES, start, stop, profile_cps);
    for (ii = 0; ii < MAX_NUM_VELOCITIES; ++ii)
    {
        Ser_PutStringFormat("%f, ", profile_cps[ii]);
    }
    Ser_PutString("\r\n");
}

/*---------------------------------------------------------------------------------------------------
 * Name: ResetPidValidationVelocity
 * Description: Resets the index for the validation velocities. 
 * Parameters: None
 * Return: FLOAT - velocity (meter/second)
 * 
 *-------------------------------------------------------------------------------------------------*/
static void ResetPidValidationVelocity()
{
    vel_index = 0;
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
    
    velocity = profile_cps[vel_index];    
    vel_index++;
    
    switch (p_pid_params->pid_type)
    {
        case PID_TYPE_LEFT:        
            Control_SetLeftRightVelocity(velocity, 0);
            break;

        case PID_TYPE_RIGHT:
            Control_SetLeftRightVelocity(0, velocity);
            break;
            
        default:
            Ser_PutString("Unknown PID type\r\n");
            return CAL_COMPLETE;
            
    }
    
    Ser_PutStringFormat("Speed: %.2f\r\n", velocity);
    
    return vel_index <= MAX_NUM_VELOCITIES ? 1 : 0;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Calibration Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: Init
 * Description: Calibration/Validation interface Init function.  Performs initialization for Linear 
 *              Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - PID calibration/validation parameters, e.g. direction, run time, etc. 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Init()
{
    Control_OverrideDebug(TRUE);
    Ser_PutStringFormat("\r\n%s PID validation\r\n", p_pid_params->name);
    
    Debug_Store();
    Debug_DisableAll();

    Control_SetLeftRightVelocityOverride(TRUE);
    ResetPidValidationVelocity();
    
    Debug_Enable(DEBUG_LEFT_PID_ENABLE_BIT|DEBUG_RIGHT_PID_ENABLE_BIT);

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Starts PID Calibration/Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - PID validation parameters. 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Start()
{
    Pid_Enable(TRUE, TRUE, FALSE);            
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();

    Ser_PutString("\r\nValidating\r\n");
    start_time = millis();
    
    SetNextValidationVelocity();

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: stage - the calibration/validation stage 
 *             params - PID calibration/validation parameters, e.g. direction, run time, etc. 
 * Return: UINT8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Update()
{
    UINT8 result;
    
    /* Assume an array of validation velocities that we want to run through.
        We use update to measure the time and advance through the array
        */
    if (millis() - start_time < p_pid_params->run_time)
    {
        return CAL_OK;    
    }
    start_time = millis();
    result = SetNextValidationVelocity();
    if (!result)
    {
        return CAL_COMPLETE;
    }
    
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Stop
 * Description: Calibration/Validation interface Stop function.  Called to stop validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - PID calibration/validation parameters, e.g. direction, run time, etc. 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Stop()
{
    Control_SetLeftRightVelocity(0.0, 0.0);
    Control_SetLeftRightVelocityOverride(FALSE);

    Ser_PutStringFormat("\r\n%s PID validation complete\r\n", p_pid_params->name);
    Debug_Restore();    
            
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Results
 * Description: Calibration/Validation interface Results function.  Called to display calibration/ 
 *              validation results. 
 * Parameters: stage - the calibration/validation stage 
 *             params - PID calibration/validation parameters, e.g. direction, run time, etc. 
 * Return: UINT8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static UINT8 Results()
{
    Ser_PutString("\r\nPrinting PID validation results\r\n");
        
    return CAL_OK;
}

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
void ValPid_Init()
{
    max_cps = 0.0;
}

CALVAL_INTERFACE_TYPE* const ValPid_Start(VAL_PID_TYPE val_pid)
{
    switch (val_pid)
    {
        case VAL_PID_LEFT_FORWARD:
            left_pid_validation.stage = CAL_VALIDATE_STAGE;
            left_pid_validation.state = CAL_INIT_STATE;
            p_pid_params = left_pid_validation.params;
            p_pid_params->direction = DIR_FORWARD;
            CalcValidationProfile(0.2, 0.8);
            return &left_pid_validation;

        case VAL_PID_LEFT_BACKWARD:
            left_pid_validation.stage = CAL_VALIDATE_STAGE;
            left_pid_validation.state = CAL_INIT_STATE;
            p_pid_params = left_pid_validation.params;
            p_pid_params->direction = DIR_BACKWARD;
            CalcValidationProfile(0.2, 0.8);        
            return &left_pid_validation;

        case VAL_PID_RIGHT_FORWARD:
            right_pid_validation.stage = CAL_VALIDATE_STAGE;
            right_pid_validation.state = CAL_INIT_STATE;
            p_pid_params = right_pid_validation.params;
            p_pid_params->direction = DIR_FORWARD;
            CalcValidationProfile(0.2, 0.8);
            return &right_pid_validation;
    
        case VAL_PID_RIGHT_BACKWARD:
            right_pid_validation.stage = CAL_VALIDATE_STAGE;
            right_pid_validation.state = CAL_INIT_STATE;
            p_pid_params = right_pid_validation.params;
            p_pid_params->direction = DIR_BACKWARD;
            CalcValidationProfile(0.2, 0.8);        
            return &right_pid_validation;

        default:
            return (CALVAL_INTERFACE_TYPE *) NULL;
    }    
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */