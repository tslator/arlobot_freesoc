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

#define STEP_VELOCITY_PERCENT  (0.2)    // 80% of maximum velocity

static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

extern volatile CAL_EEPROM_TYPE *p_cal_eeprom;

static uint32 start_time;
static uint16 old_debug_control_enabled;


static CAL_PID_PARAMS left_pid_params = {"left", PID_TYPE_LEFT, DIR_FORWARD, 5000};
static CAL_PID_PARAMS right_pid_params = {"right", PID_TYPE_RIGHT, DIR_FORWARD, 5000};

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

#define MAX_NUM_VELOCITIES (7)
static float velocities[MAX_NUM_VELOCITIES] = {0.0, 0.2, 0.5, 0.7, 0.5, 0.2, 0.0};
static uint8 vel_index = 0;

static void ResetPidValidationVelocity()
{
    vel_index = 0;
}

static float GetNextValidationVelocity(DIR_TYPE dir)
{
    float value;

    if (dir == DIR_FORWARD)
    {
        value = velocities[vel_index];
        vel_index++;// = (vel_index + 1) % (sizeof(velocities)/sizeof(float));
    }
    if (dir == DIR_BACKWARD)
    {
        value = -velocities[vel_index];
        vel_index++;// = (vel_index + 1) % (sizeof(velocities)/sizeof(float));
    }

    return value;
}

static uint8 SetNextValidationVelocity(CAL_PID_PARAMS *p_pid_params)
{
    char velocity_str[10];
    char outbuf[64];
    
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
    sprintf(outbuf, "%s\r\n", velocity_str);
    Ser_PutString(outbuf);
    
    return vel_index < MAX_NUM_VELOCITIES ? 1 : 0;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Calibration Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/
static uint8 Init(CAL_STAGE_TYPE stage, void *params)
{
    CAL_PID_PARAMS *p_pid_params = (CAL_PID_PARAMS *)params;
    char banner[64];
    
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            sprintf(banner, "\r\n%s PID calibration\r\n", p_pid_params->name);
            Ser_PutString(banner);
            Cal_SetLeftRightVelocity(0, 0);
            Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);

            old_debug_control_enabled = debug_control_enabled;

            switch (p_pid_params->pid_type)
            {
                case PID_TYPE_LEFT:
                    debug_control_enabled = DEBUG_LEFT_PID_ENABLE_BIT;// | DEBUG_LEFT_ENCODER_ENABLE_BIT;
                    break;
                    
                case PID_TYPE_RIGHT:
                    debug_control_enabled = DEBUG_RIGHT_PID_ENABLE_BIT;// | DEBUG_RIGHT_ENCODER_ENABLE_BIT;
                    break;
            }        
            break;
            
        case CAL_VALIDATE_STAGE:
            sprintf(banner, "\r\n%s PID validation\r\n", p_pid_params->name);
            Ser_PutString(banner);
            
            old_debug_control_enabled = debug_control_enabled;
            
            Cal_SetLeftRightVelocity(0, 0);
            Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);
            ResetPidValidationVelocity();

            switch (p_pid_params->pid_type)
            {
                case PID_TYPE_LEFT:
                    debug_control_enabled = DEBUG_LEFT_PID_ENABLE_BIT | DEBUG_LEFT_ENCODER_ENABLE_BIT;
                    break;
                
                case PID_TYPE_RIGHT:
                    debug_control_enabled = DEBUG_RIGHT_PID_ENABLE_BIT | DEBUG_RIGHT_ENCODER_ENABLE_BIT;
                    break;
            }
            break;
    }
    return CAL_OK;
}


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
                    
            Ser_PutString("\r\nCalibrating ");
            Ser_PutString("\r\n");
            start_time = millis();
            
            break;

            
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nValidating ");
            Ser_PutString("\r\n");
            start_time = millis();
            
            Pid_Enable(TRUE);            
            Encoder_Reset();
            Pid_Reset();
            Odom_Reset();

            SetNextValidationVelocity(p_pid_params);

            break;

    }

    return CAL_OK;
}

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
            /* Assume we have an array of validation velocities that we want to run through.
               We would use update to measure the time and advance through the array
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

static uint8 Stop(CAL_STAGE_TYPE stage, void *params)
{
    char output[64];
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

            Pid_RestoreLeftRightTarget();
            sprintf(output, "\r\n%s PID calibration complete\r\n", p_pid_params->name);
            Ser_PutString(output);
            break;
            
        case CAL_VALIDATE_STAGE:
            sprintf(output, "\r\n%s PID validation complete\r\n", p_pid_params->name);
            Ser_PutString(output);
            break;
    }
            
    debug_control_enabled = old_debug_control_enabled;    

    return CAL_OK;
}

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
    }
        
    return CAL_OK;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Module Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/
void CalPid_Init()
{
    CalPid_LeftCalibration = &left_pid_calibration;
    CalPid_RightCalibration = &right_pid_calibration;
    CalPid_LeftValidation = &left_pid_calibration;
    CalPid_RightValidation = &right_pid_calibration;
}