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
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "odom.h"
#include "config.h"
#include "encoder.h"
#include "math.h"
#include "time.h"
#include "i2c.h"
#include "utils.h"
#include "diag.h"
#include "debug.h"
#include "cal.h"


/*
    References:

    http://rossum.sourceforge.net/papers/DiffSteer/
    http://www-personal.umich.edu/~johannb/Papers/pos96rep.pdf
    Add a reference to Control of Mobile Robots

 */


/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef ODOM_UPDATE_DELTA_ENABLED
#define ODOM_DEBUG_DELTA(delta) DEBUG_DELTA_TIME("odom", delta)
#else
#define ODOM_DEBUG_DELTA(delta)
#endif    

#ifdef ODOM_DUMP_ENABLED
#define DUMP_ODOM()  DumpOdom()
#else
#define DUMP_ODOM()
#endif    

#define ODOM_SAMPLE_TIME_MS  SAMPLE_TIME_MS(ODOM_SAMPLE_RATE)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static float left_speed;
static float right_speed;
static float left_delta_dist;
static float right_delta_dist;
static float heading;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

#ifdef ODOM_DUMP_ENABLED
/*---------------------------------------------------------------------------------------------------
 * Name: Nvstore_Start
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void DumpOdom()
{
    char left_speed_str[10];
    char right_speed_str[10];
    char left_delta_dist_str[10];
    char right_delta_dist_str[10];
    char heading_str[10];
    
    ftoa(left_speed, left_speed_str, 3);
    ftoa(right_speed, right_speed_str, 3);
    ftoa(left_delta_dist, left_delta_dist_str, 3);
    ftoa(right_delta_dist, right_delta_dist_str, 3);
    ftoa(heading, heading_str, 3);
    
    if (ODOM_DEBUG_CONTROL_ENABLED)
    {
        DEBUG_PRINT_ARG("ls: %s rs: %s ld: %s rd: %s hd: %s\r\n", left_speed_str, right_speed_str, left_delta_dist_str, right_delta_dist_str, heading_str);
    }
}
#endif

/*---------------------------------------------------------------------------------------------------
 * Name: Nvstore_Start
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Odom_Init()
{
    left_speed = 0;
    right_speed = 0;
    left_delta_dist = 0;
    right_delta_dist = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Nvstore_Start
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Odom_Start()
{
}

#define NO_HEADING_CALC
/*---------------------------------------------------------------------------------------------------
 * Name: CalculateOdometry
 * Description: Calculates the odometry fields on each sample period.
 * Parameters: delta_time - the number of milliseconds between each call.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalculateOdometry(uint32 delta_time)
{
#ifdef NO_HEADING_CALC
    // Propose different values to be returned for odometry
    // left speed, right speed, delta left dist, delta right dist
    //
    // There doesn't seem to be a good reason to calculate heading, linear and angular velocity on the controller
    // I think that this was done previously because there was only a laptop and the propeller board.  However, in this
    // architecture there is the psoc, rpi, and PC.  The rpi is capable of making this calculations every 100ms or 
    // maybe even faster.
    
    static float last_left_dist;
    static float last_right_dist;
    float left_dist;
    float right_dist;
    
    left_speed = Encoder_LeftGetMeterPerSec();
    right_speed = Encoder_RightGetMeterPerSec();
    left_dist = Encoder_LeftGetDist();
    right_dist = Encoder_RightGetDist();

    heading = (left_dist - right_dist)/TRACK_WIDTH;
    
    left_delta_dist = left_dist - last_left_dist;
    right_delta_dist = right_dist - last_right_dist;
    
    last_left_dist = left_dist;
    last_right_dist = right_dist;
#endif    
#ifdef ALTERNATE_USING_CONTROL_ROBOT_EQS
    uint32 l_count = Encoder_LeftGetCount();
    uint32 r_count = Encoder_RightGetCount();
    
    uint32 l_diff = l_count - last_l_count;
    uint32 r_diff = r_count - last_r_count;
    
    float l_dist = l_diff * METER_PER_COUNT;
    float r_dist = l_diff * METER_PER_COUNT;
    theta += delta_time*(r_dist - l_dist)/TRACK_WIDTH;
    float c_dist = (l_dist + r_dist)/2;
    x_dist += c_dist*cos(theta)*delta_time;
    y_dist += c_dist*sin(theta)*delta_time;
    l_vel = Encoder_LeftGetMeterPerSec();
    r_vel = Encoder_RightGetMeterPerSec();
    lin_vel = (r_vel + l_vel)/2;
    ang_vel = (r_vel - l_vel)/TRACK_WIDTH;
    last_l_count = l_count;
    last_r_count = r_count;
#endif
#ifdef ORIGINAL_FROM_ARLOBOT_ON_PARALLAX
    float left_speed;
    float right_speed;
    float delta_heading;
    float delta_dist;
    float delta_x_dist;
    float delta_y_dist;
    float delta_left_dist;
    float delta_right_dist;
    
    
    left_speed = Encoder_LeftGetMeterPerSec();
    right_speed = Encoder_RightGetMeterPerSec();
    
    delta_left_dist = left_speed * delta_time / 1000;
    delta_right_dist = right_speed * delta_time / 1000;        
        
    /* Calculate heading, limit heading to -Pi <= heading < Pi, and update
                                    radian
       delta heading = delta dist X ------
                                     meter
    */
    delta_heading = ((delta_left_dist - delta_right_dist) / TRACK_WIDTH) * p_cal_eeprom->angular_bias;
    heading += delta_heading;

    // Constrain heading to -Pi to Pi
    constrain_angle(heading);
    
    // Calculate x/y distance and update
    delta_dist = 0.5 * (delta_left_dist + delta_right_dist) * p_cal_eeprom->linear_bias;
    delta_x_dist = delta_dist * cos(heading);
    delta_y_dist = delta_dist * sin(heading);

    x_dist += delta_x_dist;
    y_dist += delta_y_dist;
    
    //linear_speed = constrain((right_speed + left_speed) / 2, MIN_LINEAR_VELOCITY, MAX_LINEAR_VELOCITY);
    //angular_speed = constrain((right_speed - left_speed) / TRACK_WIDTH, MIN_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY);
    DiffToUni(left_speed, right_speed, &linear_speed, &angular_speed);
    linear_speed = constrain(linear_speed, MIN_LINEAR_VELOCITY, MAX_LINEAR_VELOCITY);
    angular_speed = constrain(angular_speed, MIN_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY);
#endif    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Odom_Update
 * Description: Updates the odometry fields.  This function is called from the main loop and enforces
 *              the sampling period for reporting odometry.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Odom_Update()
{
    static uint32 last_odom_time = 0;
    static uint8 odom_sched_offset_applied = 0;
    uint32 delta_time;
        
    delta_time = millis() - last_odom_time;
    ODOM_DEBUG_DELTA(delta_time);
    if (delta_time > ODOM_SAMPLE_TIME_MS)
    {
        last_odom_time = millis();
        
        APPLY_SCHED_OFFSET(ODOM_SCHED_OFFSET, odom_sched_offset_applied);
        
        CalculateOdometry(delta_time);
        I2c_WriteOdom(left_speed, right_speed, left_delta_dist, right_delta_dist);
        DUMP_ODOM();
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Odom_Reset
 * Description: Resets the odometry fields and updates them in the I2C interface.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Odom_Reset()
{
    left_speed = 0;
    right_speed = 0;
    left_delta_dist = 0;
    right_delta_dist = 0;
    heading = 0;
    I2c_WriteOdom(left_speed, right_speed, left_delta_dist, right_delta_dist);
}

/* [] END OF FILE */
