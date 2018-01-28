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
#include <math.h>
#include "odom.h"
#include "config.h"
#include "encoder.h"
#include "time.h"
#include "utils.h"
#include "diag.h"
#include "debug.h"
#include "cal.h"
#include "control.h"
#include "consts.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;

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
static FLOAT left_mps;
static FLOAT right_mps;
static FLOAT x_position;
static FLOAT y_position;
static FLOAT theta;
static FLOAT linear_meas_velocity;
static FLOAT angular_meas_velocity;

static FLOAT linear_bias;
static FLOAT angular_bias;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

#ifdef ODOM_DUMP_ENABLED
        
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
        DEBUG_PRINT_INFO("{\"odom\":{\"left_mps\":%.3f,\"right_mps\":%.3f,\"x_pos\":%.3f,\"y_pos\":%.3f,\"theta\":%.3f,\"lin_vel\":%.3f,\"ang_vel\":%.3f,}}\r\n",
                        left_mps, 
                        right_mps, 
                        x_position, 
                        y_position, 
                        theta,
                        linear_meas_velocity,
                        angular_meas_velocity
        );
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
    linear_bias = 1.0;//Cal_GetLinearBias();
    angular_bias = 1.0;//Cal_GetAngularBias();
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
    static UINT32 last_update_time = ODOM_SCHED_OFFSET;
    static UINT32 delta_time;
    static INT32 last_left_tick;
    static INT32 last_right_tick;
    INT32 left_delta_tick;
    INT32 right_delta_tick;
    INT32 left_tick;
    INT32 right_tick;
    
    FLOAT left_cps;
    FLOAT right_cps;
    FLOAT left_rps;
    FLOAT right_rps;
    
    ODOM_UPDATE_START();
    
    delta_time = millis() - last_update_time;
    ODOM_DEBUG_DELTA(delta_time);
    if (delta_time >= ODOM_SAMPLE_TIME_MS)
    {
        last_update_time = millis();
        
        left_tick = Encoder_LeftGetCount();
        left_delta_tick = left_tick - last_left_tick;
        last_left_tick = left_tick;
        
        right_tick = Encoder_RightGetCount();
        right_delta_tick = right_tick - last_right_tick;
        last_right_tick = right_tick;
        
        left_mps = Encoder_LeftGetMeterPerSec();
        right_mps = Encoder_RightGetMeterPerSec();
        
        left_cps = Encoder_LeftGetCntsPerSec();
        right_cps = Encoder_RightGetCntsPerSec();
        
        left_rps = left_cps * TWOPI / WHEEL_COUNT_PER_REV;
        right_rps = right_cps * TWOPI / WHEEL_COUNT_PER_REV;
    
        DiffToUni(left_rps, right_rps, &linear_meas_velocity, &angular_meas_velocity);
        
#ifdef RUNGE_KUTTA        
        /* With meter/sec wheel velocity do the following to calculate x, y, theta using Runge-Kutta (4th order)
            1. Calculate Linear/Angular velocity using DiffToUni
            2. Calculate Runge-Kutta terms
            3. Calculate new x, y, and theta 
            See rungekutta.py
            Ref: https://www.cs.cmu.edu/afs/cs.cmu.edu/academic/class/16311/www/s07/labs/NXTLabs/Lab%203.html
        */
        FLOAT dt_sec = (FLOAT) (delta_time / 1000.0);
        FLOAT dt_2_sec = dt_sec / 2.0;
        FLOAT dt_6_sec = dt_sec / 6.0;
        
        FLOAT k00 = linear * cos(theta);
        FLOAT k01 = linear * sin(theta);
        FLOAT k02 = angular;
    
        FLOAT k10 = linear * cos(theta + dt_2_sec * k02);
        FLOAT k11 = linear * sin(theta + dt_2_sec * k02);
        FLOAT k12 = angular;
    
        FLOAT k20 = linear * cos(theta + dt_2_sec * k12);
        FLOAT k21 = linear * sin(theta + dt_2_sec * k12);
        FLOAT k22 = angular;
    
        FLOAT k30 = linear * cos(theta + dt_sec * k22);
        FLOAT k31 = linear * sin(theta + dt_sec * k22);
        //FLOAT k32 = angular;

        x_position += dt_6_sec * (k00 + 2*(k10 + k20) + k30);
        y_position += dt_6_sec * (k01 + 2*(k11 + k21) + k31);

        /* In all cases, k02 = k12 = k22 = k32 = w, which reduces the theta equation from:
            theta = theta + t/6 * (w + 2(w + w) + w)
           to
            theta = theta + t/6 * 6w = theta + t*w
        */
        theta += dt_sec * angular;
#else
        //FLOAT left_delta_dist = left_mps * delta_time / 1000.0;
        //FLOAT right_delta_dist = right_mps * delta_time / 1000.0;
        //FLOAT center_delta_dist = linear_bias * (left_delta_dist + right_delta_dist) / 2.0;
    
        FLOAT left_delta_dist = 2 * PI * WHEEL_RADIUS * (FLOAT) left_delta_tick / WHEEL_COUNT_PER_REV;
        FLOAT right_delta_dist = 2 * PI * WHEEL_RADIUS * (FLOAT) right_delta_tick / WHEEL_COUNT_PER_REV;
        FLOAT center_delta_dist = linear_bias * (left_delta_dist + right_delta_dist) / 2.0;
        
        theta += angular_bias * (right_delta_dist - left_delta_dist)/TRACK_WIDTH;
        /* Constrain theta to -PI to PI */
        theta = NormalizeHeading(theta);

        x_position += center_delta_dist * cos(theta);
        y_position += center_delta_dist * sin(theta);
        
#endif        

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
    
    Control_WriteOdom(linear_meas_velocity, angular_meas_velocity, x_position, y_position, theta);
    
    DUMP_ODOM();       
}

/*---------------------------------------------------------------------------------------------------
 * Name: Odom_GetHeading
 * Description: Returns the heading based on the odometry calculations.
 * Parameters: None
 * Return: FLOAT (rad)
 * 
 *-------------------------------------------------------------------------------------------------*/
 FLOAT Odom_GetHeading()
{
    return theta;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Odom_GetMeasVelocity
 * Description: Returns the linear and angular velocity based on the odometry calculations.
 * Parameters: None
 * Return: FLOAT
 * 
 *-------------------------------------------------------------------------------------------------*/
void Odom_GetMeasVelocity(FLOAT* const linear, FLOAT* const angular)
{
   *linear = linear_meas_velocity;
   *angular = angular_meas_velocity;     
}
 
/*---------------------------------------------------------------------------------------------------
 * Name: Odom_GetXYPosition
 * Description: Returns the x/y position based on the odometry calculations.
 * Parameters: (out) x - x travel offset
 *             (out) y - y travel offset
 * Return: FLOAT
 * 
 *-------------------------------------------------------------------------------------------------*/
 void Odom_GetXYPosition(FLOAT* const x, FLOAT* const y)
{
    *x = x_position;
    *y = y_position;
} 

/* [] END OF FILE */
