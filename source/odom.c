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

#include "odom.h"
#include "config.h"
#include "encoder.h"
#include "math.h"
#include "time.h"
#include "i2c.h"
#include "utils.h"
#include "diag.h"
#include "debug.h"

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

static float heading;
static float x_dist;
static float y_dist;
static float linear_speed;
static float angular_speed;

#ifdef ODOM_DUMP_ENABLED
static void DumpOdom()
{
    char x_dist_str[10];
    char y_dist_str[10];
    char heading_str[10];
    char linear_str[10];
    char angular_str[10];
    
    ftoa(x_dist, x_dist_str, 3);
    ftoa(y_dist, y_dist_str, 3);
    ftoa(heading, heading_str, 3);
    ftoa(linear_speed, linear_str, 3);
    ftoa(angular_speed, angular_str, 3);
    
    DEBUG_PRINT_STR("x: %s, y: %s, h: %s, l: %s, a: %s\r\n", x_dist_str, y_dist_str, heading_str, linear_str, angular_str);
}
#endif

void Odom_Init()
{
    heading = 0;
    x_dist = 0;
    y_dist = 0;
    linear_speed = 0;
    angular_speed = 0;
}

void Odom_Start()
{
}

void calc_odom(uint32 delta_time)
/*
    Calculate the following:
        heading
            Requires: left/right delta count
        x/y distance
            Requires: heading and dist per count
        linear/angular velocity
            Requires: left/right delta count, left/right speed and track width
 */
{
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
    delta_heading = (delta_left_dist - delta_right_dist) * RADIAN_PER_METER;
    heading += delta_heading;

    // Constrain heading to -Pi to Pi
    if (heading > PI)
    {
        heading -= 2*PI;
    }
    else if (heading <= -PI)
    {
        heading += 2*PI;
    }

    // Calculate x/y distance and update
    delta_dist = 0.5 * (delta_left_dist + delta_right_dist);
    delta_x_dist = delta_dist * cos(heading);
    delta_y_dist = delta_dist * sin(heading);

    x_dist += delta_x_dist;
    y_dist += delta_y_dist;
    
    linear_speed = constrain((right_speed + left_speed) / 2, MIN_LINEAR_VELOCITY, MAX_LINEAR_VELOCITY);
    angular_speed = constrain((right_speed - left_speed) / TRACK_WIDTH, MIN_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY);
}

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
        
        calc_odom(delta_time);
        I2c_WriteOdom(x_dist, y_dist, heading, linear_speed, angular_speed);
        DUMP_ODOM();
    }
}

void Odom_Reset()
{
    heading = 0;
    x_dist = 0;
    y_dist = 0;
    linear_speed = 0;
    angular_speed = 0;
    I2c_WriteOdom(x_dist, y_dist, heading, linear_speed, angular_speed);
}

/* [] END OF FILE */
