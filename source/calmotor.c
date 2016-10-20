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

/* 
    Explanation of Motor Calibration using Count/Sec to PWM mapping:

    Calibration involves mapping count/sec values to PWM values.  Each motor is run (forward and backward) collecting
    an average count/sec for each PWM sample.  The results are stored in an structure (CAL_DATA_TYPE) that holds the
    count/sec averages, PWM samples, min/max count/sec and a scale factor.  There are four CAL_DATA_TYPE instances: one
    for left motor forward, left motor backward, right motor forward and right motor backward.  The calibration data is
    stored in NVRAM on the Psoc and pointers to the calibration data are passed to the motor module.
    
    Upon startup, the calibration data is made available to the motors via pointers to NVRAM (Note: the EEPROM component
    maps NVRAM to memory so there is no appreciable overhead in reading from NVRAM (or so I believe until proven otherwise)

 */

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <limits.h>
#include "calmotor.h"
#include "motor.h"
#include "pwm.h"
#include "nvstore.h"
#include "serial.h"
#include "encoder.h"
#include "time.h"
#include "utils.h"
#include "debug.h"
#include "pid.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define MAX_MOTOR_CAL_ITERATION (3)
#define VAL_LOWER_BOUND_PERCENTAGE (0.2)
#define VAL_UPPER_BOUND_PERCENTAGE (0.8)
#define VAL_NUM_PROFILE_DATA_POINTS (11)

#define VALIDATION_INTERATION_DONE (255)
#define MAX_CPS_ARRAY (51)

#define NUM_MOTOR_VAL_PARAMS (4)


/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/
#define LEFT_WHEEL_FORWARD(wheel, dir)   (wheel == WHEEL_LEFT && dir == DIR_FORWARD)
#define RIGHT_WHEEL_FORWARD(wheel, dir)  (wheel == WHEEL_RIGHT && dir == DIR_FORWARD)
#define LEFT_WHEEL_BACKWARD(wheel, dir)  (wheel == WHEEL_LEFT && dir == DIR_BACKWARD)
#define RIGHT_WHEEL_BACKWARD(wheel, dir) (wheel == WHEEL_RIGHT && dir == DIR_BACKWARD)



/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/

 typedef struct _motor_calibration
{
    SET_MOTOR_PWM_FUNC_TYPE set_pwm;
    GET_MOTOR_PWM_FUNC_TYPE get_pwm;
    RAMP_DOWN_PWM_FUNC_TYPE ramp_down;
    GET_ENCODER_FUNC_TYPE   get_mps;
    GET_ENCODER_FUNC_TYPE   get_cps;
    WHEEL_TYPE              wheel;
} MOTOR_CALIBRATION_TYPE;

typedef struct _pwm_params
{
    uint16 start;
    uint16 end;
    int16 step;
} PWM_PARAMS_TYPE;

typedef struct 
{
    char label[30]; 
    DIR_TYPE direction;
    MOTOR_CALIBRATION_TYPE *motor;
}VAL_MOTOR_PARAMS;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static int32         cal_cps_samples[CAL_NUM_SAMPLES];
static uint16        cal_pwm_samples[CAL_NUM_SAMPLES];
static int32         cal_cps_sum[CAL_NUM_SAMPLES];
static CAL_DATA_TYPE cal_data;

