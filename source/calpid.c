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

#define STEP_VELOCITY_PERCENT  (0.8)    // 80% of maximum velocity

static float LeftTarget()
{
    return left_cmd_speed;
}

static float RightTarget()
{
    return right_cmd_speed;
}

static float StepImpulse(GET_ENCODER_TYPE get_encoder, float velocity, uint32 sample_delay, uint32 time_in_ms)
/* Apply a step input velocity to the motor and measure/print the encoder, pid and odometry generated */
{
    float vel_error_sum = 0;
    uint32 samples = 0;
    uint32 start_time;

    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);

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
    
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();
    
    Pid_LeftSetGains(gains[0], gains[1], gains[2]);    
    
    float result = StepImpulse(Encoder_LeftGetMeterPerSec, velocity, sample_delay, run_time);

    Pid_LeftGetGains(&gains[0], &gains[1], &gains[2]);
    OutputGains("Left", gains);
    
    return result;
}

static float RightStepInput(float *gains, float velocity, uint32 sample_delay, uint32 run_time)
{
    OutputGains("Right", gains);    

    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();
    
    Pid_RightSetGains(gains[0], gains[1], gains[2]);    
    
    float result = StepImpulse(Encoder_RightGetMeterPerSec, velocity, sample_delay, run_time);
    
    Pid_RightGetGains(&gains[0], &gains[1], &gains[2]);    
    OutputGains("Right", gains);    
    
    return result;    
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

    left_fwd_max = abs(p_cal_eeprom->left_motor_fwd.cps_max/p_cal_eeprom->left_motor_fwd.cps_scale);
    left_bwd_max = abs(p_cal_eeprom->left_motor_bwd.cps_min/p_cal_eeprom->left_motor_bwd.cps_scale);
    right_fwd_max = abs(p_cal_eeprom->right_motor_fwd.cps_max/p_cal_eeprom->right_motor_fwd.cps_scale);
    right_bwd_max = abs(p_cal_eeprom->right_motor_bwd.cps_min/p_cal_eeprom->right_motor_bwd.cps_scale);
    
    left_max = min(left_fwd_max, left_bwd_max);
    right_max = min(right_fwd_max, right_bwd_max);
    
    max_cps = min(left_max, right_max);
    
    return (float) max_cps * METER_PER_COUNT * STEP_VELOCITY_PERCENT;
}

static void DoLeftManual(float *gains)
{
    debug_control_enabled = DEBUG_LEFT_PID_ENABLE_BIT;// | DEBUG_LEFT_ENCODER_ENABLE_BIT;
    
    
    left_cmd_speed = GetStepVelocity();
    right_cmd_speed = 0;

    float error = LeftStepInput(gains, left_cmd_speed, 0, 5000);
    
    left_cmd_speed = 0;
    right_cmd_speed = 0;

}

static void DoRightManual(float *gains)
{
    debug_control_enabled = DEBUG_RIGHT_PID_ENABLE_BIT;// | DEBUG_RIGHT_ENCODER_ENABLE_BIT;
    
    left_cmd_speed = 0;
    right_cmd_speed = GetStepVelocity();

    float error = RightStepInput(gains, right_cmd_speed, 0, 5000);

    left_cmd_speed = 0;
    right_cmd_speed = 0;
}

void CalibrateLeftPid(float *gains)
{
    Ser_PutString("\r\nLeft PID calibration\r\n");
    
    Pid_SetLeftRightTarget(LeftTarget, RightTarget);
    
    uint16 old_debug_control_enabled = debug_control_enabled;
    debug_control_enabled = DEBUG_LEFT_PID_ENABLE_BIT;
    
    DoLeftManual(gains);
    StoreLeftGains(gains);

    debug_control_enabled = old_debug_control_enabled;    
    Pid_SetLeftRightTarget(Control_LeftGetCmdVelocity, Control_RightGetCmdVelocity);
    
    Ser_PutString("Left PID calibration complete\r\n");

    
}

void CalibrateRightPid(float *gains)
{
    Ser_PutString("\r\nRight PID calibration\r\n");
    
    Pid_SetLeftRightTarget(LeftTarget, RightTarget);
    
    uint16 old_debug_control_enabled = debug_control_enabled;
    debug_control_enabled = DEBUG_RIGHT_PID_ENABLE_BIT;
    
    DoRightManual(gains);   
    StoreRightGains(gains);
    
    debug_control_enabled = old_debug_control_enabled;
    Pid_SetLeftRightTarget(Control_LeftGetCmdVelocity, Control_RightGetCmdVelocity);
    
    Ser_PutString("Right PID calibration complete\r\n");
}

static void WriteSpeeds(float left, float right, float delta_speed)
{
    char left_str[10];
    char right_str[10];
    char delta_speed_str[10];
    char output[64];
    
    ftoa(left, left_str, 3);
    ftoa(right, right_str, 3);
    ftoa(delta_speed, delta_speed_str, 3);
    
    sprintf(output, "%s %s %s\r\n", left_str, right_str, delta_speed_str);
    Ser_PutString(output);
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
    float left_speed;
    float right_speed;
    uint8 ii;
    
    Pid_SetLeftRightTarget(LeftTarget, RightTarget);
    
    uint16 old_debug_control_enabled = debug_control_enabled;
    
    debug_control_enabled = 0;//DEBUG_LEFT_ENCODER_ENABLE_BIT | DEBUG_RIGHT_ENCODER_ENABLE_BIT | DEBUG_LEFT_PID_ENABLE_BIT | DEBUG_RIGHT_PID_ENABLE_BIT | DEBUG_ODOM_ENABLE_BIT;

    left_cmd_speed = 0;
    right_cmd_speed = 0;
    
    float delta_speed = 0.150 / 10;
    
    /* Ramp up to the velocity */
    for (ii = 0; ii < 10; ++ii)
    {
        left_cmd_speed += delta_speed;
        right_cmd_speed += delta_speed;
        uint32 start_time = millis();
        
        while (millis() - start_time < 200)
        {            
            Encoder_Update();
            Pid_Update();
            Odom_Update();
            left_speed = Encoder_LeftGetMeterPerSec();
            right_speed = Encoder_RightGetMeterPerSec();                
            WriteSpeeds(left_speed, right_speed, left_speed - right_speed);
        }
    }
    
    left_cmd_speed = 0.150;
    right_cmd_speed = 0.150;
    
    uint32 start_time = millis();
    
    while (millis() - start_time < 5000)
    {
        Encoder_Update();
        Pid_Update();
        Odom_Update();
        
        left_speed = Encoder_LeftGetMeterPerSec();
        right_speed = Encoder_RightGetMeterPerSec();                
        WriteSpeeds(left_speed, right_speed, left_speed - right_speed);
    }
    
    /* Ramp down to 0 */
    for (ii = 0; ii < 10; ++ii)
    {
        left_cmd_speed -= delta_speed;
        right_cmd_speed -= delta_speed;
        uint32 start_time = millis();
        
        while (millis() - start_time < 200)
        {            
            Encoder_Update();
            Pid_Update();
            Odom_Update();
            left_speed = Encoder_LeftGetMeterPerSec();
            right_speed = Encoder_RightGetMeterPerSec();                
            WriteSpeeds(left_speed, right_speed, left_speed - right_speed);
        }
    }
    left_cmd_speed = 0;
    right_cmd_speed = 0;
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    
    Pid_SetLeftRightTarget(Control_LeftGetCmdVelocity, Control_RightGetCmdVelocity);
    debug_control_enabled = old_debug_control_enabled;
}

#ifdef TWIDDLE
#define SUM(g)      (g[0] + g[1] + g[2])
#define TOLERANCE   (0.2)
#define TIME_IN_MS  (2000)  // millisecond
#define NUM_GAINS   (3)


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
#endif