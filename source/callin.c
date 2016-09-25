#include <math.h>
#include "callin.h"
#include "odom.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "time.h"
#include "utils.h"
#include "serial.h"
#include "nvstore.h"
#include "debug.h"
#include "pwm.h"

/* The purpose of linear validation will be move the robot in the forward/backward direction,
   capture the odometry, and compare the results with the expected distance traveled.  The
   goal is to see the robot moving in a straight line.
 */

#define LINEAR_BIAS_DISTANCE (1.0)
#define LINEAR_BIAS_VELOCITY (0.150)
#define LINEAR_BIAS_TOLERANCE (0.001)
#define LINEAR_ACCELERATION (0.067)
#define RAMP_UP_DIST        (LINEAR_BIAS_DISTANCE / 3.0)

/* Note: The Encoder and PID sample rates are 20 Hz.  The sample rate for updating the linear velocity must be less 
         than those sample rates, i.e., 10 Hz.
*/
#define CALLIN_SAMPLE_RATE (PID_SAMPLE_RATE / 2)        
#define CALLIN_SAMPLE_TIME_MS  SAMPLE_TIME_MS(CALLIN_SAMPLE_RATE)

static CAL_LIN_PARAMS linear_params = {DIR_FORWARD, 
                                       10000, 
                                       LINEAR_BIAS_DISTANCE, 
                                       LINEAR_BIAS_VELOCITY};

static uint32 start_time;
static uint16 old_debug_control_enabled;

static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static float left_cmd_velocity;
static float right_cmd_velocity;
static uint32 last_time;

static CALIBRATION_TYPE linear_calibration = {CAL_INIT_STATE,
                                              CAL_VALIDATE_STAGE,
                                              &linear_params,
                                              Init,
                                              Start,
                                              Update,
                                              Stop,
                                              Results};



static uint8 Init(CAL_STAGE_TYPE stage, void *params)
{
    CAL_LIN_PARAMS *p_lin_params = (CAL_LIN_PARAMS *)params;
    char banner[64];

    switch (stage)
    {
        case CAL_VALIDATE_STAGE:
            sprintf(banner, "\r\n%s Linear validation\r\n", p_lin_params->direction == DIR_FORWARD ? "Forward" : "Backward");
            Ser_PutString(banner);
            Cal_SetLeftRightVelocity(0, 0);
            Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);

            old_debug_control_enabled = debug_control_enabled;

            debug_control_enabled = DEBUG_ODOM_ENABLE_BIT | DEBUG_LEFT_PID_ENABLE_BIT | DEBUG_LEFT_ENCODER_ENABLE_BIT | DEBUG_RIGHT_PID_ENABLE_BIT | DEBUG_RIGHT_ENCODER_ENABLE_BIT;

            break;

        default:
            break;
    }


    return CAL_OK;
}

static uint8 Start(CAL_STAGE_TYPE stage, void *params)
{
    CAL_LIN_PARAMS *p_lin_params = (CAL_LIN_PARAMS *) params;

    switch (stage)
    {
        case CAL_VALIDATE_STAGE:
            Ser_PutString("Linear Validation Start\r\n");
            Pid_Enable(TRUE);            
            Encoder_Reset();
            Pid_Reset();
            Odom_Reset();

            float velocity = p_lin_params->mps;
            if( p_lin_params->direction == DIR_BACKWARD )
            {
                velocity = -velocity;
            }

            Cal_SetLeftRightVelocity(velocity, velocity);

            Ser_PutString("\r\nValidating ");
            Ser_PutString("\r\n");
            start_time = millis();

            break;

        default:
            break;
    }

    return CAL_OK;
}

static uint8 Update(CAL_STAGE_TYPE stage, void *params)
{
    CAL_LIN_PARAMS * p_lin_params = (CAL_LIN_PARAMS *) params;

    switch (stage)
    {
        case CAL_VALIDATE_STAGE:
            if (millis() - start_time < p_lin_params->run_time)
            {
                float left_dist = abs(Encoder_LeftGetDist());
                float right_dist = abs(Encoder_RightGetDist());

                if ( left_dist < p_lin_params->distance && right_dist < p_lin_params->distance )
                {
                    return CAL_OK;
                }

                return CAL_COMPLETE;
            }
            Ser_PutString("\r\nRun time expired\r\n");
            break;

        default:
            break;
    }

    return CAL_COMPLETE;
}