static float val_fwd_cps[VAL_NUM_PROFILE_DATA_POINTS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static float val_bwd_cps[VAL_NUM_PROFILE_DATA_POINTS] = {-0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0, -0.0};

/* Provides an implementation of the Calibration interface */
static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static CAL_MOTOR_PARAMS cal_motor_params = { 2000 };

static CALIBRATION_TYPE motor_calibration = {CAL_INIT_STATE, 
                                             CAL_CALIBRATE_STAGE,
                                             &cal_motor_params,
                                             Init, 
                                             Start, 
                                             Update, 
                                             Stop, 
                                             Results };

/* The PWM params: start, end, step are defined for each motor (left/right) and each direction (forward/backward)
   In general, the servo interface to the motors is defined as follows:
        1500 - stop
        2000 - full forward
        1000 - full reverse
 
   The orientation of the motors is such that the left more moves as above, but the right motor is rotated 180 degree
   such that:
        1500 - stop
        2000 - full reverse
        1000 - full forward
 
   The following is organized so that the count/sec samples are ordered numerically increase, i.e., lowest to highest.
   For the forward direction, the count/sec values range from 0 to Max Forward (a positive number).
   For the reverse direction, the count/sec values range from Max Revers (a negative number) to 0.
 
   Note: The ordering is a constraint of the binary search algorithm.
 
   Consequently, the organization below is:
       Left Wheel, Forward
            PWM runs from 1500 to 2000, Lowest CPS corresponds to 1500
            PWM array index runs from 0 to Max
            CPS array index runs from 0 to Max
    
       Left Wheel, Backward
            PWM runs from 1000 to 1500, Lowest CPS corresponds to 1000
            PWM array index runs from Max to 0
            CPS array index runs from Max to 0
    
       Right Wheel, Forward
            PWM runs from 1500 to 1000, Lowest CPS corresponds to 1500
            PWM array index runs from 0 to Max
            CPS array index runs from 0 to Max
    
       Right Wheel, Backward
            PWM runs from 2000 to 1500, Lowest CPS corresponds to 2000
            PWM array index runs from Max to 0
            CPS array index runs from Max to 0
 
   The above ensures that the pwm array and cps array is matched and ensures that the cps values are in ascending order
   so that linear interpolation works.
 */
PWM_PARAMS_TYPE pwm_params[2][2] = {{{LEFT_PWM_STOP, LEFT_PWM_FULL_FORWARD, LEFT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1)}, 
                                     {LEFT_PWM_FULL_BACKWARD, LEFT_PWM_STOP, LEFT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1)}
                                    }, 
                                    {
                                     {RIGHT_PWM_STOP, RIGHT_PWM_FULL_FORWARD, -(RIGHT_PWM_FORWARD_DOMAIN / (CAL_NUM_SAMPLES - 1))}, 
                                     {RIGHT_PWM_FULL_BACKWARD, RIGHT_PWM_STOP, -(RIGHT_PWM_BACKWARD_DOMAIN / (CAL_NUM_SAMPLES - 1))}
                                    }};

MOTOR_CALIBRATION_TYPE motor_cal[2] = 
    {{Motor_LeftSetPwm,
      Motor_LeftGetPwm,
      Motor_LeftRampDown,
      Encoder_LeftGetMeterPerSec,
      Encoder_LeftGetCntsPerSec,
      WHEEL_LEFT}, 

     {Motor_RightSetPwm,
      Motor_RightGetPwm,
      Motor_RightRampDown,
      Encoder_RightGetMeterPerSec,
      Encoder_RightGetCntsPerSec,
      WHEEL_RIGHT}};


VAL_MOTOR_PARAMS motor_val_params[4] = {{
                                        "left-forward", 
                                        DIR_FORWARD,
                                        &motor_cal[WHEEL_LEFT]
                                        }, 
                                        {
                                        "right-forward",
                                        DIR_FORWARD,
                                        &motor_cal[WHEEL_RIGHT]
                                        }, 
                                        {
                                        "left-backward",
                                        DIR_BACKWARD,
                                        &motor_cal[WHEEL_LEFT]
                                        }, 
                                        {
                                        "right-backward",
                                        DIR_BACKWARD,
                                        &motor_cal[WHEEL_RIGHT]
                                        }};
static uint8 motor_val_index;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Print Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: PrintAllMotorParams
 * Description: Prints the left/right, forward/backward count/sec and pwm calibration values
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void PrintAllMotorParams()
{
    Ser_PutString("\r\n");
    Cal_PrintSamples("Left-Backward", (int32 *) p_cal_eeprom->left_motor_bwd.cps_data, (uint16 *) p_cal_eeprom->left_motor_bwd.pwm_data);
    Cal_PrintSamples("Left-Forward", (int32 *) p_cal_eeprom->left_motor_fwd.cps_data, (uint16 *) p_cal_eeprom->left_motor_fwd.pwm_data);
    Cal_PrintSamples("Right-Backward", (int32 *) p_cal_eeprom->right_motor_bwd.cps_data, (uint16 *) p_cal_eeprom->right_motor_bwd.pwm_data);
    Cal_PrintSamples("Right-Forward", (int32 *) p_cal_eeprom->right_motor_fwd.cps_data, (uint16 *) p_cal_eeprom->right_motor_fwd.pwm_data);
}

