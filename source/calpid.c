#include <stdio.h>
#include "calpid.h"
#include "cal.h"
#include "serial.h"
#include "nvstore.h"
#include "pid.h"
#include "utils.h"

extern volatile CAL_EEPROM_TYPE *p_cal_eeprom;

typedef float (*RUN_TYPE)(float *gains, float velocity, uint32 run_time);

#define SUM(g)      (g[0] + g[1] + g[2])
#define TOLERANCE   (0.2)
#define STEP_INPUT  (225)   // counts/sec
#define TIME_IN_MS  (2000)  // millisecond
#define NUM_GAINS   (3)

static void Twiddle(RUN_TYPE run, float *gains, float velocity, uint32 run_time)
{
    float d_gains[NUM_GAINS] = {1.0, 1.0, 1.0};
    float best_error;
    uint8 ii;
    float err;
    
    gains[0] = gains[1] = gains[2] = 0.0;
    
    best_error = run(gains, velocity, run_time);
    
    while (SUM(d_gains) > TOLERANCE)
    {
        for (ii = 0; ii < NUM_GAINS; ++ii)
        {
            gains[ii] += d_gains[ii];
            err = run(gains, velocity, run_time);
            
            if (err < best_error)
            {
                best_error = err;
                d_gains[ii] *= 1.1;
            }
            else
            {
                gains[ii] -= 2.0 * d_gains[ii];
                err = run(gains, velocity, run_time);
                
                if (err < best_error)
                {
                    best_error = err;
                    d_gains[ii] *= 1.1;
                }
                else
                {
                    gains[ii] += d_gains[ii];
                    d_gains[ii] *= 0.9;
                }
            }
        }        
    }    
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

static void OutputGains(char *label, float *gains)
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

static void DoLeftTwiddle()
{
    float gains[3];
    Twiddle(Pid_LeftStepInput, gains, STEP_INPUT, TIME_IN_MS);
    StoreLeftGains(gains);
    OutputGains("Left", gains);
}

static void DoRightTwiddle()
{
    float gains[3];
    Twiddle(Pid_RightStepInput, gains, STEP_INPUT, TIME_IN_MS);
    StoreRightGains(gains);
    OutputGains("Right", gains);
}

void PerformPidCalibration()
/* 
    Clear PID calibration bit
    Do twiddle on left PID
    Do twiddle on right PID
    Write gains to EEPROM
    Set PID calibration bit
    Update PID calibration bit in EEPROM
 */
{
    Ser_PutString("\r\nPerforming pid calibration\r\n");

    Ser_PutString("Left PID calibration\r\n");
    DoLeftTwiddle();
    Ser_PutString("Right PID calibration\r\n");
    DoRightTwiddle();   
    
    Ser_PutString("PID calibration complete\r\n");
}
