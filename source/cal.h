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

#ifndef CAL_H
#define CAL_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/

#include <project.h>
#include "config.h"
#include "calstore.h"
#include "pwm.h"
    
/* The calibration module supports the following calibrations:
    
    1. Motor - creates a mapping between count/sec velocity and motor pwm value.  The mappping between count/sec and pwm
       must be performed for each wheel in the forward and backward direction.  This resuls in four tables:
            left-forward  - cps starts at 0 and runs to max, pwm starts at 1500 and runs to 2000
            left-reverse  - cps starts at 0 and runs to -max, pwm starts at 1500 and runs to 1000
            right-forward - cps starts at 0 and runs to max, pwm starts at 1500 and runs to 1000
            right-reverse - cps starts at 0 and runs to -max, pwm starts at 1500 and runs to 2000
       Note: The pwm values are reversed between left/right wheels because the right wheel is rotated 90 degees
    
       Calibration involves selecting many pwm values evenly distributed across the pwm range and measuring the count/sec
       velocity.  An average velocity is taken for each pwm value.  The count/sec values are scaled to reduce duplicate
       entries.
    
       The four tables are stored in EEPROM and read into memory on startup.           
    
    2. Left/Right PID - each wheel has a PID controller defined.  The purpose of the left/right PID controllers is to 
       ensure wheel speed is accurate wrt to the commanded velocity and that the wheels are responsive.  The gains for 
       each PID control must be set and tuned as a part of calibration.  PID calibration is performed by:
            1. setting Kp, Ki, Kd for PID control
            2. executing a step input
            3. capturing the output variables for analysis
            4. repeat until PID is tuned
       Calibration is run for each wheel.  The resulting gains are stored in EEPROM and read into memory on startup.
    
 */
    
/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/

/* Calibration control/status bits */
#define CAL_MOTOR_BIT           (0x0001)
#define CAL_PID_BIT             (0x0002)
#define CAL_VERBOSE_BIT         (0x0080)
    
#define CAL_SCALE_FACTOR (100)
    
#define CAL_OK          (0)
#define CAL_COMPLETE    (1)
#define CAL_ERROR       (255)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef enum {CAL_INIT_STATE, CAL_START_STATE, CAL_RUNNING_STATE, CAL_STOP_STATE, CAL_RESULTS_STATE, CAL_DONE_STATE} CAL_STATE_TYPE;
typedef enum {CAL_CALIBRATE_STAGE, CAL_VALIDATE_STAGE } CAL_STAGE_TYPE;

typedef uint8 (*INIT_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);
typedef uint8 (*START_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);
typedef uint8 (*UPDATE_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);
typedef uint8 (*STOP_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);
typedef uint8 (*RESULTS_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);

typedef struct calibration_type_tag
{
    CAL_STATE_TYPE state;
    CAL_STAGE_TYPE stage;
    void *params;
    INIT_FUNC_TYPE init;
    START_FUNC_TYPE start;
    UPDATE_FUNC_TYPE update;
    STOP_FUNC_TYPE stop;
    RESULTS_FUNC_TYPE results;
} CALIBRATION_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
extern char* cal_stage_to_string[];
extern char* cal_state_to_string[];

volatile CAL_EEPROM_TYPE *p_cal_eeprom;

GET_TARGET_FUNC_TYPE Cal_LeftTarget;
GET_TARGET_FUNC_TYPE Cal_RightTarget;

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/
#define CAL_STAGE_TO_STRING(stage) cal_stage_to_string[stage]
#define CAL_STATE_TO_STRING(state) cal_state_to_string[state]

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void Cal_Init();
void Cal_Start();
void Cal_Update();
float Cal_ReadResponse();
CAL_PID_TYPE* Cal_LeftGetPidGains();
CAL_PID_TYPE* Cal_RightGetPidGains();
void Cal_SetLeftRightVelocity(float left, float right);
PWM_TYPE Cal_CpsToPwm(WHEEL_TYPE wheel, float cps);
void Cal_Clear();
void ClearCalibrationStatusBit(uint16 bit);
void SetCalibrationStatusBit(uint16 bit);
void Cal_PrintSamples(char *label, int32 *cps_samples, PWM_TYPE *pwm_samples);
void Cal_PrintGains(char *label, float *gains);
void Cal_CalcTriangularProfile(uint8 num_points, float lower_limit, float upper_limit, float *forward_output, float *backward_output);

#endif

/* [] END OF FILE */