/*---------------------------------------------------------------------------------------------------
 * Name: PrintWheelVelocity
 * Description: Prints the specified wheel velocity: count/sec, meter/sec, and pwm values
 * Parameters: label - string identifying the wheel 
 *             cps - count/sec value
 *             mps - meter/sec value
 *             pwm - pulse-width modulation value
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void PrintWheelVelocity(char *label, float cps, float mps, uint16 pwm)
{
    char cps_str[10];
    char mps_str[10];
    char calc_mps_str[10];
    char delta_mps_str[10];
    char output[64];

    float calc_mps = cps * METER_PER_COUNT;
    float delta_mps = calc_mps - mps;

    ftoa(cps, cps_str, 3);
    ftoa(mps, mps_str, 3);
    ftoa(calc_mps, calc_mps_str, 3);
    ftoa(delta_mps, delta_mps_str, 6);
    
    
    sprintf(output, "%s - cps: %s cmpd: %s mps: %s dmps: %s, pwm: %d\r\n", label, cps_str, calc_mps_str, mps_str, delta_mps_str, pwm);
    Ser_PutString(output);
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalculatePwmSamples
 * Description: Calculates an array of pwm values based on the specified wheel, and direction. 
 * Parameters: wheel - left/right wheel 
 *             dir - forward/backward direction
 *             pwm_samples - array of pwm values
 *             reverse_pwm - indicates the ordering within the pwm samples
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalculatePwmSamples(WHEEL_TYPE wheel, DIR_TYPE dir, uint16 *pwm_samples)
/* The pwm samples are filled in from lowest value to highest PWM value.  This ensures that the
   corresponding count/sec values (stored in a different array) are orders from lowest value to
   highest value, i.e., forward: 0 to max cps, reverse: -max cps to 0.
 */
{
    uint8 ii;
    uint16 pwm;
    uint16 pwm_start;
    //uint16 pwm_end;
    int16 pwm_step;

    pwm_start = pwm_params[wheel][dir].start;
    // Note: The code becomes more complicated to use pwm_end because sometimes the pwm is incrementing and sometimes
    // it is decrementing.  Ultimately, I chose to just loop over the number of sample points and make pwm_step a 
    // positive or negative number.  I will remove end at some point when I'm confident about how everything works.
    //pwm_end = pwm_params[wheel][dir].end;
    pwm_step = pwm_params[wheel][dir].step;
    
    for (ii =0, pwm = pwm_start; ii < CAL_NUM_SAMPLES; ++ii, pwm += pwm_step)
    {
        pwm_samples[ii] = pwm;
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: AddSamplesToSum
 * Description: Adds an array of values to a sum 
 * Parameters: cap_samples - array of count/sec samples
 *             cps_sum - resulting sum
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void AddSamplesToSum(int32 *cps_samples, int32 *cps_sum)
{
    uint8 ii;
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        cps_sum[ii] += cps_samples[ii];
    }
}
    
/*---------------------------------------------------------------------------------------------------
 * Name: CalculateAvgCpsSamples
 * Description: Calculates an average for each array entry
 * Parameters: cps_sum - array of count/sec sums
 *             num_runs - number of runs used to collect the sum
 *             avg_cps_samples - array of averages
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalculateAvgCpsSamples(int32 *cps_sum, uint8 num_runs, int32 *avg_cps_samples)
{
    uint8 ii;
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
    {
        avg_cps_samples[ii] = cps_sum[ii]/num_runs;
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalculateMinMaxCpsSample
 * Description: Calculates the min/max from an array of values
 * Parameters: samples - array of values
 *             num_samples - the number of samples in the array
 *             min - the minimum value of the array
 *             max - the maximum value of the array
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalculateMinMaxCpsSample(int32 *samples, int32 *min, int32 *max)
{
    uint8 ii;
    *min = INT_MAX;
    *max = INT_MIN;    
    
    for (ii = 0; ii < CAL_NUM_SAMPLES; ++ii)
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

/*---------------------------------------------------------------------------------------------------
 * Name: CollectCpsPwmSamples
 * Description: Collects count/sec values at a given pwm value
 * Parameters: set_pwm - motor set pwm function
 *             cnts_per_sec - encoder get count/sec function
 *             pwm - the pwm to be driven
 * Return: int32 - average cps
 * 
 *-------------------------------------------------------------------------------------------------*/
static int32 CalcCpsAvgAtPwm(MOTOR_CALIBRATION_TYPE *motor, uint16 pwm)
{
    #define NUM_CPS_AVG_ITERATIONS (50)
    #define NUM_SETTLING_ITERATIONS (5)
    uint8 ii;
    float cnts_per_sec_sum;
    float enc_cnts_per_sec;
    uint16 iterations;
    
    cnts_per_sec_sum = 0;
    
    iterations = NUM_CPS_AVG_ITERATIONS + NUM_SETTLING_ITERATIONS;

    motor->set_pwm(pwm);

    /* Note: The extra 5 iterations (5 * 10 = 50 ms) is settling time for the pwm change.
       The reason it is a part of the loop is to keep the encoder sampling active.  There
       are averages involved with measuring count/sec so we want the pipeline to be full.
     */
    for ( ii = 0; ii < iterations; ++ii)
    {
        Encoder_Update();
        if (ii >= NUM_SETTLING_ITERATIONS)
        {
            enc_cnts_per_sec = motor->get_cps();        
            cnts_per_sec_sum += enc_cnts_per_sec * CAL_SCALE_FACTOR;
        }
        CyDelay(10);
    }
    
    return (int32) cnts_per_sec_sum / NUM_CPS_AVG_ITERATIONS;
}

/*---------------------------------------------------------------------------------------------------
 * Name: CollectPwmCpsSamples
 * Description: 
 * Parameters: wheel - 
 *             reverse_pwm -
 *             num_avg_iter -
 *             pwm_samples -
 *             cps_samples - 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CollectCpsSamples(MOTOR_CALIBRATION_TYPE *motor, 
                              PWM_TYPE               *pwm_samples, 
                              int32                  *cps_samples,
                              uint8                  reverse)
{
    /* The count/sec samples must be placed into the array in numeric order, i.e., lower to higher,
       because of constraints imposed by the binary search algorithm.  In the case of forward
       motion, the cps values are 0 to Max Forward CPS (a positive number).  In the case of backward motion, the
       cps values are Max Backward CPS (a negative number) to 0.
     
       However, the motor must be driven from slow to fast.  Consequently, it the case for calculating
       cps for reverse motion, it is necessary to run the PWM from the last sample (1500) or motor stop
       to the first sample (1000) or motor max.
     
     */
    
    #define CALC_OFFSET(reverse, index)  (reverse ? CAL_NUM_SAMPLES - 1 - index : index)

    uint8 index;
    uint8 offset;
    
    motor->set_pwm(PWM_STOP);
    CyDelay(500);
    Encoder_Reset();

    for ( index = 0; index < CAL_NUM_SAMPLES; ++index)
    {
        offset = CALC_OFFSET(reverse, index);
        cps_samples[offset] = CalcCpsAvgAtPwm(motor, pwm_samples[offset]);
    }
    
    /* Ramp down the motor speed to be nice to motor */
    motor->ramp_down(1000);
    motor->set_pwm(PWM_STOP);
}

