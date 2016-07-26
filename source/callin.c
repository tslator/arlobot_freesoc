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

static uint8 Init(CAL_STAGE_TYPE stage, void *params);
static uint8 Start(CAL_STAGE_TYPE stage, void *params);
static uint8 Update(CAL_STAGE_TYPE stage, void *params);
static uint8 Stop(CAL_STAGE_TYPE stage, void *params);
static uint8 Results(CAL_STAGE_TYPE stage, void *params);

static float left_cmd_velocity;
static float right_cmd_velocity;
static uint32 last_time;

static CALIBRATION_TYPE linear_calibration = {CAL_INIT_STATE,
                                              CAL_CALIBRATE_STAGE,
                                              0,
                                              Init,
                                              Start,
                                              Update,
                                              Stop,
                                              Results};

/* The purpose of linear calibration will be to set the gains for the linear velocity PID.  The linear velocity PID
   operates from within control and ensures that the commanded linear velocity is followed.  Its not clear at this
   point how useful the linear velocity PID will be.  Each wheel has a PID controller whose purpose is to drive the
   wheel at the specified velocity.  However, after left/right PID calibration the motor velocities are close but
   they vary +/-.  Possibily the linear velocity PID could smooth the variations.  The velocity returned by odometry
   is an average.  Its not clear if that will help or hurt.  The motion here will include a ramp up to prevent wheel
   slipage.  Tracking will be done via the x distance parameter of odometry.
 
   Is it correct to say, that if the robot doesn't move the correct distance it is because it is not traveling at the
   expected speed?  This isn't a timed motion, e.g., move at 0.1 m/s for 10 seconds to get 1 meter.  We're relying on
   the accuracy of the encoding to tell us how far we've traveled.  Maybe there doesn't need to be a linear pid, just
   a linear bias?
 
   The calibration the linear velocity PID, the following is done:
 
   Linear Velocity PID
   1. Set the linear gains (try 0, 0, 0 to start)
   2. Move the robot forward 1 meter
   3. If the robot travel is less than or greater than 1 meter repeat.
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
    CalLin_Calibration = &linear_calibration;
}

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
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    
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
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);

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

