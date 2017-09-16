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
   Description: This module provides functionality for computing and transmitting odometry.
 *-------------------------------------------------------------------------------------------------*/

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
static float left_mps;
static float right_mps;
static float x_position;
static float y_position;
static float theta;
static float linear_meas_velocity;
static float angular_meas_velocity;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

#ifdef ODOM_DUMP_ENABLED
static float linear_bias;
static float angular_bias;
static MOVING_AVERAGE_FLOAT_TYPE linear_meas_velocity_ma;
static MOVING_AVERAGE_FLOAT_TYPE angular_meas_velocity_ma;
        
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
        DEBUG_PRINT_ARG("ls: %.3f rs: %.3f x: %.3f y: %.3f th: %.3f lv: %.3f av: %.3f lb: %.3f ab: %.3f\r\n", 
                        left_mps, 
                        right_mps, 
                        x_position, 
                        y_position, 
                        theta,
                        linear_meas_velocity,
                        angular_meas_velocity,
                        linear_bias,
                        angular_bias);
            
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
    left_mps = 0.0;
    right_mps = 0.0;
    x_position = 0.0;
    y_position = 0.0;
    theta = 0.0;
    linear_meas_velocity = 0.0;
    angular_meas_velocity = 0.0;
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
#ifdef ODOM_DUMP_ENABLED
    linear_bias = Cal_GetLinearBias();
    angular_bias = Cal_GetAngularBias();
#endif
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
        
        left_mps = Encoder_LeftGetMeterPerSec();
        right_mps = Encoder_RightGetMeterPerSec();
    
        DiffToUni(left_mps, right_mps, &linear_meas_velocity, &angular_meas_velocity);
        
#ifdef RUNGE_KUTTA        
        /* With meter/sec wheel velocity do the following to calculate x, y, theta using Runge-Kutta (4th order)
            1. Calculate Linear/Angular velocity using DiffToUni
            2. Calculate Runge-Kutta terms
            3. Calculate new x, y, and theta 
            See rungekutta.py
            Ref: https://www.cs.cmu.edu/afs/cs.cmu.edu/academic/class/16311/www/s07/labs/NXTLabs/Lab%203.html
        */
        float dt_sec = (float) (delta_time / 1000.0);
        float dt_2_sec = dt_sec / 2.0;
        float dt_6_sec = dt_sec / 6.0;
        
        float delta_x = center_delta_dist * cos(theta);
        float delta_y = center_delta_dist * sin(theta);

        x_position += cos(theta) * delta_x - sin(theta) * delta_y;
        y_position += sin(theta) * delta_x + cos(theta) * delta_y;
        theta += (right_delta_dist - left_delta_dist) / TRACK_WIDTH;
        float k00 = linear * cos(theta);
        float k01 = linear * sin(theta);
        float k02 = angular;
    
        float k10 = linear * cos(theta + dt_2_sec * k02);
        float k11 = linear * sin(theta + dt_2_sec * k02);
        float k12 = angular;
    
        float k20 = linear * cos(theta + dt_2_sec * k12);
        float k21 = linear * sin(theta + dt_2_sec * k12);
        float k22 = angular;
    
        float k30 = linear * cos(theta + dt_sec * k22);
        float k31 = linear * sin(theta + dt_sec * k22);
        //float k32 = angular;

        x_position += dt_6_sec * (k00 + 2*(k10 + k20) + k30);
        y_position += dt_6_sec * (k01 + 2*(k11 + k21) + k31);

        /* In all cases, k02 = k12 = k22 = k32 = w, which reduces the theta equation from:
            theta = theta + t/6 * (w + 2(w + w) + w)
           to
            theta = theta + t/6 * 6w = theta + t*w
        */
        theta += dt_sec * angular;
#else
        float left_delta_dist = left_mps * delta_time / 1000.0;
        float right_delta_dist = right_mps * delta_time / 1000.0;
        float center_delta_dist = (left_delta_dist + right_delta_dist) / 2.0;
        
        theta += (right_delta_dist - left_delta_dist)/TRACK_WIDTH;
        x_position += center_delta_dist * cos(theta);
        y_position += center_delta_dist * sin(theta);
        
#endif        
        /* Constrain theta to -PI to PI */
        theta = NormalizeHeading(theta);

        Control_WriteOdom(linear_meas_velocity, angular_meas_velocity, x_position, y_position, theta);
        
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
    x_position = 0;
    y_position = 0;
    theta = 0;
    linear_meas_velocity = 0;
    angular_meas_velocity = 0;
    
    linear_bias = Cal_GetLinearBias();
    angular_bias = Cal_GetAngularBias();
    
    Control_WriteOdom(linear_meas_velocity, angular_meas_velocity, x_position, y_position, theta);
    DUMP_ODOM();       
}

float Odom_GetHeading()
{
    return theta;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Odom_GetMeasVelocity
 * Description: Returns the linear and angular velocity based on the odometry calculations.
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
 void Odom_GetMeasVelocity(float *linear, float *angular)
 {
    *linear = linear_meas_velocity;
    *angular = angular_meas_velocity;     
 }
 
 
/* [] END OF FILE */