/*---------------------------------------------------------------------------------------------------
 * Name: StoreWheelSpeedSamples
 * Description: 
 * Parameters: wheel - 
 *             dir -
 *             cps_samples -
 *             pwm_samples - 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void StoreWheelSpeedCalibration(CAL_DATA_TYPE *p_cal_data, int32 *cps_samples, uint16 *pwm_samples)
{
    /* Note: cal_data is a module variable used to temporarily hold the calibration data before it
       is written to non-volatile storage.
     */
    cal_data.cps_scale = CAL_SCALE_FACTOR;
    CalculateMinMaxCpsSample(cps_samples, &cal_data.cps_min, &cal_data.cps_max);

    memcpy(&cal_data.cps_data[0], cps_samples, sizeof(cps_samples[0]) * CAL_NUM_SAMPLES);
    memcpy(&cal_data.pwm_data[0], pwm_samples, sizeof(pwm_samples[0]) * CAL_NUM_SAMPLES);

    /* Retrieve the non-volatile storage offset for the specified wheel and direction */
    uint16 offset = NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(p_cal_data);

    /* Write the calibration to non-volatile storage */
    Nvstore_WriteBytes((uint8 *) &cal_data, sizeof(cal_data), offset);
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalibrateWheelSpeed
 * Description: 
 * Parameters: wheel - 
 *             dir -
 *             num_runs -
 *             cps_samples -
 *             pwm_samples - 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalibrateWheelSpeed(WHEEL_TYPE wheel, DIR_TYPE dir)
