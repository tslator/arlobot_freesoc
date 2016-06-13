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

#ifdef ODOM_UPDATE_DELTA_ENABLED
#define ODOM_DEBUG_DELTA(delta) DEBUG_DELTA_TIME("odom", delta)
#else
#define ODOM_DEBUG_DELTA(delta)
#endif    

#define ODOM_SAMPLE_TIME_MS  SAMPLE_TIME_MS(ODOM_SAMPLE_RATE)

#define MILLIMETER_COUNT_SAMPLE_MILLIMETER (MILLIMETER_PER_COUNT / TRACK_WIDTH)

static int32 last_left_count;
static int32 last_right_count;
static float heading;
static float x_dist;
static float y_dist;
static float linear_speed;
static float angular_speed;

void Odom_Init()
{
    last_left_count = 0;
    last_right_count = 0;
    heading = 0;
    x_dist = 0;
    y_dist = 0;
    linear_speed = 0;
    angular_speed = 0;
}

void Odom_Start()
{
}

void calc_odom()
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
    int32 left_count;
    int32 right_count;
    int32 delta_left_count;
    int32 delta_right_count;
    float delta_heading;
    float delta_dist;
    float delta_x_dist;
    float delta_y_dist;
    
    left_speed = Encoder_LeftGetMmPerSec();
    right_speed = Encoder_RightGetMmPerSec();
    
    left_count = Encoder_LeftGetCount();
    right_count = Encoder_RightGetCount();
    delta_left_count = left_count - last_left_count;
    delta_right_count = right_count - last_right_count;
    last_left_count = left_count;
    last_right_count = right_count;
    
    /* Calculate heading, limit heading to -Pi <= heading < Pi, and update
                       delta count diff   meter   sample
       delta heading = ----------------- X ----- X ------
                          sample           count   meter
    */
    delta_heading = (delta_right_count - delta_left_count) * MILLIMETER_COUNT_SAMPLE_MILLIMETER;
    heading += delta_heading;

    // Calculate x/y distance and update
    delta_dist = 0.5 * (delta_left_count + delta_right_count) * MILLIMETER_PER_COUNT;
    delta_x_dist = delta_dist * cos(heading);
    delta_y_dist = delta_dist * sin(heading);

    x_dist += delta_x_dist;
    y_dist += delta_y_dist;
    
    linear_speed = (right_speed + left_speed) / 2;
    angular_speed = (right_speed - left_speed) / TRACK_WIDTH;
    
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
        
        calc_odom();
        I2c_WriteOdom(x_dist, y_dist, heading, linear_speed, angular_speed);
    }
}

void Odom_Reset()
{
    last_left_count = 0;
    last_right_count = 0;
    heading = 0;
    x_dist = 0;
    y_dist = 0;
    linear_speed = 0;
    angular_speed = 0;
}

/* [] END OF FILE */
