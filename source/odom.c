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

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "odom.h"
#include "config.h"
#include "encoder.h"
#include "math.h"
#include "time.h"
#include "utils.h"
#include "diag.h"
#include "debug.h"
#include "cal.h"
#include "control.h"


/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef ODOM_DUMP_ENABLED
#define DUMP_ODOM()  DumpOdom()
#else
#define DUMP_ODOM()
#endif    

#define ODOM_SAMPLE_TIME_MS  SAMPLE_TIME_MS(ODOM_SAMPLE_RATE)


#ifdef ODOM_DEBUG_DELTA_ENABLED
#define ODOM_DEBUG_DELTA(delta) DEBUG_DELTA_TIME("odom", delta)
#else
#define ODOM_DEBUG_DELTA(delta)
#endif    

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static float left_speed;
static float right_speed;
static float x_position;
static float y_position;
static float theta;
static float linear_bias;
static float angular_bias;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

#ifdef ODOM_DUMP_ENABLED
static char left_speed_str[10];
static char right_speed_str[10];
static char x_position_str[10];
static char y_position_str[10];
static char theta_str[10];
static char linear_bias_str[10];
static char angular_bias_str[10];
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
    if (Debug_IsEnabled(DEBUG_ODOM_ENABLE_BIT))
    {
        uint32 delta_time;
        delta_time = millis() - last_odom_report;
        if (delta_time > ODOM_SAMPLE_TIME_MS)
        {
            ftoa(left_speed, left_speed_str, 3);
            ftoa(right_speed, right_speed_str, 3);
            ftoa(x_position, x_position_str, 3);
            ftoa(y_position, y_position_str, 3);
            ftoa(theta, theta_str, 3);
            ftoa(linear_bias, linear_bias_str, 3);
            ftoa(angular_bias, angular_bias_str, 3);
            
            DEBUG_PRINT_ARG("ls: %s rs: %s ld: %s rd: %s hd: %s ab: %s lb: %s\r\n", 
                            left_speed_str, 
                            right_speed_str, 
                            x_position_str, 
                            y_position_str, 
                            theta_str,
                            linear_bias_str,
                            angular_bias_str);
            
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
    x_position = 0.0;
    y_position = 0.0;
    theta = 0.0;
    
    angular_bias = Cal_GetAngularBias();
    linear_bias = Cal_GetLinearBias();
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
    static uint32 last_update_time = ODOM_SCHED_OFFSET;
    static uint32 delta_time;
    
    ODOM_UPDATE_START();
    
    delta_time = millis() - last_update_time;
    ODOM_DEBUG_DELTA(delta_time);
    if (delta_time >= ODOM_SAMPLE_TIME_MS)
    {
        last_update_time = millis();
        
        left_speed = Encoder_LeftGetMeterPerSec();
        right_speed = Encoder_RightGetMeterPerSec();
        
        float left_delta_dist = Encoder_LeftGetDeltaDist();
        float right_delta_dist = Encoder_RightGetDeltaDist();
        float center_delta_dist = Encoder_GetCenterDist();
        
        x_position += center_delta_dist * cos(theta);
        y_position += center_delta_dist * sin(theta);
        theta += (right_delta_dist - left_delta_dist) / TRACK_WIDTH;
        
        Control_WriteOdom(left_speed, right_speed, x_position, y_position, theta);
        
        DUMP_ODOM();
    }

    ODOM_UPDATE_END();    
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
    x_position = 0;
    y_position = 0;
    theta = 0;
    
    linear_bias = Cal_GetLinearBias();
    angular_bias = Cal_GetAngularBias();
    
    Control_WriteOdom(left_speed, right_speed, x_position, y_position, theta);
    DUMP_ODOM();       
}

float Odom_GetHeading()
{
    return theta;
}

/* [] END OF FILE */
