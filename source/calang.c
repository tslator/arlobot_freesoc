#include <math.h>
#include "calang.h"
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
#define ANGULAR_BIAS_ANGLE (2*PI)   // rad
#define ANGULAR_BIAS_VELOCITY (0.05) // rad/s
#define ANGULAR_MAX_TIME (15000)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static uint32 start_time;
static uint32 end_time;
static uint16 old_debug_control_enabled;

static CAL_ANG_PARAMS angular_params = {DIR_CW, 
                                        ANGULAR_MAX_TIME, 
                                        ANGULAR_BIAS_ANGLE,
                                        0.0,    // There is no linear velocity because we are rotating in place
                                        ANGULAR_BIAS_VELOCITY,
                                        TRUE};

static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static CALIBRATION_TYPE angular_calibration = {CAL_INIT_STATE,
                                               CAL_CALIBRATE_STAGE,
                                               &angular_params,
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
 * Description: Calibration/Validation interface Init function.  Performs initialization for Angular 
 *              Calibration/Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Init(CAL_STAGE_TYPE stage, void *params)
{
    CAL_ANG_PARAMS *p_ang_params = (CAL_ANG_PARAMS *)params;
    char banner[64];

    Cal_SetLeftRightVelocity(0, 0);
    Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);

    old_debug_control_enabled = debug_control_enabled;

    debug_control_enabled = DEBUG_ODOM_ENABLE_BIT;

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            /* Note: Do we want to support both clockwise and counter clockwise calibration?  How would the
               bias be different?  How would it be applied?
             */

            sprintf(banner, "\r\nAngular Calibration\r\n");
            Ser_PutString(banner);
            sprintf(banner, "\r\nPlace a mark on the floor corresponding\r\n");
            Ser_PutString(banner);
            sprintf(banner, "to the center of one of the wheels\r\n");
            Ser_PutString(banner);

            break;

        case CAL_VALIDATE_STAGE:
            /* We should support validating clockwise and counter clockwise */

            sprintf(banner, "\r\n%s Angular validation\r\n", p_ang_params->direction == DIR_CW ? "Clockwise" : "Counter Clockwise");
            Ser_PutString(banner);
            break;

        default:
            break;
    }
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Agnular Calibration/Validation.
 * Parameters: stage - the calibration/validation stage 
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Start(CAL_STAGE_TYPE stage, void *params)
{
    CAL_ANG_PARAMS *p_ang_params = (CAL_ANG_PARAMS *) params;
    float left;
    float right;
    float velocity;

    Pid_Enable(TRUE);            
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("Angular Calibration Start\r\n");
            
            Ser_PutString("\r\nCalibrating ");
            Ser_PutString("\r\n");

            velocity = p_ang_params->rps;
            if (p_ang_params->direction == DIR_CW)
            {
                velocity = -velocity;
            }
            UniToDiff(p_ang_params->mps, velocity, &left, &right);
            
            /* Note: The angular bit is used to by the Encoder module to select the default angular bias or the value
               stored in EEPROM.  Therefore, because angular calibration can be an iterative process, we need to clear 
               the bit after we've reset the Encoder; otherwise, we'll pick up the default and never see the results.
             */
            Cal_ClearCalibrationStatusBit(CAL_ANGULAR_BIT);
            
            Cal_SetLeftRightVelocity(left, right);
            start_time = millis();
            break;

        case CAL_VALIDATE_STAGE:
            Ser_PutString("Angular Validation Start\r\n");
            
            Ser_PutString("\r\nValidating ");
            Ser_PutString("\r\n");
            
            velocity = p_ang_params->rps;
            if (p_ang_params->direction == DIR_CW)
            {
                velocity = -velocity;
            }
            UniToDiff(p_ang_params->mps, velocity, &left, &right);
            
            Cal_SetLeftRightVelocity(left, right);            
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
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK, CAL_COMPLETE
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Update(CAL_STAGE_TYPE stage, void *params)
{
    float heading;    
    char heading_str[10];
    char distance_str[10];
    char output[64];
    CAL_ANG_PARAMS * p_ang_params = (CAL_ANG_PARAMS *) params;

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
        case CAL_VALIDATE_STAGE:
            if (millis() - start_time < p_ang_params->run_time)
            {
                heading = Odom_GetHeading();
                
                /* Because the heading is limited to -PI to PI, to do one full turn 360 degrees/2PI radians it is 
                   necessary to track the turn in two segments: 0 to -PI, and PI to 0
                
                   In the 'first half' of the move, the heading is 0 - -PI.  Once the first 180 degrees has been turned
                   we need to track the 'second half' of the move from PI to 0.
                 */
                                
                /************************************************************
                 NOTE: THE FOLLOWING ONLY WORKS FOR THE CLOCKWISE DIRECTION. 
                  
                 need to add a check of direction and change the conditions
                 ************************************************************/
                
                if (p_ang_params->first_half)
                {
                    if ( heading <= 0.0 && heading >= -PI)
                    {
                        return CAL_OK;
                    }
                    p_ang_params->first_half = FALSE;
                    return CAL_OK;
                }
                else
                {
                    if (heading >= 0.0)
                    {
                        return CAL_OK;
                    }                    
                    p_ang_params->first_half = TRUE;
                }
                
                end_time = millis();
                return CAL_COMPLETE;
            }
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
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Stop(CAL_STAGE_TYPE stage, void *params)
{
    char output[64];

    CAL_ANG_PARAMS *p_ang_params = (CAL_ANG_PARAMS *)params;

    Cal_SetLeftRightVelocity(0, 0);
    Motor_SetPwm(PWM_STOP, PWM_STOP);

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            sprintf(output, "\r\nAngular Calibration complete\r\n");
            Ser_PutString(output);
            break;
            
        case CAL_VALIDATE_STAGE:
            sprintf(output, "\r\n%s Angular Validation complete\r\n", p_ang_params->direction == DIR_CW ? "clockwise" : "counter clockwise");
            Ser_PutString(output);
            break;
            
        default:
            break;
    }

    debug_control_enabled = old_debug_control_enabled;    

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Results
 * Description: Calibration/Validation interface Results function.  Called to display calibration/validation 
 *              results. 
 * Parameters: stage - the calibration/validation stage (validation only) 
 *             params - angular validation parameters, e.g. direction, run time, etc. 
 * Return: uint8 - CAL_OK
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 Results(CAL_STAGE_TYPE stage, void *params)
{
    float left_dist;
    float right_dist;
    float heading;

    char left_dist_str[10];
    char right_dist_str[10];
    char heading_str[10];
    char angular_bias_str[10];

    char output[64];
    
    left_dist = Encoder_LeftGetDist();
    right_dist = Encoder_RightGetDist();
    heading = Odom_GetHeading();

    ftoa(left_dist, left_dist_str, 3);    
    ftoa(right_dist, right_dist_str, 3);
    ftoa(heading, heading_str, 6);
    ftoa(Cal_GetAngularBias(), angular_bias_str, 3);
    
    sprintf(output, "Left Wheel Distance: %s\r\nRight Wheel Distance: %s\r\n", left_dist_str, right_dist_str);
    Ser_PutString(output);
    sprintf(output, "Heading: %s\r\n", heading_str);
    Ser_PutString(output);
    sprintf(output, "Elapsed Time: %ld\r\n", end_time - start_time);
    Ser_PutString(output);
    sprintf(output, "Angular Bias: %s\r\n", angular_bias_str);
    Ser_PutString(output);
        
    switch (stage)
    {
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nPrinting Angular validation results\r\n");
            /* Get the left, right and average distance traveled
                Need to capture the left, right delta distance at the start (probably 0 because odometry is reset)
                We want to see how far each wheel went and compute the error
               */
            break;

        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nMeasure the rotate traveled by the robot.");
            Ser_PutString("\r\nEnter the rotation (in degrees): ");
            float rot_in_degrees = Cal_ReadResponse();
            Ser_PutString("\r\n");
            
            /* If the actual rotation is less than 360.0 then each delta is too small, i.e., lengthen delta by 360/rotation
               If the actual rotation is greater than 360.0 then each delta is too small, i.e., shorten delta by rotation/360
             */
            float bias = rot_in_degrees >= 360.0 ? bias = 360.0 / rot_in_degrees : rot_in_degrees / 360.0;
    
            ftoa(bias, angular_bias_str, 3);
            sprintf(output, "New Angular Bias: %s\r\n", angular_bias_str);
            Ser_PutString(output);
            
            Nvstore_WriteFloat(bias, (uint16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->angular_bias));
            Cal_SetCalibrationStatusBit(CAL_ANGULAR_BIT);
            
        default:
            break;    
    }

    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalLin_Init
 * Description: Initializes the Angular calibration module 
 * Parameters: None 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void CalAng_Init()
{
    CalAng_Calibration = &angular_calibration;
    CalAng_Validation = &angular_calibration;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
