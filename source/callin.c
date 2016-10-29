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
 * The purpose of linear validation will be move the robot in the forward/backward direction,
 * capture the odometry, and compare the results with the expected distance traveled.  The
 * goal is to see the robot moving in a straight line.
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define LINEAR_DISTANCE (1.0)
#define LINEAR_VELOCITY (0.150)
#define LINEAR_MAX_TIME (10000)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static uint32 start_time;
static uint32 end_time;
static uint16 old_debug_control_enabled;

static CAL_LIN_PARAMS linear_params = {DIR_FORWARD, 
                                       LINEAR_MAX_TIME, 
                                       LINEAR_DISTANCE, 
                                       LINEAR_VELOCITY};

static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static CALIBRATION_TYPE linear_calibration = {CAL_INIT_STATE,
                                              CAL_CALIBRATE_STAGE,
                                              &linear_params,
                                              Init,
                                              Start,
                                              Update,
                                              Stop,
                                              Results};

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: Init
 * Description: Calibration/Validation interface Init function.  Performs initialization for Linear 
 *              Calibration/Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - linear validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Init(CAL_STAGE_TYPE stage, void *params)
{
    CAL_LIN_PARAMS *p_lin_params = (CAL_LIN_PARAMS *)params;
    char output[64];
    
    old_debug_control_enabled = debug_control_enabled;
        
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            /* This should look just like the validation except after moving forward, we need to prompt the user to 
               enter the actual distance moved.
               Also, we need to add a linear scaler to nvram storage, initialize the scaler on startup and apply the
               scalar in the encoder.
             */
            /* Note: Do we want to support both forward and backward calibration?  How would the
               bias be different?  How would it be applied?
             */
            sprintf(output, "\r\nLinear Calibration\r\n");
            Ser_PutString(output);
            sprintf(output, "\r\nPlace a meter stick along side the robot starting centered\r\n");
            Ser_PutString(output);
            sprintf(output, "on the wheel and extending toward the front of the robot\r\n");
            Ser_PutString(output);
            
            Cal_SetLeftRightVelocity(0, 0);
            Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);
            
            debug_control_enabled = DEBUG_ODOM_ENABLE_BIT | DEBUG_LEFT_ENCODER_ENABLE_BIT | DEBUG_RIGHT_ENCODER_ENABLE_BIT;
            
            break;            
            
        case CAL_VALIDATE_STAGE:
            sprintf(output, "\r\n%s Linear validation\r\n", p_lin_params->direction == DIR_FORWARD ? "Forward" : "Backward");
            Ser_PutString(output);

            Cal_SetLeftRightVelocity(0, 0);
            Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);
            
            debug_control_enabled = DEBUG_ODOM_ENABLE_BIT;
            
            break;

        default:
            break;
    }

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Linear Calibration/Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - linear validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Start(CAL_STAGE_TYPE stage, void *params)
{
    CAL_LIN_PARAMS *p_lin_params = (CAL_LIN_PARAMS *) params;

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("Linear Calibration Start\r\n");
            
            Ser_PutString("\r\nCalibrating ");
            Ser_PutString("\r\n");
            
            Pid_Enable(TRUE);            
            Encoder_Reset();
            Pid_Reset();
            Odom_Reset();

            /* Note: The linear bit is used to by the Encoder module to select the default linear bias or the value
               stored in EEPROM.  Therefore, because linear calibration can be an iterative process, we need to clear 
               the bit after we've reset the Encoder; otherwise, we'll pick up the default and never see the results.
             */
            Cal_ClearCalibrationStatusBit(CAL_LINEAR_BIT);
            
            Cal_SetLeftRightVelocity(p_lin_params->mps, p_lin_params->mps);            
            start_time = millis();
            
            break;
            
        case CAL_VALIDATE_STAGE:
            Ser_PutString("Linear Validation Start\r\n");

            Ser_PutString("\r\nValidating ");
            Ser_PutString("\r\n");
            
            float velocity = p_lin_params->mps;
            if( p_lin_params->direction == DIR_BACKWARD )
            {
                velocity = -velocity;
            }

            Pid_Enable(TRUE);            
            Encoder_Reset();
            Pid_Reset();
            Odom_Reset();
            
            Cal_SetLeftRightVelocity(velocity, velocity);
            start_time = millis();

            break;

        default:
            break;
    }

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: stage - the calibration/validation stage 
 *             params - linear validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Update(CAL_STAGE_TYPE stage, void *params)
{
    float left_dist;
    float right_dist;

    CAL_LIN_PARAMS * p_lin_params = (CAL_LIN_PARAMS *) params;

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            if (millis() - start_time < p_lin_params->run_time)
            {
                left_dist = abs(Encoder_LeftGetDist());
                right_dist = abs(Encoder_RightGetDist());
                if ( left_dist < p_lin_params->distance && right_dist < p_lin_params->distance )
                {
                    return CAL_OK;
                }
                Cal_SetLeftRightVelocity(0, 0);
                end_time = millis();
                return CAL_COMPLETE;
            }
            end_time = millis();
            Cal_SetLeftRightVelocity(0, 0);
            Ser_PutString("\r\nRun time expired\r\n");
            break;
            
        case CAL_VALIDATE_STAGE:
            if (millis() - start_time < p_lin_params->run_time)
            {
                left_dist = abs(Encoder_LeftGetDist());
                right_dist = abs(Encoder_RightGetDist());

                if ( left_dist < p_lin_params->distance && right_dist < p_lin_params->distance )
                {
                    return CAL_OK;
                }
                Cal_SetLeftRightVelocity(0, 0);
                end_time = millis();
                return CAL_COMPLETE;
            }
            Cal_SetLeftRightVelocity(0, 0);
            end_time = millis();
            Ser_PutString("\r\nRun time expired\r\n");
            break;

        default:
            break;
    }

    return CAL_COMPLETE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Stop
 * Description: Calibration/Validation interface Stop function.  Called to stop calibration/validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - linear validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Stop(CAL_STAGE_TYPE stage, void *params)
{
    char output[64];
    CAL_LIN_PARAMS *p_lin_params = (CAL_LIN_PARAMS *)params;

    Cal_SetLeftRightVelocity(0, 0);
    /* I found that setting the velocities to 0 didn't immediately stop the motors before the calibration prompt for
       the actual distance traveled.  Maybe that has to do with the PIDs, but calling SetPwm on the motor guarantees
       the motors will be stopped which is what we want.  It just seems heavy handed :-)
     */
    Motor_SetPwm(PWM_STOP, PWM_STOP);
    Pid_RestoreLeftRightTarget();    
    debug_control_enabled = old_debug_control_enabled;    
    
    switch (stage)
    {
        case CAL_VALIDATE_STAGE:
            sprintf(output, "\r\n%s Linear Validation complete\r\n", p_lin_params->direction == DIR_FORWARD ? "Forward" : "Backward");
            Ser_PutString(output);
            break;
            
        case CAL_CALIBRATE_STAGE:
            sprintf(output, "\r\nLinear Calibration complete\r\n");
            Ser_PutString(output);
            break;
            
        default:
            break;
    }

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Results
 * Description: Calibration/Validation interface Results function.  Called to display validation 
 *              results. 
 * Parameters: stage - the calibration/validation stage (validation only) 
 *             params - linear validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Results(CAL_STAGE_TYPE stage, void *params)
{
    float distance;
    float left_dist;
    float right_dist;
    float heading;

    char left_dist_str[10];
    char right_dist_str[10];
    char heading_str[10];
    char linear_bias_str[10];

    char output[64];
    
    left_dist = Encoder_LeftGetDist();
    right_dist = Encoder_RightGetDist();
    heading = Odom_GetHeading();

    ftoa(left_dist, left_dist_str, 3);    
    ftoa(right_dist, right_dist_str, 3);
    ftoa(heading, heading_str, 6);
    ftoa(Cal_GetLinearBias(), linear_bias_str, 3);
    
    sprintf(output, "Left Wheel Distance: %s\r\nRight Wheel Distance: %s\r\n", left_dist_str, right_dist_str);
    Ser_PutString(output);
    sprintf(output, "Heading: %s\r\n", heading_str);
    Ser_PutString(output);
    sprintf(output, "Elapsed Time: %ld\r\n", end_time - start_time);
    Ser_PutString(output);
    sprintf(output, "Linear Bias: %s\r\n", linear_bias_str);
    Ser_PutString(output);
    
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nMeasure the distance traveled by the robot.");
            Ser_PutString("\r\nEnter the distance (0.5 to 1.5): ");
            distance = Cal_ReadResponse();
            Ser_PutString("\r\n");

            if (distance < CAL_LINEAR_BIAS_MIN || distance > CAL_LINEAR_BIAS_MAX)
            {
                char distance_str[10];
                ftoa(distance, distance_str, 6);
                sprintf(output, "The distance entered %s is out of the allowed range.  No change will be made.\r\n", distance_str);
                Ser_PutString(output);
                distance = p_cal_eeprom->linear_bias;
            }
            distance = constrain(distance, CAL_LINEAR_BIAS_MIN, CAL_LINEAR_BIAS_MAX);

            Nvstore_WriteFloat(distance, (uint16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->linear_bias));
            Cal_SetCalibrationStatusBit(CAL_LINEAR_BIT);
            
            break;
            
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nPrinting Linear validation results\r\n");
            /* Get the left, right and average distance traveled
                Need to capture the left, right delta distance at the start (probably 0 because odometry is reset)
                We want to see how far each wheel went and compute the error
               */
            break;

        default:
            break;    
    }

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalLin_Init
 * Description: Initializes the Linear calibration module 
 * Parameters: None 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void CalLin_Init()
{
    CalLin_Validation = &linear_calibration;
    CalLin_Calibration = &linear_calibration;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
