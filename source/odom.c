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

/*---------------------------------------------------------------------------------------------------
 * Name: CalculateOdometry
 * Description: Calculates the odometry fields on each sample period.
 * Parameters: delta_time - the number of milliseconds between each call.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void CalculateOdometry(uint32 delta_time)
{
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
