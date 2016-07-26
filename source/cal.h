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

#include <project.h>
#include "config.h"
#include "calstore.h"
    
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
    
    3. Angular PID - because the Left/Right velocity PID controllers are independent each will attempt to 
       run at the commanded velocity, but they will very likely have different response times which can cause the 
       robot to deviate from the intended path.
    
       A single PID controllers have been added to maintain angular velocity based on the odometry.  The 
       gains for the PID need to be calibrated.  Likely though, only Kp will be needed.
       
    4. Linear Bias - moves 1 meter forward, stops and waits to receive the actual linear distance (in meters), calculates
       the linear bias and stores in EEPROM.  The linear bias is applied to the delta distance calculation in odometry.
       
    5. Angular Bias - rotates 360 degrees, stops and waits to receive the actual rotation (in degrees), calculates the 
       angular bias and stores in EEPROM.  The angular bias is applied to the delta heading calculation in odometry.
 */
    
/* Calibration control/status bits */
#define CAL_MOTOR_BIT           (0x0001)
#define CAL_PID_BIT             (0x0002)
#define CAL_LINEAR_BIAS_BIT     (0x0004)
#define CAL_ANGULAR_BIAS_BIT    (0x0008)
#define CAL_VERBOSE_BIT         (0x0080)
    
#define CAL_SCALE_FACTOR (100)
    
#define CAL_OK          (0)
#define CAL_COMPLETE    (1)
#define CAL_ERROR       (255)

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

volatile CAL_EEPROM_TYPE *p_cal_eeprom;

GET_TARGET_FUNC_TYPE Cal_LeftTarget;
GET_TARGET_FUNC_TYPE Cal_RightTarget;

void Cal_Init();
void Cal_Start();
void Cal_Update();

float Cal_ReadResponse();

void Cal_LeftGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data);
void Cal_RightGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data);

CAL_PID_TYPE* Cal_LeftGetPidGains();
CAL_PID_TYPE* Cal_RightGetPidGains();
CAL_PID_TYPE* Cal_LinearGetPidGains();
CAL_PID_TYPE* Cal_AngularGetPidGains();


void Cal_SetLeftRightVelocity(float left, float right);



#endif

/* [] END OF FILE */