/*
    calculate the PWM samples
        Note: the pwm samples depend on the wheel and direction
    for the number of runs
        collect cps samples for each pwm
        add the samples to the sum (for averaging)

    calculate the average cps for each pwm
 
    store the calibration values in non-volatile memory
*/
{
    uint8 run;
    MOTOR_CALIBRATION_TYPE *motor;
    CAL_DATA_TYPE *p_cal_data;
    uint8 reverse;

    motor = &motor_cal[wheel];

    CalculatePwmSamples(wheel, dir, cal_pwm_samples);
    
    /* The CPS values must be stored from lowest (most negative) to higest (most positive) which means the index used
       for storage must be reversed.
     */
    reverse = (dir == DIR_BACKWARD) ? 1 : 0;
    
    memset(cal_cps_sum, 0, sizeof(cal_cps_sum));
    
    for (run = 0; run < MAX_MOTOR_CAL_ITERATION; ++run)
    {
        CollectCpsSamples(motor, cal_pwm_samples, cal_cps_samples, reverse);
        AddSamplesToSum(cal_cps_samples, cal_cps_sum);
    }
    
    CalculateAvgCpsSamples(cal_cps_sum, MAX_MOTOR_CAL_ITERATION, cal_cps_samples);

    p_cal_data = WHEEL_DIR_TO_CAL_DATA[wheel][dir];
    StoreWheelSpeedCalibration(p_cal_data, cal_cps_samples, cal_pwm_samples);
}

static uint8 GetNextCps(DIR_TYPE dir, float *cps)
/* Return the next value in the array and increment the index
   Return 0 if the index is in range 0 to N-1
   Return 1 if the index rolls over
   Note: Allow rollover so that the index auto-initializes
 */
{
    static uint8 index = 0;
    
    if (index == VAL_NUM_PROFILE_DATA_POINTS)
    {
        index = 0;
        return 1;
    }
    
    switch (dir)
    {
        case DIR_FORWARD:
            *cps = val_fwd_cps[index];
            break;
            
        case DIR_BACKWARD:
            *cps = val_bwd_cps[index];
            break;
            
        default:
            *cps = 0.0;
            break;
    }
    
    index++;
    
    return 0;
}

static void SetNextVelocity(MOTOR_CALIBRATION_TYPE *motor, float cps)
{
    float mps = cps * METER_PER_COUNT;
    if (motor->wheel == WHEEL_LEFT)
    {
        Cal_SetLeftRightVelocity(mps, 0);
    }
    if (motor->wheel == WHEEL_RIGHT)
    {
        Cal_SetLeftRightVelocity(0, mps);
    }
}


