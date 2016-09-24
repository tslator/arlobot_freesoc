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

#define ANGULAR_BIAS_VELOCITY (0.7)
#define ANGULAR_BIAS_ANGLE (2*PI)
#define ANGULAR_ACCELERATION (0.3)
#define RAMP_UP_ANGLE        (ANGULAR_BIAS_ANGLE / 3.0)

/* Note: The Encoder and PID sample rates are 20 Hz.  The sample rate for updating the linear velocity must be less 
         than those sample rates, i.e., 10 Hz.
*/
#define CALANG_SAMPLE_RATE (PID_SAMPLE_RATE / 2)        
#define CALANG_SAMPLE_TIME_MS  SAMPLE_TIME_MS(CALANG_SAMPLE_RATE)

static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static CALIBRATION_TYPE angular_calibration = {CAL_INIT_STATE,
                                               CAL_CALIBRATE_STAGE,
                                               0,
                                               Init,
                                               Start,
                                               Update,
                                               Stop,
                                               Results};

static float left_cmd_velocity;
static float right_cmd_velocity;
static float angular_velocity;
static uint32 last_time;

/* The purpose of angular calibration will be to set the gains for the angular velocity PID.  The angular velocity PID
   operates from within control and ensures that the commanded angular velocity is followed.  The calibration the angular
   velocity PID, the following is done:
 
   The calibration the angular velocity PID, the following is done:
 
   Angular Velocity PID
   1. Set the angular gains (try 0, 0, 0 to start)
   2. Move the robot forward 1 meter
   3. If the robot veers off left or right then re-align the robot and repeat
   Note: Each iteration will reverse the direction of the robot, e.g., forward 1 meter, reverse 1 meter, forward 1 meter, etc,
   so it is necessary to re-align the robot before each run (or not if you have plenty of room).

 */

static uint8 Init(CAL_STAGE_TYPE stage, void *params)
{
    return CAL_OK;
}

static uint8 Start(CAL_STAGE_TYPE stage, void *params)
{
    return CAL_OK;
}

static uint8 Update(CAL_STAGE_TYPE stage, void *params)
{
    return CAL_COMPLETE;
}

static uint8 Stop(CAL_STAGE_TYPE stage, void *params)
{
    return CAL_OK;
}

static uint8 Results(CAL_STAGE_TYPE stage, void *params)
{
    return CAL_OK;
}

static float CalAngularLeftTarget()
{
    return left_cmd_velocity;
}

static float CalAngularRightTarget()
{
    return right_cmd_velocity;
}

void CalAng_Init()
{
    CalAng_Calibration = &angular_calibration;
}

static void UpdateVelocity(float heading)
{
    uint32 delta_time;
    #define MAX_VELOCITY (0.14105)
    
    delta_time = millis() - last_time;
    if (delta_time >= CALANG_SAMPLE_TIME_MS)
    {
        last_time = millis();
    
        /* Ramp up for the first 1/3 of the motion:
         */
        
        if (abs(heading) <= RAMP_UP_ANGLE)
        {
            angular_velocity += ANGULAR_ACCELERATION * delta_time / 1000.0;
            angular_velocity = min(angular_velocity, ANGULAR_BIAS_VELOCITY);
            
            UniToDiff(0, angular_velocity, &left_cmd_velocity, &right_cmd_velocity);            
        }
        else
        {
            UniToDiff(0, ANGULAR_BIAS_VELOCITY, &left_cmd_velocity, &right_cmd_velocity);            
        }
    }
        
}

void DoAngularBiasMotion()
/*
    Set angular bias to 1.0
    Rotate 360 degrees
        Get the current heading
        Stop motors
        Start turn angle = 0
        Test angle = 360
        while turn angle < test angle

            Set angular velocity
            Get heading
            Calculate delta angle
            Update turn and last angles

        Stop motors

 */
{
    float heading;
    float turn_heading;
    float delta_heading;
    float last_heading;
    
    uint16 old_debug_control_enabled = debug_control_enabled;    
    debug_control_enabled = DEBUG_ODOM_ENABLE_BIT;
    
    last_time = millis();

    left_cmd_velocity = 0;
    right_cmd_velocity = 0;
    angular_velocity = 0;
    Motor_SetPwm(PWM_STOP, PWM_STOP);
    
    Pid_SetLeftRightTarget(CalAngularLeftTarget, CalAngularRightTarget);
    
    Encoder_Reset();
    Pid_Reset();
    Odom_Reset();    
    
    turn_heading = 0;
    last_heading = Odom_GetHeading();

    do
    {
        Encoder_Update();
        Pid_Update();
        Odom_Update();
        
        heading = Odom_GetHeading();
        delta_heading = heading - last_heading;
        if (delta_heading > PI) 
        { 
            delta_heading -= 2*PI; 
        } 
        else if (delta_heading <= -PI) 
        { 
            delta_heading += 2*PI; 
        }

        turn_heading += delta_heading;
        last_heading = heading;
        
        UpdateVelocity(turn_heading);
    
    } while (abs(turn_heading) < ANGULAR_BIAS_ANGLE);
    
    left_cmd_velocity = 0;
    right_cmd_velocity = 0;
    Motor_SetPwm(PWM_STOP, PWM_STOP);
    
    debug_control_enabled = old_debug_control_enabled;    
}

void CalibrateAngularBias()
{
    Ser_PutString("\r\nPerforming angular bias calibration\r\n");

    Nvstore_WriteFloat(1.0, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->angular_bias));
    
    DoAngularBiasMotion();
                
    Ser_PutString("Enter the measured rotation in degrees (enter 5 chars), e.g., 345.0 : ");
    float meas_rotation = Cal_ReadResponse();
    float angular_bias = 360.0 / meas_rotation;
    Cal_DisplayBias("angular", angular_bias);
    
    /* Store the angular bias into EEPROM */
    Nvstore_WriteFloat(angular_bias, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->angular_bias));
    Ser_PutString("Angular bias calibration complete\r\n");
    
}

void ValidateAngularBias()
{
    /* Note: consider whether calibration should be done in both directions: CW and CCW and the average taken as the
       actual angular bias.  There would need to be some interaction with the user to mark a new starting position
       after the first motion.
     */
    
    Ser_PutString("\r\nValidating angular bias calibration\r\n");
    DoAngularBiasMotion();
    Ser_PutString("Angular bias validation complete\r\n");
}
