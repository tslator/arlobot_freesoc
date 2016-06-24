#include "calpid.h"
#include "cal.h"
#include "serial.h"
#include "nvstore.h"
#include "pid.h"

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

static void DoLeftTwiddle()
{
    float gains[3];
    Twiddle(Pid_LeftStepInput, gains, STEP_INPUT, TIME_IN_MS);
    StoreLeftGains(gains);
}

static void DoRightTwiddle()
{
    float gains[3];
    Twiddle(Pid_RightStepInput, gains, STEP_INPUT, TIME_IN_MS);
    StoreRightGains(gains);
}

void PerformPidCalibration(uint8 verbose)
/* 
    Clear PID calibration bit
    Do twiddle on left PID
    Do twiddle on right PID
    Write gains to EEPROM
    Set PID calibration bit
    Update PID calibration bit in EEPROM
 */
{
    if (verbose)
    {
        Ser_PutString("Starting PID calibration ...\r\n");
    }

    DoLeftTwiddle();
    DoRightTwiddle();   
    
    if (verbose)
    {
        Ser_PutString("PID Calibration complete.\r\n");
    }
}