static uint8 ValidateMotorCalibration(VAL_MOTOR_PARAMS *val_params, uint32 run_time)
{
    static uint8 running = FALSE;
    static uint32 start_time = 0;
    static float cps;
    uint8 result;
    MOTOR_CALIBRATION_TYPE *motor;

    motor = val_params->motor;

    if( !running )
    {
        Cal_SetLeftRightVelocity(0, 0);
        result = GetNextCps(val_params->direction, &cps);
        if( result )
        {
            motor->set_pwm(PWM_STOP);
            return CAL_COMPLETE;
        }
        
        SetNextVelocity(motor, cps);
        start_time = millis();
        running = TRUE;
    }

    if( running )
    {
        if ( millis() - start_time < run_time )
        {
            return CAL_OK;
        }
        PrintWheelVelocity(val_params->label, cps, motor->get_mps(), motor->get_pwm());
        
        /* Note: Make GetNextCps reset the index back to 0 when it reaches the end of the array
           so we don't have to explicitly reset the index.
         
           Consider doing the same for the PID -- auto reset, it's better that way
         
         */
        result = GetNextCps(val_params->direction, &cps);
        if( result )
        {
            running = FALSE;
            Cal_SetLeftRightVelocity(0, 0);
            return VALIDATION_INTERATION_DONE;
        }
        SetNextVelocity(motor, cps);
        start_time = millis();
        return CAL_OK;
    }
    
    return CAL_OK;
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
    params = params;

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nInitialize motor calibration\r\n");            
            ClearCalibrationStatusBit(CAL_MOTOR_BIT);
            break;
            
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nInitialize motor validation\r\n");

            Cal_CalcTriangularProfile(VAL_NUM_PROFILE_DATA_POINTS, 
                                      VAL_LOWER_BOUND_PERCENTAGE, 
                                      VAL_UPPER_BOUND_PERCENTAGE, 
                                      val_fwd_cps, 
                                      val_bwd_cps);
            
            /* Left/Right wheel validation uses the main loop so the PID must be enabled */
            Pid_Enable(TRUE);
            Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);         

            break;
    }
    
    return CAL_OK;    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Linear Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - motor validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Start(CAL_STAGE_TYPE stage, void *params)
{
    params = params;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nPerforming motor calibration\r\n");
            break;
        
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nPerforming motor validation\r\n");
            Cal_SetLeftRightVelocity(0, 0);            
            break;
    }
    
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: stage - the calibration/validation stage 
 *             params - motor validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Update(CAL_STAGE_TYPE stage, void *params)
{
    uint16 last_debug_control_enabled;

    params = params;
    
    last_debug_control_enabled = debug_control_enabled;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:

            /* Note: For calibration, Update is not called repeatedly from the main loop.
               Instead, there is a loop for each calibration cycle, e.g., left-forward,
               left-backward, right-forward, right-backward
             */
        
            Motor_SetPwm(PWM_STOP, PWM_STOP);

            debug_control_enabled = DEBUG_LEFT_ENCODER_ENABLE_BIT | DEBUG_LEFT_MOTOR_ENABLE_BIT;

            Ser_PutString("Left-Forward Calibration\r\n");
            CalibrateWheelSpeed(WHEEL_LEFT, DIR_FORWARD);
            
            Ser_PutString("Left-Backward Calibration\r\n");
            CalibrateWheelSpeed(WHEEL_LEFT, DIR_BACKWARD);
            
            debug_control_enabled = DEBUG_RIGHT_ENCODER_ENABLE_BIT | DEBUG_RIGHT_MOTOR_ENABLE_BIT;

            Ser_PutString("Right-Forward Calibration\r\n");
            CalibrateWheelSpeed(WHEEL_RIGHT, DIR_FORWARD);
            
            Ser_PutString("Right-Backward Calibration\r\n");
            CalibrateWheelSpeed(WHEEL_RIGHT, DIR_BACKWARD);

            debug_control_enabled = last_debug_control_enabled;

            return CAL_COMPLETE;
            break;
            
        case CAL_VALIDATE_STAGE:
            {
                CAL_MOTOR_PARAMS *p_motor_params = (CAL_MOTOR_PARAMS *) params;
                VAL_MOTOR_PARAMS *val_params = &motor_val_params[motor_val_index];
    
                uint8 result = ValidateMotorCalibration(val_params, p_motor_params->run_time);
                if( result == VALIDATION_INTERATION_DONE )
                {
                    motor_val_index++;
                    if( motor_val_index == NUM_MOTOR_VAL_PARAMS )
                    {
                        Cal_SetLeftRightVelocity(0, 0);            
                        return CAL_COMPLETE;
                    }
                }
                return CAL_OK;
            }
            break;
    }
    
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Stop
 * Description: Calibration/Validation interface Stop function.  Called to stop validation.
 * Parameters: stage - the calibration/validation stage
 *             params - motor validation parameters 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Stop(CAL_STAGE_TYPE stage, void *params)
{
    params = params;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("Motor calibration complete\r\n");
            SetCalibrationStatusBit(CAL_MOTOR_BIT);
            break;
        case CAL_VALIDATE_STAGE:
            Ser_PutString("Motor validation complete\r\n");
            Cal_SetLeftRightVelocity(0, 0);
            Pid_RestoreLeftRightTarget();
            break;
    }
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Results
 * Description: Calibration/Validation interface Results function.  Called to display calibration/ 
 *              validation results. 
 * Parameters: stage - the calibration/validation stage 
 *             params - motor calibration/validation parameters 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Results(CAL_STAGE_TYPE stage, void *params)
{
    params = params;
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nPrinting motor calibration results\r\n");
            PrintAllMotorParams();
            break;
        
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nPrinting motor validation results\r\n");
            break;
    }
    return CAL_OK;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Module Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/
void CalMotor_Init()
{
    WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_FORWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_fwd;
    WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_BACKWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_bwd;
    WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_FORWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_fwd;
    WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_BACKWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_bwd;
    
    CalMotor_Calibration = &motor_calibration;
    CalMotor_Validation = &motor_calibration;    
}

