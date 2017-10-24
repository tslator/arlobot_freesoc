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
#include "leftpid.h"
#include "rightpid.h"
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
static uint32 start_time;

static CALVAL_PID_PARAMS left_pid_params = {"left", PID_TYPE_LEFT, DIR_FORWARD, 3000};
static CALVAL_PID_PARAMS right_pid_params = {"right", PID_TYPE_RIGHT, DIR_FORWARD, 3000};

static uint8 Init();
static uint8 Start();
static uint8 Update();
static uint8 Stop();
static uint8 Results();

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

static float val_fwd_cps[MAX_NUM_VELOCITIES] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static float val_bwd_cps[MAX_NUM_VELOCITIES] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static uint8 vel_index = 0;


static float max_cps;
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
 * Return: float - velocity (count/second)
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalcValidationProfile(float low_percent, float high_percent, float *val_fwd_cps)
{
    float start;
    float stop;
    int ii;
    
    start = 0;
    stop = 0;

    if (p_pid_params->direction == DIR_FORWARD)
    {
        start = (float) min(WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_FORWARD]->cps_min, WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_FORWARD]->cps_min);
        stop = (float) min(WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_FORWARD]->cps_max, WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_FORWARD]->cps_max);
    }
    else if (p_pid_params->direction == DIR_BACKWARD)
    {
        start = (float) min(WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_BACKWARD]->cps_min, WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_BACKWARD]->cps_min);
        stop = (float) min(WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_BACKWARD]->cps_max, WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_BACKWARD]->cps_max);
    }

    stop = min(stop, (float) min(LEFTPID_MAX, RIGHTPID_MAX));
    start = low_percent * stop;
    stop = high_percent * stop;
    
    CalcTriangularProfile(MAX_NUM_VELOCITIES, start, stop, val_fwd_cps);
    for (ii = 0; ii < MAX_NUM_VELOCITIES; ++ii)
    {
        Ser_PutStringFormat("%f, ", val_fwd_cps[ii]);
    }
    Ser_PutString("\r\n");
}

/*---------------------------------------------------------------------------------------------------
 * Name: ResetPidValidationVelocity
 * Description: Resets the index for the validation velocities. 
 * Parameters: None
 * Return: float - velocity (meter/second)
 * 
 *-------------------------------------------------------------------------------------------------*/
static void ResetPidValidationVelocity()
{
    vel_index = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetNextValidationVelocity
 * Description: Returns the next validation velocity from the array. 
 * Parameters: dir - specifies whether the validation is in the forward or backward direction
 * Return: float - velocity (count/second)
 * 
 *-------------------------------------------------------------------------------------------------*/
static float GetNextValidationVelocity(DIR_TYPE dir)
{
    float value;

    switch( dir )
    {
        case DIR_FORWARD:
            value = val_fwd_cps[vel_index];
            break;
    
        case DIR_BACKWARD:
            value = val_bwd_cps[vel_index];
            break;

        default:
            value = 0;
            break;
    }

    vel_index++;

    return value;
}

/*---------------------------------------------------------------------------------------------------
 * Name: SetNextValidationVelocity
 * Description: Gets and sets the next validation velocity.
 * Parameters: p_pid_params - pointer to PID params
 * Return: 0 if all the velocities have been used; otherwise 1.
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 SetNextValidationVelocity()
{
    float velocity = GetNextValidationVelocity(p_pid_params->direction);
    switch (p_pid_params->pid_type)
    {
        case PID_TYPE_LEFT:
            Cal_SetLeftRightVelocity(velocity, 0);
            break;

        case PID_TYPE_RIGHT:
            Cal_SetLeftRightVelocity(0, velocity);
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
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Init()
{
    Ser_PutStringFormat("\r\n%s PID validation\r\n", p_pid_params->name);
    
    Debug_Store();

    CalcValidationProfile(0.2, 0.8, &val_fwd_cps[0]);
    
    Cal_SetLeftRightVelocity(0, 0);
    Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);
    ResetPidValidationVelocity();

    switch (p_pid_params->pid_type)
    {
        case PID_TYPE_LEFT:
            Debug_Enable(DEBUG_LEFT_PID_ENABLE_BIT);
            break;
        
        case PID_TYPE_RIGHT:
            Debug_Enable(DEBUG_RIGHT_PID_ENABLE_BIT);
            break;
            
        default:
            Ser_PutString("Unknown PID type\r\n");
            return CAL_COMPLETE;
            
    }

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Starts PID Calibration/Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - PID validation parameters. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Start()
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
 * Return: uint8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Update()
{
    /* Assume an array of validation velocities that we want to run through.
        We use update to measure the time and advance through the array
        */
    if (millis() - start_time < p_pid_params->run_time)
    {
        return CAL_OK;    
    }
    start_time = millis();
    uint8 result = SetNextValidationVelocity();
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
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Stop()
{
    Cal_SetLeftRightVelocity(0, 0);

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
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Results()
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

CALVAL_INTERFACE_TYPE * ValPid_Start(VAL_PID_TYPE val_pid)
{
    switch (val_pid)
    {
        case VAL_PID_LEFT_FORWARD:
            left_pid_validation.stage = CAL_VALIDATE_STAGE;
            left_pid_validation.state = CAL_INIT_STATE;
            p_pid_params = left_pid_validation.params;
            p_pid_params->direction = DIR_FORWARD;
            return &left_pid_validation;

        case VAL_PID_LEFT_BACKWARD:
            left_pid_validation.stage = CAL_VALIDATE_STAGE;
            left_pid_validation.state = CAL_INIT_STATE;
            p_pid_params = left_pid_validation.params;
            p_pid_params->direction = DIR_BACKWARD;
            return &left_pid_validation;

        case VAL_PID_RIGHT_FORWARD:
            right_pid_validation.stage = CAL_VALIDATE_STAGE;
            right_pid_validation.state = CAL_INIT_STATE;
            p_pid_params = right_pid_validation.params;
            p_pid_params->direction = DIR_FORWARD;
            return &right_pid_validation;
    
        case VAL_PID_RIGHT_BACKWARD:
            right_pid_validation.stage = CAL_VALIDATE_STAGE;
            right_pid_validation.state = CAL_INIT_STATE;
            p_pid_params = right_pid_validation.params;
            p_pid_params->direction = DIR_BACKWARD;
            return &right_pid_validation;

        default:
            return (CALVAL_INTERFACE_TYPE *) NULL;
    }    
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */