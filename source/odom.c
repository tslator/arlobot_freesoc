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


/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
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
static float left_dist;
static float right_dist;
static float heading;
static float angular_bias;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

#ifdef ODOM_DUMP_ENABLED
static char left_speed_str[10];
static char right_speed_str[10];
static char left_dist_str[10];
static char right_dist_str[10];
static char heading_str[10];
static uint32 last_odom_report = 0;
        
/*---------------------------------------------------------------------------------------------------
 * Name: DumpOdom
 * Description: Prints the current odmetry.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void DumpOdom()
{
    if (ODOM_DEBUG_CONTROL_ENABLED)
    {
        uint32 delta_time;
        delta_time = millis() - last_odom_report;
        if (delta_time > ODOM_SAMPLE_TIME_MS)
        {
            ftoa(left_speed, left_speed_str, 3);
            ftoa(right_speed, right_speed_str, 3);
            ftoa(left_dist, left_dist_str, 3);
            ftoa(right_dist, right_dist_str, 3);
            ftoa(heading, heading_str, 3);
            
            DEBUG_PRINT_ARG("ls: %s rs: %s ld: %s rd: %s hd: %s\r\n", left_speed_str, right_speed_str, left_dist_str, right_dist_str, heading_str);
            
            last_odom_report = millis();
        }
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
    left_speed = 0.0;
    right_speed = 0.0;
    left_dist = 0.0;
    right_dist = 0.0;
    heading = 0.0;
    
    angular_bias = 1.0;
    if (p_cal_eeprom->status & CAL_ANGULAR_BIT)
    {
        angular_bias = p_cal_eeprom->angular_bias;
    }
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
 * Name: Odom_Update
 * Description: Calculates the heading and transfers the odometry fields to the I2C interface.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Odom_Update()
{
    /* Note: All of the calculations (except for heading) are performed in the encoder module where
       there is time information.  This routine will be called on every cycle of the main loop which
       is estimated to be about 40-50 ms.  I don't think this frequency will affect i2c performance.
       The goal is to relay as fast as possible the latest odometry information.
     */
    
    left_speed = Encoder_LeftGetMeterPerSec();
    right_speed = Encoder_RightGetMeterPerSec();
    
    left_dist = Encoder_LeftGetDist();
    right_dist = Encoder_RightGetDist();

    heading = CalcHeading(left_dist, right_dist, TRACK_WIDTH, angular_bias);
    
    I2c_WriteOdom(left_speed, right_speed, left_dist, right_dist, heading);
    
    DUMP_ODOM();
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
    left_dist = 0;
    right_dist = 0;
    heading = 0;
    I2c_WriteOdom(left_speed, right_speed, left_dist, right_dist, heading);
}

float Odom_GetHeading()
{
    return heading;
}

/* [] END OF FILE */
