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

extern volatile CAL_EEPROM_TYPE *p_cal_eeprom;

static float left_cmd_speed;
static float right_cmd_speed;


typedef float (*RUN_TYPE)(float *gains, float velocity, uint32 sample_delay, uint32 run_time);

#define SUM(g)      (g[0] + g[1] + g[2])
#define TOLERANCE   (0.2)
#define STEP_INPUT  (225)   // counts/sec
#define TIME_IN_MS  (2000)  // millisecond
#define NUM_GAINS   (3)

static float LeftTarget()
{
    return left_cmd_speed;
}

static float RightTarget()
{
    return right_cmd_speed;
}

static float StepImpulse(GET_ENCODER_TYPE get_encoder, float velocity, uint32 sample_delay, uint32 time_in_ms)
/*
    Apply the velocity to the motor
    Update encoder and pid update
    Quit after 5 seconds
 */
{
    float vel_error_sum = 0;
    uint32 samples = 0;
    uint32 start_time;
    
    start_time = millis();
    while (millis() - start_time < time_in_ms)
    {
        Encoder_Update();
        Pid_Update();
        Odom_Update();
        
        if (millis() - start_time > sample_delay)
        {
            float error = velocity - get_encoder();
            vel_error_sum += error * error;
            samples++;
        }
    }

    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    
    return vel_error_sum / samples;
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

static void Twiddle(RUN_TYPE run, float *gains, float velocity, uint32 run_time)
{
    float d_gains[NUM_GAINS] = {1.0, 1.0, 1.0};
    float best_error;
    uint8 ii;
    float err;
    
    gains[0] = gains[1] = gains[2] = 0.0;
    
    best_error = run(gains, velocity, 1000, run_time);
    
    while (SUM(d_gains) > TOLERANCE)
    {
        for (ii = 0; ii < NUM_GAINS; ++ii)
        {
            gains[ii] += d_gains[ii];
            err = run(gains, velocity, 1000, run_time);
            
            if (err < best_error)
            {
                best_error = err;
                d_gains[ii] *= 1.1;
            }
            else
            {
                gains[ii] -= 2.0 * d_gains[ii];
                err = run(gains, velocity, 1000, run_time);
                
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

static float LeftStepInput(float *gains, float velocity, uint32 sample_delay, uint32 run_time)
{
    OutputGains("Left", gains);    
    Pid_LeftSetGains(gains[0], gains[1], gains[2]);    
    
    return StepImpulse(Encoder_LeftGetMeterPerSec, velocity, sample_delay, run_time);
}

static float RightStepInput(float *gains, float velocity, uint32 sample_delay, uint32 run_time)
{
    OutputGains("Right", gains);    
    Pid_RightSetGains(gains[0], gains[1], gains[2]);    
    
    return StepImpulse(Encoder_RightGetMeterPerSec, velocity, sample_delay, run_time);
}

static void DoLeftTwiddle()
{
    float gains[3];
    Twiddle(LeftStepInput, gains, STEP_INPUT, TIME_IN_MS);
    StoreLeftGains(gains);
}

static void DoRightTwiddle()
{
    float gains[3];
    Twiddle(RightStepInput, gains, STEP_INPUT, TIME_IN_MS);
    StoreRightGains(gains);
}

static void DoManual(RUN_TYPE run, float *gains, float step_velocity, uint32 sample_delay, uint32 run_time)
{
    /* Perform the step input and dump the velocity to serial port for plotting */
    
    run(gains, step_velocity, sample_delay, run_time);
    
}

static void DoLeftManual(float *gains)
{
    debug_control_enabled = DEBUG_LEFT_PID_ENABLE_BIT;
    
    
    left_cmd_speed = 0.745;
    right_cmd_speed = 0;

    DoManual(LeftStepInput, gains, 0.745, 0, 5000);
    
    left_cmd_speed = 0;
    right_cmd_speed = 0;

}

static void DoRightManual(float *gains)
{
    debug_control_enabled = DEBUG_RIGHT_PID_ENABLE_BIT;
    left_cmd_speed = 0;
    right_cmd_speed = 0.745;

    DoManual(RightStepInput, gains, 0.745, 0, 5000);

    left_cmd_speed = 0;
    right_cmd_speed = 0;
}

void CalibratePidManual(float *gains)
{
    Ser_PutString("\r\nPerforming pid manual calibration \r\n");
    
    Pid_SetLeftRightTarget(LeftTarget, RightTarget);
    
    uint16 old_debug_control_enabled = debug_control_enabled;
    Ser_PutString("Left PID calibration\r\n");
    debug_control_enabled = DEBUG_LEFT_PID_ENABLE_BIT;
    DoLeftManual(gains);

    //Ser_PutString("Right PID calibration\r\n");
    //debug_control_enabled = DEBUG_RIGHT_PID_ENABLE_BIT;
    //DoRightManual(gains);   
    debug_control_enabled = old_debug_control_enabled;
    Ser_PutString("PID manual calibration complete\r\n");

    
}

void CalibratePidAuto()
/* 
    Clear PID calibration bit
    Do twiddle on left PID
    Do twiddle on right PID
    Write gains to EEPROM
    Set PID calibration bit
    Update PID calibration bit in EEPROM
 */
{
    Ser_PutString("\r\nPerforming pid auto calibration (using TWIDDLE)\r\n");
    uint16 old_debug_control_enabled = debug_control_enabled;
    Ser_PutString("Left PID calibration\r\n");
    debug_control_enabled = DEBUG_LEFT_PID_ENABLE_BIT;
    DoLeftTwiddle();
    Ser_PutString("Right PID calibration\r\n");
    debug_control_enabled = DEBUG_RIGHT_PID_ENABLE_BIT;
    DoRightTwiddle();   
    debug_control_enabled = old_debug_control_enabled;
    Ser_PutString("PID auto calibration complete\r\n");
}

void ValidatePid()
{
    /* 
        Validates the PID settings
    
        What does it mean to validate the PID?
            - Ensure that both motors operate at the commanded speed
        What is the motors have different actual speeds for the same commanded speed?
            - Where can we apply a bias to correct this?
        If the motor velocities are correct why would this be a problem?
            - Could response time be a factor here?  I.E., one motor takes longer to reduce velocity error?
        Maybe there is another parameter to PID calibration to ensure that they have comparable response times?
            
    
     */
    char lcs_str[10];
    char rcs_str[10];
    char ls_str[10];
    char rs_str[10];
    char output[64];

    float left_speed;
    float right_speed;
    
    Pid_SetLeftRightTarget(LeftTarget, RightTarget);
    left_cmd_speed = 0.150;
    right_cmd_speed = 0.150;
    left_speed = 0;
    right_speed = 0;
    
    uint32 start_time = millis();
    
    while (millis() - start_time < 5000)
    {
        Encoder_Update();
        Pid_Update();
        Odom_Update();
        
        left_speed = Encoder_LeftGetMeterPerSec();
        right_speed = Encoder_RightGetMeterPerSec();
    }

    ftoa(left_cmd_speed, lcs_str, 3);
    ftoa(right_cmd_speed, rcs_str, 3);
    ftoa(left_speed, ls_str, 3);
    ftoa(right_speed, rs_str, 3);
    
    left_cmd_speed = 0;
    right_cmd_speed = 0;
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    Pid_SetLeftRightTarget(Control_LeftGetCmdVelocity, Control_RightGetCmdVelocity);
        
    /* Print the commanded and actual left/right speeds */
    sprintf(output, "\r\nlcs: %s, ls: %s, rcs: %s, rs: %s\r\n", lcs_str, ls_str, rcs_str, rs_str);
    Ser_PutString(output);
    
    
}
