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

#define STEP_VELOCITY_PERCENT  (0.8)    // 80% of maximum velocity

static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

extern volatile CAL_EEPROM_TYPE *p_cal_eeprom;

static uint32 start_time;
static uint16 old_debug_control_enabled;


typedef struct cal_pid_params_tag
{
    char name[6];
    PID_TYPE pid_type;
    uint32 run_time;
} CAL_PID_PARAMS;

static CAL_PID_PARAMS left_pid_params = {"left", PID_TYPE_LEFT, 5000};
static CAL_PID_PARAMS right_pid_params = {"right", PID_TYPE_RIGHT, 5000};

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

static void PrintGains(char *label, float *gains)
{
    char output[64];
    char pgain_str[10];
    char igain_str[10];
    char dgain_str[10];
    
    ftoa(gains[0], pgain_str, 3);
    ftoa(gains[1], igain_str, 3);
    ftoa(gains[2], dgain_str, 3);
    
    sprintf(output, "%s - P: %s, I: %s, D: %s\r\n", label, pgain_str, igain_str, dgain_str);
    Ser_PutString(output);
}

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

/*----------------------------------------------------------------------------------------------------------------------
 * Calibration Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/
static uint8 Init(CAL_STAGE_TYPE stage, void *params)
{
    CAL_PID_PARAMS *p_pid_params = (CAL_PID_PARAMS *)params;
    char banner[64];
    
    sprintf(banner, "\r\n%s PID calibration\r\n", p_pid_params->name);
    Ser_PutString(banner);
    old_debug_control_enabled = debug_control_enabled;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            {
                switch (p_pid_params->pid_type)
                {
                    case PID_TYPE_LEFT:
                        debug_control_enabled = DEBUG_LEFT_PID_ENABLE_BIT;// | DEBUG_LEFT_ENCODER_ENABLE_BIT;

                        Cal_SetLeftRightVelocity(0, 0);
                        Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);
                        break;
                        
                    case PID_TYPE_RIGHT:
                        debug_control_enabled = DEBUG_RIGHT_PID_ENABLE_BIT | DEBUG_RIGHT_MOTOR_ENABLE_BIT | DEBUG_RIGHT_ENCODER_ENABLE_BIT;

                        Cal_SetLeftRightVelocity(0, 0);
                        Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);
                        break;
                }        
            }
            break;
            
        case CAL_VALIDATE_STAGE:
            sprintf(banner, "\r\n%s PID validation\r\n", p_pid_params->name);
            Ser_PutString(banner);
            old_debug_control_enabled = debug_control_enabled;
            debug_control_enabled = DEBUG_RIGHT_PID_ENABLE_BIT;// | DEBUG_LEFT_ENCODER_ENABLE_BIT;
            
            Cal_SetLeftRightVelocity(0, 0);
            Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);
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
            
            Encoder_Reset();
            Pid_Reset();
            Odom_Reset();

            switch (p_pid_params->pid_type)
            {
                case PID_TYPE_LEFT:
                    Pid_LeftSetGains(gains[0], gains[1], gains[2]);
                    Cal_SetLeftRightVelocity(GetStepVelocity(), 0);
                    break;
                    
                case PID_TYPE_RIGHT:
                    Pid_RightSetGains(gains[0], gains[1], gains[2]);
                    Cal_SetLeftRightVelocity(0, GetStepVelocity());
                    break;                    
            }
                    
            Ser_PutString("\r\nCalibrating ");
            Ser_PutString("\r\n");
            start_time = millis();
            
            break;

            
        case CAL_VALIDATE_STAGE:
            /* Hmmm, what can we do for validation of the PID 
             * 
             * Maybe we can spit out the encoder values (count/sec, meter/second) in a format conducive to plotting
             * 
             * */
            break;

    }

    return CAL_OK;
}

static uint8 Update(CAL_STAGE_TYPE stage, void *params)
{
    CAL_PID_PARAMS * p_pid_params = (CAL_PID_PARAMS *) params;
        
    if (millis() - start_time < p_pid_params->run_time)
    {
        return CAL_OK;
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
    
    Ser_PutString("\r\nPrinting PID calibration results\r\n");
    
    switch (p_pid_params->pid_type)
    {
        case PID_TYPE_LEFT:
            Pid_LeftGetGains(&gains[0], &gains[1], &gains[2]);
            PrintGains("Left PID", gains);
            break;
        
        case PID_TYPE_RIGHT:
            Pid_RightGetGains(&gains[0], &gains[1], &gains[2]);
            PrintGains("Right PID", gains);
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
}