static uint8 Stop(CAL_STAGE_TYPE stage, void *params)
{
    float left_dist;
    float right_dist;
    float heading;
    char output[64];
    char left_dist_str[10];
    char right_dist_str[10];
    char heading_str[10];
    CAL_LIN_PARAMS *p_lin_params = (CAL_LIN_PARAMS *)params;

    Cal_SetLeftRightVelocity(0, 0);

    switch (stage)
    {
        case CAL_VALIDATE_STAGE:
            left_dist = abs(Encoder_LeftGetDist());
            right_dist = abs(Encoder_RightGetDist());
            heading = (left_dist - right_dist)/TRACK_WIDTH;

            ftoa(left_dist, left_dist_str, 3);    
            ftoa(right_dist, right_dist_str, 3);
            ftoa(heading, heading_str, 3);
            sprintf(output, "\r\n%s Linear validation complete\r\n", p_lin_params->direction == DIR_FORWARD ? "Forward" : "Backward");
            Ser_PutString(output);
            sprintf(output, "Left Wheel Distance: %s\r\nRight Wheel Distance: %s\r\n", left_dist_str, right_dist_str);
            Ser_PutString(output);
            sprintf(output, "Heading: %s\r\n", heading_str);
            Ser_PutString(output);
            break;
            
        default:
            break;
    }

    debug_control_enabled = old_debug_control_enabled;    

    return CAL_OK;
}

static uint8 Results(CAL_STAGE_TYPE stage, void *params)
{
    CAL_LIN_PARAMS *p_lin_params = (CAL_LIN_PARAMS *)params;

    switch (stage)
    {
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nPrinting Linear validation results\r\n");
            /* Get the left, right and average distance traveled
                Need to capture the left, right delta distance at the start (probably 0 because odometry is reset)
                We want to see how far each wheel went and compute the error
               */
            break;
    }

    return CAL_OK;
}


static float CalLinearLeftTarget()
{
    return left_cmd_velocity;
}

static float CalLinearRightTarget()
{
    return right_cmd_velocity;
}

void CalLin_Init()
{
    CalLin_Validation = &linear_calibration;
}
#ifdef NOT_NOW
static void UpdateVelocity(float distance)
{
    uint32 delta_time;
    
    delta_time = millis() - last_time;
    if (delta_time >= CALLIN_SAMPLE_TIME_MS)
    {
        last_time = millis();
    
        /* Ramp up for the first 1/3 of the motion:
         */
        
        if (distance <= RAMP_UP_DIST)
        {
            float velocity = LINEAR_ACCELERATION * delta_time / 1000.0;
            left_cmd_velocity += velocity;
            left_cmd_velocity = min(left_cmd_velocity, LINEAR_BIAS_VELOCITY);
            right_cmd_velocity += velocity;
            right_cmd_velocity = min(right_cmd_velocity, LINEAR_BIAS_VELOCITY);
        }
        else
        {
            left_cmd_velocity = LINEAR_BIAS_VELOCITY;
            right_cmd_velocity = LINEAR_BIAS_VELOCITY;
        }
    }
}

void DoLinearBiasMotion()
/* 
    Move forward for 1 meter
        Get current position
        Stop motors
        while distance error > theshold
            Get current position
            Calculate distance
            Calculate distance error
            Move motor at linear bias velocity
        Stop motor
    Wait for Linear Bias offset
    Wait for command
        if START the goto START
        if STOP continue
    Update linear bias value in EEPROM
    Set linear bias calibration status bit
    Update linear bias calibration status bit in EEPROM

 */
{
    float x_start;
    float y_start;
    float x_pos;
    float y_pos;
    float dist_error = LINEAR_BIAS_DISTANCE;

    uint16 old_debug_control_enabled = debug_control_enabled;    
    debug_control_enabled = DEBUG_ODOM_ENABLE_BIT;
    
    last_time = millis();

    left_cmd_velocity = 0;
    right_cmd_velocity = 0;
    Motor_SetPwm(PWM_STOP, PWM_STOP);
    
    Pid_SetLeftRightTarget(CalLinearLeftTarget, CalLinearRightTarget);
    
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();        
    
    Odom_GetPosition(&x_start, &y_start);
    
    do
    {
        Encoder_Update();
        Pid_Update();
        Odom_Update();
        
        Odom_GetPosition(&x_pos, &y_pos);
        float distance = sqrt(pow((x_pos - x_start), 2) + pow((y_pos - y_start), 2));

        dist_error = LINEAR_BIAS_DISTANCE - distance;

        UpdateVelocity(distance);
        
    } while (dist_error > LINEAR_BIAS_TOLERANCE);
    
    left_cmd_velocity = 0;
    right_cmd_velocity = 0;
    Motor_SetPwm(PWM_STOP, PWM_STOP);

    debug_control_enabled = old_debug_control_enabled;
}

void CalibrateLinearBias()
{
    Ser_PutString("\r\nPerforming linear bias calibration\r\n");
                
    Nvstore_WriteFloat(1.0, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->linear_bias));

    DoLinearBiasMotion();
    
    Ser_PutString("Enter the measured distance in meters (enter 5 chars), e.g., 1.023 : ");
    float meas_dist = Cal_ReadResponse();
    float linear_bias = 1.0 / meas_dist;
    Cal_DisplayBias("linear", linear_bias);
    
    /* Store the linear bias into EEPROM */
    Nvstore_WriteFloat(linear_bias, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->linear_bias));
    Ser_PutString("Linear bias calibration complete\r\n");
    
}

void ValidateLinearBias()
{
    Ser_PutString("\r\nValidating linear bias calibration\r\n");
    DoLinearBiasMotion();
    Ser_PutString("Linear bias validation complete\r\n");
}
#endif
