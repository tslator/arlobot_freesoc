#include <stdio.h>
#include "calcps2pwm.h"
#include "motor.h"
#include "pwm.h"
#include "nvstore.h"
#include "serial.h"

/*
    Explantation of Count/Sec to PWM Calibration:

    Calibration involves mapping count/sec values to PWM values.  Each motor is run (forward and backward) collecting
    an average count/sec for each PWM sample.  The results are stored in an structure (CAL_DATA_TYPE) that holds the
    count/sec averages, PWM samples, min/max count/sec and a scale factor.  There are four CAL_DATA_TYPE instances: one
    for left motor forward, left motor backward, right motor forward and right motor backward.  The calibration data is
    stored in NVRAM on the Psoc and pointers to the calibration data are passed to the motor module.
    
    Upon startup, the calibration data is made available to the motors via pointers to NVRAM (Note: the EEPROM component
    maps NVRAM to memory so there is no appreciable overhead in reading from NVRAM (or so I believe until proven otherwise)

 */


static int32 cps_samples[CAL_NUM_SAMPLES];
static uint16 pwm_samples[CAL_NUM_SAMPLES];
static int32 cps_sum[CAL_NUM_SAMPLES];
static CAL_DATA_TYPE cal_data;

static void CalculatePwmSamples(WHEEL_TYPE wheel, DIR_TYPE dir, uint16 *pwm_samples, uint8 *reverse_pwm)
{
    /*
        if wheel is left
            if dir is forward
                pwm start = 1500
                pwm end = 2000
                pwm step = (2000 - 1500) / num_samples
            if dir is backward
                pwm start = 1000
                pwm end = 1500
                pwm step = (1500 - 1000) / num_samples
    
        if wheel is right
            if dir is forward
                pwm start = 1000
                pwm end = 1500
                pwm step = (1500 - 1000) / num_samples
            if dir is backward
                pwm start = 1500
                pwm end = 2000
                pwm step = (2000 - 1500) / num_samples
    */
    
    uint16 pwm;
    uint16 pwm_start;
    uint16 pwm_end;
    uint16 pwm_step;
    
    if (wheel == LEFT_WHEEL)
    {
        if (dir == FORWARD_DIR)
        {
            pwm_start = LEFT_PWM_STOP;
            pwm_end = LEFT_PWM_FULL_FORWARD;
            pwm_step = LEFT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1);
            *reverse_pwm = 0;
        }
        else
        {
            pwm_start = LEFT_PWM_FULL_BACKWARD;
            pwm_end = LEFT_PWM_STOP;
            pwm_step = LEFT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1);
            *reverse_pwm = 1;
        }

        uint8 ii;
        for (ii = 0, pwm = pwm_start; pwm <= pwm_end; ++ii, pwm += pwm_step)
        {
            pwm_samples[ii] = pwm;
        }
    }
    else
    {
        if (dir == FORWARD_DIR)
        {
            pwm_start = RIGHT_PWM_STOP;
            pwm_end = RIGHT_PWM_FULL_FORWARD;
            pwm_step = RIGHT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1);
            *reverse_pwm = 0;
        }
        else
        {
            pwm_start = RIGHT_PWM_FULL_BACKWARD;
            pwm_end = RIGHT_PWM_STOP;
            pwm_step = RIGHT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1);
            *reverse_pwm = 1;
        }
        uint8 ii;
        for (ii = 0, pwm = pwm_start; pwm >= pwm_end; ++ii, pwm -= pwm_step)
        {
            pwm_samples[ii] = pwm;
        }
        
    }
}

static void AddSamplesToSum(int32 *cps_samples, int32 *cps_sum)
{
    uint8 ii;
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        cps_sum[ii] += cps_samples[ii];
    }
}
    
static void CalculateAvgCpsSamples(int32 *cps_sum, uint8 num_runs, int32 *avg_cps_samples)
{
    uint8 ii;
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        avg_cps_samples[ii] = cps_sum[ii]/num_runs;
    }
}

static void CalculateMinMaxCpsSample(int32 *samples, uint8 num_samples, int32 *min, int32 *max)
{
    uint8 ii;
    *min = 65535;
    *max = -65535;
    
    for (ii = 0; ii < num_samples; ++ii)
    {
        if (samples[ii] > *max)
        {
            *max = samples[ii];
        }
        if (samples[ii] < *min)
        {
            *min = samples[ii];
        }
    }
}

static void CalibrateWheelSpeed(WHEEL_TYPE wheel, DIR_TYPE dir, uint8 num_runs, int32 *cps_samples, uint16 *pwm_samples)
/*
    calculate the PWM samples
        Note: the pwm samples depend on the wheel and direction
    for the number of runs
        collect cps-to-pwm samples
        add the samples to the sum (for averaging)

    calculate the average cps for each pwm
*/
{
    uint8 run;
    uint8 num_avg_iter = 10;
    uint8 reverse_pwm;
    
    
    CalculatePwmSamples(wheel, dir, pwm_samples, &reverse_pwm);

    memset(cps_sum, 0, CAL_NUM_SAMPLES * sizeof(int32));
    
    for (run = 0; run < num_runs; ++run)
    {
        Motor_CollectPwmCpsSamples(wheel, reverse_pwm, num_avg_iter, pwm_samples, cps_samples);
        AddSamplesToSum(cps_samples, cps_sum);
    }
    
    CalculateAvgCpsSamples(cps_sum, num_runs, cps_samples);
}

static void StoreWheelSpeedSamples(WHEEL_TYPE wheel, DIR_TYPE dir, int32 *cps_samples, uint16 *pwm_samples)
{
    CalculateMinMaxCpsSample(cps_samples, CAL_NUM_SAMPLES, &cal_data.cps_min, &cal_data.cps_max);
    cal_data.cps_scale = CAL_SCALE_FACTOR;
    memcpy(&cal_data.cps_data[0], cps_samples, sizeof(cps_samples[0]) * CAL_NUM_SAMPLES);
    memcpy(&cal_data.pwm_data[0], pwm_samples, sizeof(pwm_samples[0]) * CAL_NUM_SAMPLES);

    CAL_DATA_TYPE *p_cal_data;
    if (wheel == LEFT_WHEEL)
    {
        if (dir == FORWARD_DIR)
        {
            p_cal_data = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_fwd;
        }
        else
        {
            p_cal_data = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_bwd;
        }
    }
    else
    {
        if (dir == FORWARD_DIR)
        {
            p_cal_data = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_fwd;
        }
        else
        {
            p_cal_data = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_bwd;
        }
    }
    
    uint16 offset = NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(p_cal_data);
    Nvstore_WriteBytes((uint8 *) &cal_data, sizeof(cal_data), offset);
}

static void OutputSamples(char *label, int32 *cps_samples, uint16 *pwm_samples)
{
    uint8 ii;
    char  buffer[100];
    char label_str[20];
    
    sprintf(label_str, "%s\r\n", label);
    Ser_PutString(label_str);
    
    for (ii = 0; ii < CAL_NUM_SAMPLES - 1; ++ii)
    {
        sprintf(buffer, "%ld -> %d,", cps_samples[ii], pwm_samples[ii]);
        Ser_PutString(buffer);
    }
    sprintf(buffer, "%ld -> %d\r\n\r\n", cps_samples[ii], pwm_samples[ii]);
    Ser_PutString(buffer);
}

void PerformCountPerSecToPwmCalibration(uint8 verbose)
/* 
    Clear Count/Sec to PWM calibration bit
    Perform left/right forward/reverse calibration
    If verbose then write results to serial port
    Store calibration data to EEPROM
    Set Count/Sec to PWM calibration bit
    Update Count/Sec to PWM calibration bit in EEPROM
 */
{
    if (verbose)
    {
        Ser_PutString("Starting Count/Sec To PWM calibration ...\r\n");
    }
    
    CalibrateWheelSpeed(LEFT_WHEEL, FORWARD_DIR, 5, cps_samples, pwm_samples);
    StoreWheelSpeedSamples(LEFT_WHEEL, FORWARD_DIR, cps_samples, pwm_samples);
    if (verbose)
    {
        OutputSamples("Left-Forward", cps_samples, pwm_samples);
    }

    CalibrateWheelSpeed(LEFT_WHEEL, BACKWARD_DIR, 5, cps_samples, pwm_samples);
    StoreWheelSpeedSamples(LEFT_WHEEL, BACKWARD_DIR, cps_samples, pwm_samples);
    if (verbose)
    {
        OutputSamples("Left-Backward", cps_samples, pwm_samples);
    }

    CalibrateWheelSpeed(RIGHT_WHEEL, FORWARD_DIR, 5, cps_samples, pwm_samples);
    StoreWheelSpeedSamples(RIGHT_WHEEL, FORWARD_DIR, cps_samples, pwm_samples);
    if (verbose)
    {
        OutputSamples("Right-Forward", cps_samples, pwm_samples);
    }

    CalibrateWheelSpeed(RIGHT_WHEEL, BACKWARD_DIR, 5, cps_samples, pwm_samples);
    StoreWheelSpeedSamples(RIGHT_WHEEL, BACKWARD_DIR, cps_samples, pwm_samples);
    if (verbose)
    {
        OutputSamples("Right-Backward", cps_samples, pwm_samples);
    }

    Motor_LeftSetCalibration((CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_fwd, (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_bwd);
    Motor_RightSetCalibration((CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_fwd, (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_bwd);
    
    if (verbose)
    {
        Ser_PutString("Count/Sec to PWM Calibration complete.\r\n");
    }
}
