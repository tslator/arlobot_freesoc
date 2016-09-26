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

static float left_cmd_velocity;
static float right_cmd_velocity;

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
                                              CAL_VALIDATE_STAGE,
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
 *              Validation.
 * Parameters: stage - the calibration/validation stage (validation only) 
 *             params - linear validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
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

        case CAL_CALIBRATE_STAGE:
        default:
            break;
    }

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Linear Validation.
 * Parameters: stage - the calibration/validation stage (validation only) 
 *             params - linear validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
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

        case CAL_CALIBRATE_STAGE:
        default:
            break;
    }

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Update
 * Description: Calibration/Validation interface Update function.  Called periodically to evaluate 
 *              the termination condition.
 * Parameters: stage - the calibration/validation stage (validation only) 
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
        case CAL_VALIDATE_STAGE:
            if (millis() - start_time < p_lin_params->run_time)
            {
                left_dist = abs(Encoder_LeftGetDist());
                right_dist = abs(Encoder_RightGetDist());

                if ( left_dist < p_lin_params->distance && right_dist < p_lin_params->distance )
                {
                    return CAL_OK;
                }
                end_time = millis();
                return CAL_COMPLETE;
            }
            end_time = millis();
            Ser_PutString("\r\nRun time expired\r\n");
            break;

        case CAL_CALIBRATE_STAGE:
        default:
            break;
    }

    return CAL_COMPLETE;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Stop
 * Description: Calibration/Validation interface Stop function.  Called to stop validation.
 * Parameters: stage - the calibration/validation stage (validation only) 
 *             params - linear validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Stop(CAL_STAGE_TYPE stage, void *params)
{
    float left_dist;
    float right_dist;
    float heading;

    char left_dist_str[10];
    char right_dist_str[10];
    char heading_str[10];

    char output[64];

    CAL_LIN_PARAMS *p_lin_params = (CAL_LIN_PARAMS *)params;

    Cal_SetLeftRightVelocity(0, 0);

    switch (stage)
    {
        case CAL_VALIDATE_STAGE:
            left_dist = Encoder_LeftGetDist();
            right_dist = Encoder_RightGetDist();
            heading = (left_dist - right_dist)/TRACK_WIDTH;

            ftoa(left_dist, left_dist_str, 3);    
            ftoa(right_dist, right_dist_str, 3);
            ftoa(heading, heading_str, 6);

            sprintf(output, "\r\n%s Linear validation complete\r\n", p_lin_params->direction == DIR_FORWARD ? "Forward" : "Backward");
            Ser_PutString(output);
            sprintf(output, "Left Wheel Distance: %s\r\nRight Wheel Distance: %s\r\n", left_dist_str, right_dist_str);
            Ser_PutString(output);
            sprintf(output, "Heading: %s\r\n", heading_str);
            Ser_PutString(output);
            sprintf(output, "Elapsed Time: %ld\r\n", end_time - start_time);
            Ser_PutString(output);
            break;
            
        case CAL_CALIBRATE_STAGE:
        default:
            break;
    }

    debug_control_enabled = old_debug_control_enabled;    

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

        case CAL_CALIBRATE_STAGE:
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
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
