/* 
MIT License

Copyright (c) 2017 Tim Slator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*---------------------------------------------------------------------------------------------------
   Description: This module provides the implementation for calibration.
 *-------------------------------------------------------------------------------------------------*/    

#ifndef CAL_H
#define CAL_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/

#include "freesoc.h"
#include "config.h"
#include "calstore.h"
#include "pwm.h"
#include "pidtypes.h"
    
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
#define CAL_LINEAR_BIT          (0x0004)
#define CAL_ANGULAR_BIT         (0x0008)
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

typedef UINT8 (*INIT_FUNC_TYPE)();
typedef UINT8 (*START_FUNC_TYPE)();
typedef UINT8 (*UPDATE_FUNC_TYPE)();
typedef UINT8 (*STOP_FUNC_TYPE)();
typedef UINT8 (*RESULTS_FUNC_TYPE)();
typedef void (*SET_LEFT_RIGHT_VELOCITY_TYPE)(FLOAT left, FLOAT right);


typedef struct calval_interface_type_tag
{
    CAL_STATE_TYPE state;
    CAL_STAGE_TYPE stage;
    void *params;
    INIT_FUNC_TYPE init;
    START_FUNC_TYPE start;
    UPDATE_FUNC_TYPE update;
    STOP_FUNC_TYPE stop;
    RESULTS_FUNC_TYPE results;
} CALVAL_INTERFACE_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
extern char* cal_stage_to_string[];
extern char* cal_state_to_string[];

GET_TARGET_FUNC_TYPE Cal_LeftTarget;
GET_TARGET_FUNC_TYPE Cal_RightTarget;


/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/
#define CAL_STAGE_TO_STRING(stage) cal_stage_to_string[stage]
#define CAL_STATE_TO_STRING(state) cal_state_to_string[state]

/* The CPS values must be stored from lowest (most negative) to higest (most positive) which means the index used
   for storage must be adjusted based on direction
 */     
#define PWM_CALC_OFFSET(direction, index)  (direction == DIR_BACKWARD ? CAL_NUM_SAMPLES - 1 - index : index)
 
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
 
typedef struct calval_pid_params_tag
{
    char name[6];
    PID_ENUM_TYPE pid_type;
    DIR_TYPE direction;
    UINT32 run_time;
} CALVAL_PID_PARAMS;

typedef struct _linear_params
{
    DIR_TYPE direction;
    UINT32   run_time;
    FLOAT    distance;
    FLOAT    linear;
    FLOAT    angular;
} CALVAL_LIN_PARAMS;

typedef struct _angular_params
{
    DIR_TYPE direction;
    UINT32   run_time;
    FLOAT    heading;
    FLOAT    distance;
    FLOAT    linear;
    FLOAT    angular;
} CALVAL_ANG_PARAMS;


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void Cal_Init();
void Cal_Start();
void Cal_Update();
FLOAT Cal_ReadResponse();
void Cal_SetLeftRightVelocity(FLOAT left, FLOAT right);
PWM_TYPE Cal_CpsToPwm(WHEEL_TYPE wheel, FLOAT cps);
void Cal_Clear();
void Cal_ClearCalibrationStatusBit(UINT16 bit);
void Cal_SetCalibrationStatusBit(UINT16 bit);
UINT16 Cal_GetCalibrationStatusBit(UINT16 bit);

void Cal_PrintLeftMotorParams(BOOL as_json);
void Cal_PrintRightMotorParams(BOOL as_json);
void Cal_PrintAllMotorParams(BOOL as_json);
void Cal_PrintSamples(WHEEL_TYPE wheel, DIR_TYPE dir, CAL_DATA_TYPE* const cal_data, UINT8 as_json);
void Cal_PrintPidGains(WHEEL_TYPE wheel, FLOAT* const gains, UINT8 as_json);
void Cal_PrintAllPidGains(BOOL as_json);

void Cal_CalcTriangularProfile(UINT8 num_points, FLOAT lower_limit, FLOAT upper_limit, FLOAT* const forward_output, FLOAT* const backward_output);

void Cal_CalcOperatingRange(FLOAT low_percent, FLOAT high_percent, FLOAT domain, FLOAT* const start, FLOAT* const stop);
void Cal_CalcForwardOperatingRange(FLOAT low_percent, FLOAT high_percent, FLOAT* const start, FLOAT* const stop);
void Cal_CalcBackwardOperatingRange(FLOAT low_percent, FLOAT high_percent, FLOAT* const start, FLOAT* const stop);

FLOAT Cal_GetLinearBias();
FLOAT Cal_GetAngularBias();
void Cal_PrintBias(UINT8 as_json);
CAL_PID_TYPE* Cal_GetPidGains(PID_ENUM_TYPE pid);
UINT16 Cal_GetStatus();
void Cal_PrintStatus(UINT8 as_json);
void Cal_SetGains(PID_ENUM_TYPE pid, FLOAT* const gains);
CAL_DATA_TYPE* Cal_GetMotorData(WHEEL_TYPE wheel, DIR_TYPE dir);

void Cal_SetAngularBias(FLOAT bias);
void Cal_SetLinearBias(FLOAT bias);
void Cal_SetMotorData(WHEEL_TYPE wheel, DIR_TYPE dir, CAL_DATA_TYPE *data);

#endif

/* [] END OF FILE */
