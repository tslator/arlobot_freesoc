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
#include "calpid.h"
#include "cal.h"
#include "serial.h"
#include "nvstore.h"
#include "pid.h"
#include "leftpid.h"
#include "rightpid.h"
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
#define STEP_VELOCITY_PERCENT  (0.8)    // 80% of maximum velocity

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

static CALVAL_INTERFACE_TYPE left_pid_calibration = {CAL_INIT_STATE,
                                                CAL_CALIBRATE_STAGE,
                                                &left_pid_params,
                                                Init,
                                                Start,
                                                Update,
                                                Stop,
                                                Results};

static CALVAL_INTERFACE_TYPE right_pid_calibration = {CAL_INIT_STATE,
                                                 CAL_CALIBRATE_STAGE,
                                                 &right_pid_params,
                                                 Init,
                                                 Start,
                                                 Update,
                                                 Stop,
                                                 Results};

static float max_cps;
static CALVAL_PID_PARAMS *p_pid_params;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: StoreLeftGains/StoreRightGains
 * Description: Stores the specified gain values into the EEPROM for the left PID. 
 * Parameters: gains - array of float values corresponding to Kp, Ki, Kd.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void StoreLeftGains(float *gains)
{
    Cal_SetGains(PID_TYPE_LEFT, gains);
}

static void StoreRightGains(float *gains)
{
    Cal_SetGains(PID_TYPE_RIGHT, gains);
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetStepVelocity
 * Description: Calculates a step velocity at the percent based on the calibration motor values. 
 * Parameters: None
 * Return: float - velocity (meter/second)
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalcMaxCps()
/* The step input velocity is 80% of maximum wheel velocity.
   Maximum wheel velocity is determined considering two factors:
        1. the maximum left/right PID value (derived from the theoretical maximum)
        2. the maximum calibrated velocity of each wheel
   The minimum value of the above is the basis for determining the step input velocity.
 */
{
    int16 left_fwd_max;
    int16 left_bwd_max;
    int16 right_fwd_max;
    int16 right_bwd_max;
    int16 left_max;
    int16 right_max;
    int16 max_leftright_cps;
    int16 max_leftright_pid;

    left_fwd_max = abs(Cal_GetMotorData(WHEEL_LEFT, DIR_FORWARD)->cps_max);
    left_bwd_max = abs(Cal_GetMotorData(WHEEL_LEFT, DIR_BACKWARD)->cps_min);
    right_fwd_max = abs(Cal_GetMotorData(WHEEL_RIGHT, DIR_FORWARD)->cps_max);
    right_bwd_max = abs(Cal_GetMotorData(WHEEL_RIGHT, DIR_BACKWARD)->cps_min);
    
    left_max = min(left_fwd_max, left_bwd_max);
    right_max = min(right_fwd_max, right_bwd_max);
    
    max_leftright_cps = min(left_max, right_max);
    max_leftright_pid = min(LEFTPID_MAX, RIGHTPID_MAX);
    
    max_cps = min(max_leftright_cps, max_leftright_pid);

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
    if (!Cal_GetCalibrationStatusBit(CAL_MOTOR_BIT))
    {
        Ser_PutStringFormat("Motor calibration not performed (%02x)\r\n", Cal_GetStatus());
        return CAL_COMPLETE;
    }

    Ser_PutStringFormat("\r\n%s PID calibration\r\n", p_pid_params->name);

    Cal_ClearCalibrationStatusBit(CAL_PID_BIT);
    Cal_SetLeftRightVelocity(0, 0);
    Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);

    Debug_Store();

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
    float gains[4];
    
    Ser_PutString("\r\nEnter proportional gain: ");
    gains[0] = Cal_ReadResponse();
    Ser_PutString("\r\nEnter integral gain: ");
    gains[1] = Cal_ReadResponse();
    Ser_PutString("\r\nEnter derivative gain: ");
    gains[2] = Cal_ReadResponse();
    Ser_PutString("\r\nEnter feedforward gain: ");
    gains[3] = Cal_ReadResponse();
    Ser_PutString("\r\n");
    
    Pid_Enable(TRUE, TRUE, FALSE);
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();
    
    float step_velocity = max_cps * STEP_VELOCITY_PERCENT;

    switch (p_pid_params->pid_type)
    {
        case PID_TYPE_LEFT:
            LeftPid_SetGains(gains[0], gains[1], gains[2], gains[3]);
            Cal_SetLeftRightVelocity(step_velocity, 0);
            break;
            
        case PID_TYPE_RIGHT:
            RightPid_SetGains(gains[0], gains[1], gains[2], gains[3]);
            Cal_SetLeftRightVelocity(0, step_velocity);
            break;
            
        default:
            Ser_PutString("Unknown PID type\r\n");
            return CAL_COMPLETE;
    }
            
    Ser_PutString("\r\nCalibrating\r\n");
    start_time = millis();
            
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
    if (millis() - start_time < p_pid_params->run_time)
    {
        return CAL_OK;
    }

    return CAL_COMPLETE;
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
    float gains[4];

    Cal_SetLeftRightVelocity(0, 0);

    switch (p_pid_params->pid_type)
    {
        case PID_TYPE_LEFT:
            LeftPid_GetGains(&gains[0], &gains[1], &gains[2], &gains[3]);
            StoreLeftGains(gains);
            break;
            
        case PID_TYPE_RIGHT:
            RightPid_GetGains(&gains[0], &gains[1], &gains[2], &gains[3]);
            StoreRightGains(gains);
            break;

        default:
            Ser_PutString("Unknown PID type\r\n");
            return CAL_COMPLETE;
    }
    Cal_SetCalibrationStatusBit(CAL_PID_BIT);

    Pid_RestoreLeftRightTarget();
    Ser_PutStringFormat("\r\n%s PID calibration complete\r\n", p_pid_params->name);
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
    float gains[4];
    
    Ser_PutString("\r\nPrinting PID calibration results\r\n");

    switch (p_pid_params->pid_type)
    {
        case PID_TYPE_LEFT:
            LeftPid_GetGains(&gains[0], &gains[1], &gains[2], &gains[3]);
            Cal_PrintGains("Left PID", gains);
            break;
        
        case PID_TYPE_RIGHT:
            RightPid_GetGains(&gains[0], &gains[1], &gains[2], &gains[3]);
            Cal_PrintGains("Right PID", gains);
            break;
            
        default:
            Ser_PutString("Unknown PID type\r\n");
            return CAL_COMPLETE;            
    }

    return CAL_OK;
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
void CalPid_Init()
{
    max_cps = 0.0;
    CalcMaxCps();    
}

CALVAL_INTERFACE_TYPE* CalPid_Start(WHEEL_TYPE wheel)
{
    switch (wheel)
    {
        case WHEEL_LEFT:
            left_pid_calibration.state = CAL_INIT_STATE;
            p_pid_params = left_pid_calibration.params;
            return &left_pid_calibration;

        case WHEEL_RIGHT:
            right_pid_calibration.state = CAL_INIT_STATE;
            p_pid_params = right_pid_calibration.params;
            return &right_pid_calibration;
            
        default:
            return (CALVAL_INTERFACE_TYPE *)NULL;
    }
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */