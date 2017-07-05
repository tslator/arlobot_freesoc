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
#include "calpid.h"
#include "cal.h"
#include "serial.h"
#include "nvstore.h"
#include "pid.h"
#include "utils.h"
#include "encoder.h"
#include "time.h"
#include "motor.h"
#include "debug.h"
#include "control.h"
#include "odom.h"

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#define STEP_VELOCITY_PERCENT  (0.2)    // 20% of maximum velocity
#define MAX_NUM_VELOCITIES     (7)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static uint32 start_time;

static CAL_PID_PARAMS left_pid_params = {"left", PID_TYPE_LEFT, DIR_FORWARD, 5000};
static CAL_PID_PARAMS right_pid_params = {"right", PID_TYPE_RIGHT, DIR_FORWARD, 5000};

static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static CALIBRATION_TYPE left_pid_calibration = {CAL_INIT_STATE,
                                                CAL_CALIBRATE_STAGE,
                                                &left_pid_params,
                                                Init,
                                                Start,
                                                Update,
                                                Stop,
                                                Results};

static CALIBRATION_TYPE right_pid_calibration = {CAL_INIT_STATE,
                                                 CAL_CALIBRATE_STAGE,
                                                 &right_pid_params,
                                                 Init,
                                                 Start,
                                                 Update,
                                                 Stop,
                                                 Results};

static CALIBRATION_TYPE left_pid_validation = {CAL_INIT_STATE,
                                               CAL_VALIDATE_STAGE,
                                                &left_pid_params,
                                                Init,
                                                Start,
                                                Update,
                                                Stop,
                                                Results};

static CALIBRATION_TYPE right_pid_validation = {CAL_INIT_STATE,
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
    Nvstore_WriteFloat(gains[0], (uint16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_gains.kp));
    Nvstore_WriteFloat(gains[1], (uint16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_gains.ki));
    Nvstore_WriteFloat(gains[2], (uint16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_gains.kd));
}

static void StoreRightGains(float *gains)
{
    Nvstore_WriteFloat(gains[0], (uint16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_gains.kp));
    Nvstore_WriteFloat(gains[1], (uint16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_gains.ki));
    Nvstore_WriteFloat(gains[2], (uint16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_gains.kd));
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetStepVelocity
 * Description: Calculates a step velocity at the percent based on the calibration motor values. 
 * Parameters: None
 * Return: float - velocity (meter/second)
 * 
 *-------------------------------------------------------------------------------------------------*/
static float GetStepVelocity()
/* The step input velocity is 80% of maximum wheel velocity.
   Maximum wheel velocity is determined by wheel calibration.  We take the min of all maximums, e.g., left forward max,
   left backward max, right forward max, right backward max.
   Resutling velocity must be in m/s.
 */
{
    static int32 left_fwd_max;
    static int32 left_bwd_max;
    static int32 right_fwd_max;
    static int32 right_bwd_max;
    static int32 left_max;
    static int32 right_max;
    static int32 max_cps;
    static float mps;

    left_fwd_max = abs(p_cal_eeprom->left_motor_fwd.cps_max/p_cal_eeprom->left_motor_fwd.cps_scale);
    left_bwd_max = abs(p_cal_eeprom->left_motor_bwd.cps_min/p_cal_eeprom->left_motor_bwd.cps_scale);
    right_fwd_max = abs(p_cal_eeprom->right_motor_fwd.cps_max/p_cal_eeprom->right_motor_fwd.cps_scale);
    right_bwd_max = abs(p_cal_eeprom->right_motor_bwd.cps_min/p_cal_eeprom->right_motor_bwd.cps_scale);
    
    left_max = min(left_fwd_max, left_bwd_max);
    right_max = min(right_fwd_max, right_bwd_max);
    
    max_cps = min(left_max, right_max);
    
    mps = (float) max_cps * METER_PER_COUNT * STEP_VELOCITY_PERCENT;
    
    return mps;
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
 * Return: float - velocity (meter/second)
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

    /* Note: Convert CPS to MPS */
    return value * METER_PER_COUNT;
}

/*---------------------------------------------------------------------------------------------------
 * Name: SetNextValidationVelocity
 * Description: Gets and sets the next validation velocity.
 * Parameters: p_pid_params - pointer to PID params
 * Return: 0 if all the velocities have been used; otherwise 1.
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 SetNextValidationVelocity(CAL_PID_PARAMS *p_pid_params)
{
    char velocity_str[10];
    
    float velocity = GetNextValidationVelocity(p_pid_params->direction);
    switch (p_pid_params->pid_type)
    {
        case PID_TYPE_LEFT:
            Cal_SetLeftRightVelocity(velocity, 0);
            break;

        case PID_TYPE_RIGHT:
            Cal_SetLeftRightVelocity(0, velocity);
            break;
    }
    
    ftoa(velocity, velocity_str, 3);
    Ser_PutStringFormat("Speed: %s\r\n", velocity_str);
    
    return vel_index < MAX_NUM_VELOCITIES ? 1 : 0;
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
static uint8 Init(CAL_STAGE_TYPE stage, void *params)
{
    CAL_PID_PARAMS *p_pid_params = (CAL_PID_PARAMS *)params;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
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
            }        
            break;
            
        case CAL_VALIDATE_STAGE:
            Ser_PutStringFormat("\r\n%s PID validation\r\n", p_pid_params->name);
            
            Debug_Store();

            Cal_CalcTriangularProfile(MAX_NUM_VELOCITIES, 0.2, 0.8, val_fwd_cps, val_bwd_cps);
            
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
            }
            break;
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
static uint8 Start(CAL_STAGE_TYPE stage, void *params)
{
    float gains[3];       
    CAL_PID_PARAMS *p_pid_params = (CAL_PID_PARAMS *) params;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nEnter proportional gain: ");
            gains[0] = Cal_ReadResponse();
            Ser_PutString("\r\nEnter integral gain: ");
            gains[1] = Cal_ReadResponse();
            Ser_PutString("\r\nEnter derivative gain: ");
            gains[2] = Cal_ReadResponse();
            Ser_PutString("\r\n");
            
            Pid_Enable(TRUE);            
            Encoder_Reset();
            Pid_Reset();
            Odom_Reset();
            
            float step_velocity = GetStepVelocity();

            switch (p_pid_params->pid_type)
            {
                case PID_TYPE_LEFT:
                    Pid_LeftSetGains(gains[0], gains[1], gains[2]);
                    Cal_SetLeftRightVelocity(step_velocity, 0);
                    break;
                    
                case PID_TYPE_RIGHT:
                    Pid_RightSetGains(gains[0], gains[1], gains[2]);
                    Cal_SetLeftRightVelocity(0, step_velocity);
                    break;                    
            }
                    
            Ser_PutString("\r\nCalibrating\r\n");
            start_time = millis();
            
            break;

            
        case CAL_VALIDATE_STAGE:
            Pid_Enable(TRUE);            
            Encoder_Reset();
            Pid_Reset();
            Odom_Reset();

            Ser_PutString("\r\nValidating\r\n");
            start_time = millis();
            
            SetNextValidationVelocity(p_pid_params);

            break;
    }

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
static uint8 Update(CAL_STAGE_TYPE stage, void *params)
{
    CAL_PID_PARAMS * p_pid_params = (CAL_PID_PARAMS *) params;
        
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            if (millis() - start_time < p_pid_params->run_time)
            {
                return CAL_OK;
            }
            break;

        case CAL_VALIDATE_STAGE:
            /* Assume an array of validation velocities that we want to run through.
               We use update to measure the time and advance through the array
             */
            if (millis() - start_time < p_pid_params->run_time)
            {
                return CAL_OK;    
            }
            start_time = millis();
            uint8 result = SetNextValidationVelocity(p_pid_params);
            if (!result)
            {
                return CAL_COMPLETE;
            }
            
            return CAL_OK;
            break;
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
static uint8 Stop(CAL_STAGE_TYPE stage, void *params)
{
    CAL_PID_PARAMS *p_pid_params = (CAL_PID_PARAMS *)params;
    float gains[3];

    Cal_SetLeftRightVelocity(0, 0);

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:

            switch (p_pid_params->pid_type)
            {
                case PID_TYPE_LEFT:
                    Pid_LeftGetGains(&gains[0], &gains[1], &gains[2]);
                    StoreLeftGains(gains);
                    break;
                    
                case PID_TYPE_RIGHT:
                    Pid_RightGetGains(&gains[0], &gains[1], &gains[2]);
                    StoreRightGains(gains);
                    break;
            }
            Cal_SetCalibrationStatusBit(CAL_PID_BIT);

            Pid_RestoreLeftRightTarget();
            Ser_PutStringFormat("\r\n%s PID calibration complete\r\n", p_pid_params->name);
            Debug_Restore();    
            break;
            
        case CAL_VALIDATE_STAGE:
            Ser_PutStringFormat("\r\n%s PID validation complete\r\n", p_pid_params->name);
            Debug_Restore();    
            break;
    }
            
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
static uint8 Results(CAL_STAGE_TYPE stage, void *params)
{
    float gains[3];
    CAL_PID_PARAMS *p_pid_params = (CAL_PID_PARAMS *)params;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:

            Ser_PutString("\r\nPrinting PID calibration results\r\n");
    
            switch (p_pid_params->pid_type)
            {
                case PID_TYPE_LEFT:
                    Pid_LeftGetGains(&gains[0], &gains[1], &gains[2]);
                    Cal_PrintGains("Left PID", gains);
                    break;
                
                case PID_TYPE_RIGHT:
                    Pid_RightGetGains(&gains[0], &gains[1], &gains[2]);
                    Cal_PrintGains("Right PID", gains);
                    break;
            }
            break;
            
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nPrinting PID validation results\r\n");
            break;
            
        default:
            Ser_PutStringFormat("\r\nInvalid stage: %d", stage);
            break;
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
    CalPid_LeftCalibration = &left_pid_calibration;
    CalPid_RightCalibration = &right_pid_calibration;
    CalPid_LeftValidation = &left_pid_validation;
    CalPid_RightValidation = &right_pid_validation;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */