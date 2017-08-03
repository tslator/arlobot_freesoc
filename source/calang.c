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
 * Description
 *-------------------------------------------------------------------------------------------------*/
// Add a description of the module

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
#define ANGULAR_MAX_TIME (10000)
#define ANGULAR_BIAS_DIR (DIR_CW)
#define ANGULAR_BIAS_VELOCITY (0.05) // rad/s


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static uint32 start_time;
static uint32 end_time;

static CAL_ANG_PARAMS angular_params = {ANGULAR_BIAS_DIR,
                                        ANGULAR_MAX_TIME, 
                                        0.0,
                                        ANGULAR_BIAS_ANGLE,
                                        // There is no linear velocity because we are rotating in place
                                        0.0,                
                                        ANGULAR_BIAS_DIR == DIR_CW ? -ANGULAR_BIAS_VELOCITY : ANGULAR_BIAS_VELOCITY};

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
 * Name: IsMoveFinished
 * Description: Determines if the move is finished
 * Parameters: distance - the target distance for the move. 
 *             direction - the direction of the move, i.e., CW or CCW 
 * Return: uint8 - TRUE if the move is complete; otherwise, FALSE
 *-------------------------------------------------------------------------------------------------*/
static uint8 IsMoveFinished(DIR_TYPE direction, float *heading, float *distance)
{
    float new_heading;

    new_heading = NormalizeHeading(Odom_GetHeading(), direction);
    
    if (new_heading >= *heading)
    {
        *distance -= new_heading - *heading;
        if( *distance <= 0.0 )
        {
            return TRUE;
        }
        *heading = new_heading;
    }
    else
    {
        return TRUE;
    }

    return FALSE;
}


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

    Cal_SetLeftRightVelocity(0, 0);
    Pid_SetLeftRightTarget(Cal_LeftTarget, Cal_RightTarget);

    Debug_Store();
    Debug_Enable(DEBUG_ODOM_ENABLE_BIT);

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            /* Note: Do we want to support both clockwise and counter clockwise calibration?  How would the
               bias be different?  How would it be applied?
             
               What if we averaged the cw and ccw biases?
             */

            Ser_PutString("\r\nAngular Calibration\r\n");
            Ser_PutString("\r\nPlace a mark on the floor corresponding to the center of one of the wheels\r\n");

            p_ang_params->distance = ANGULAR_BIAS_ANGLE;
            p_ang_params->rps = p_ang_params->direction == DIR_CW ? -ANGULAR_BIAS_VELOCITY : ANGULAR_BIAS_VELOCITY;
            
            break;

        case CAL_VALIDATE_STAGE:
            /* We should support validating clockwise and counter clockwise */

            Ser_PutStringFormat("\r\n%s Angular validation\r\n", 
                                p_ang_params->direction == DIR_CW ? "Clockwise" : "Counter Clockwise");
            
            p_ang_params->distance = ANGULAR_BIAS_ANGLE;
            p_ang_params->rps = p_ang_params->direction == DIR_CW ? -ANGULAR_BIAS_VELOCITY : ANGULAR_BIAS_VELOCITY;
            break;

        default:
            break;
    }
    return CAL_OK;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Start
 * Description: Calibration/Validation interface Start function.  Start Angular Calibration/Validation.
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

    Pid_Enable(TRUE);            
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();

    UniToDiff(p_ang_params->mps, p_ang_params->rps, &left, &right);    
    p_ang_params->heading = NormalizeHeading(Odom_GetHeading(), p_ang_params->direction);

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("Angular Calibration Start\r\n");            
            Ser_PutString("\r\nCalibrating\r\n");

            /* Note: The angular bit is used to by the Encoder module to select the default angular bias or the value
               stored in EEPROM.  Therefore, because angular calibration can be an iterative process, we need to clear 
               the bit after we've reset the Encoder; otherwise, we'll pick up the default and never see the results.
             */
            Cal_ClearCalibrationStatusBit(CAL_ANGULAR_BIT);
            
            break;

        case CAL_VALIDATE_STAGE:
            Ser_PutString("Angular Validation Start\r\n");            
            Ser_PutString("\r\nValidating\r\n");
            
            break;

        default:
            break;
    }

    Cal_SetLeftRightVelocity(left, right);            
    start_time = millis();

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
    CAL_ANG_PARAMS * p_ang_params = (CAL_ANG_PARAMS *) params;

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
        case CAL_VALIDATE_STAGE:
        
            if (millis() - start_time < p_ang_params->run_time)
            {
                if (IsMoveFinished(p_ang_params->direction, &p_ang_params->heading, &p_ang_params->distance))
                {
                    end_time = millis();
                    return CAL_COMPLETE;
                }
                
                return CAL_OK;
                
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
    CAL_ANG_PARAMS *p_ang_params = (CAL_ANG_PARAMS *)params;

    Cal_SetLeftRightVelocity(0, 0);
    Motor_SetPwm(PWM_STOP, PWM_STOP);

    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nAngular Calibration complete\r\n");
            break;
            
        case CAL_VALIDATE_STAGE:
            Ser_PutStringFormat("\r\n%s Angular Validation complete\r\n", 
                                p_ang_params->direction == DIR_CW ? "clockwise" : "counter clockwise");
            break;
            
        default:
            break;
    }

    Debug_Restore();    

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
    float delta_left_dist;
    float delta_right_dist;
    float heading;

    char delta_left_dist_str[10];
    char delta_right_dist_str[10];
    char heading_str[10];
    char angular_bias_str[10];
    
    params = params;

    delta_left_dist = Encoder_LeftGetDeltaDist();
    delta_right_dist = Encoder_RightGetDeltaDist();
    heading = Odom_GetHeading();

    ftoa(delta_left_dist, delta_left_dist_str, 3);    
    ftoa(delta_right_dist, delta_right_dist_str, 3);
    ftoa(heading, heading_str, 6);
    ftoa(Cal_GetAngularBias(), angular_bias_str, 3);
    
    Ser_PutStringFormat("Left Delta Wheel Distance: %s\r\nRight Delta Wheel Distance: %s\r\n", delta_left_dist_str, delta_right_dist_str);
    Ser_PutStringFormat("Heading: %s\r\n", heading_str);
    Ser_PutStringFormat("Elapsed Time: %ld\r\n", end_time - start_time);
    Ser_PutStringFormat("Angular Bias: %s\r\n", angular_bias_str);
        
    switch (stage)
    {
        case CAL_CALIBRATE_STAGE:
            Ser_PutString("\r\nMeasure the rotate traveled by the robot.");
            Ser_PutString("\r\nEnter the rotation (in degrees): ");
            float rot_in_degrees = Cal_ReadResponse();
            Ser_PutString("\r\n");
            
            /* If the actual rotation is less than 360.0 then each delta is too small, i.e., lengthen delta by 360/rotation
               If the actual rotation is greater than 360.0 then each delta is too small, i.e., shorten delta by rotation/360
             */
            float bias = rot_in_degrees >= 360.0 ? 360.0 / rot_in_degrees : rot_in_degrees / 360.0;
    
            ftoa(bias, angular_bias_str, 3);
            Ser_PutStringFormat("New Angular Bias: %s\r\n", angular_bias_str);
            
            Nvstore_WriteFloat(bias, (uint16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->angular_bias));
            Cal_SetCalibrationStatusBit(CAL_ANGULAR_BIT);
            
            break;
            
        case CAL_VALIDATE_STAGE:
            Ser_PutString("\r\nPrinting Angular validation results\r\n");
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
