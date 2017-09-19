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
   Description: This module contains configuration settings and global definitions.
 *-------------------------------------------------------------------------------------------------*/


#ifndef HWCONFIG_H
#define HWCONFIG_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <project.h>
#include <assert.h>    
    
/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/

/* Testing Macros    
 */    
    
/* Enable this define and flash before running the i2c.py module test */    
//#define TEST_I2C

    
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef enum {WHEEL_LEFT, WHEEL_RIGHT, WHEEL_BOTH} WHEEL_TYPE;
typedef enum {DIR_FORWARD, DIR_BACKWARD, DIR_CW, DIR_CCW} DIR_TYPE;
    
    
/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------------------- 
    Communication
----------------------------------------------------------------------------------------------------------------------*/

#define COMMS_DEBUG_ENABLED

/*---------------------------------------------------------------------------------------------------
 * Motors
 *-------------------------------------------------------------------------------------------------*/
#define PI (3.1415926535897932384626433832795)
#define TWOPI (2 * PI)
#define MILLIS_PER_SECOND (1000)

#define SEC_PER_MIN (60.0)
#define RADIAN_PER_REV (TWOPI)
 
#define MAX_WHEEL_RPM  (95.0)                 // rpm
#define WHEEL_RADIUS   (0.0775)             // meter
#define WHEEL_DIAMETER (2 * WHEEL_RADIUS)   // meter
#define TRACK_WIDTH (0.3937)                 // meter

#define WHEEL_CIRCUMFERENCE (PI * WHEEL_DIAMETER)          // meter
#define WHEEL_METER_PER_REV (WHEEL_CIRCUMFERENCE)         // meter
#define WHEEL_ENCODER_TICK_PER_REV (500)   // ticks or counts
#define WHEEL_COUNT_PER_REV (WHEEL_ENCODER_TICK_PER_REV * 4)  /* encoder tick per revolution times 4x encoder (quadrature encoding) */
#define WHEEL_METER_PER_COUNT (WHEEL_METER_PER_REV/WHEEL_COUNT_PER_REV)
#define WHEEL_COUNT_PER_METER (WHEEL_COUNT_PER_REV/WHEEL_METER_PER_REV)
#define WHEEL_COUNT_PER_RADIAN (WHEEL_COUNT_PER_REV / TWOPI)

/* Wheel maximum angular velocity: 
    omega = theta / sec = RPM/60 * 2PI
 */
#define MAX_WHEEL_RADIAN_PER_SECOND   ((MAX_WHEEL_RPM / SEC_PER_MIN) * RADIAN_PER_REV)
/* Wheel maximum linear velocity:
    v = omega * radius
 */
#define MAX_WHEEL_METER_PER_SECOND    (MAX_WHEEL_RADIAN_PER_SECOND * WHEEL_RADIUS)


#define MAX_WHEEL_FORWARD_LINEAR_VELOCITY  (MAX_WHEEL_METER_PER_SECOND)     // meter/sec
#define MAX_WHEEL_BACKWARD_LINEAR_VELOCITY (-MAX_WHEEL_METER_PER_SECOND)    // meter/sec
#define MAX_WHEEL_CW_ANGULAR_VELOCITY      (MAX_WHEEL_RADIAN_PER_SECOND)    // radian/sec
#define MIN_WHEEL_CCW_ANGULAR_VELOCITY     (-MAX_WHEEL_RADIAN_PER_SECOND)   // radian/sec

#define MAX_WHEEL_FORWARD_COUNT_PER_SEC (MAX_WHEEL_FORWARD_LINEAR_VELOCITY * WHEEL_COUNT_PER_METER)
#define MAX_WHEEL_BACKWARD_COUNT_PER_SEC (MAX_WHEEL_BACKWARD_LINEAR_VELOCITY * WHEEL_COUNT_PER_METER)
#define MAX_WHEEL_CW_COUNT_PER_SEC (MAX_WHEEL_CW_ANGULAR_VELOCITY / WHEEL_RADIAN_PER_COUNT)
#define MAX_WHEEL_CCW_COUNT_PER_SEC (MIN_WHEEL_CCW_ANGULAR_VELOCITY / WHEEL_RADIAN_PER_COUNT)
                

/* Calculate the maximum rotation of the robot */

/* This is the circumference of the circle made when the robot turns in place, i.e., one wheel max forward, one wheel 
   max reverse.  Only in this case is the angular velocity maximum.   
 */

/* Calculate robot max RPM 
    RPM = v * 60 / (2 * PI * r) = v * 60 / (PI * d)
    where,
        v is the linear velocity of the wheel
        r is half the track width (or using d which is the track width)
*/
#define MAX_ROBOT_RPM ((MAX_WHEEL_FORWARD_LINEAR_VELOCITY * 60) / (PI * TRACK_WIDTH))

/* Calculate angular velocity of robot rotation from linear velocity of wheel:
    circumference = (RPM / 60) * 2 * PI
 */
#define MAX_ROBOT_RADIAN_PER_SECOND ((MAX_ROBOT_RPM / SEC_PER_MIN) * RADIAN_PER_REV)

#define ROBOT_METER_PER_REV (PI * TRACK_WIDTH)
#define ROBOT_NUM_WHEEL_ROTATION_PER_ROBOT_REV (ROBOT_METER_PER_REV / WHEEL_METER_PER_REV) // num of wheel rotations per robot revolution
#define ROBOT_COUNT_PER_REV (ROBOT_NUM_WHEEL_ROTATION_PER_ROBOT_REV * WHEEL_COUNT_PER_REV) // count/robot revolution

#define MAX_ROBOT_CW_RADIAN_PER_SECOND (MAX_ROBOT_RADIAN_PER_SECOND)
#define MAX_ROBOT_CCW_RADIAN_PER_SECOND (-MAX_ROBOT_RADIAN_PER_SECOND)


/*----------------------------------------------------------------------------------------------------------------------

Sample Rates

----------------------------------------------------------------------------------------------------------------------*/
#define ENC_SAMPLE_RATE     (50) /* Hz */
#define PID_SAMPLE_RATE     (50) /* Hz */
#define ODOM_SAMPLE_RATE    (50) /* Hz */
#define HEARTBEAT_RATE      (2)  /* Hz */
#define STATUS_LED_RATE     (2)  /* Hz */

/* The following defines and macro provide a mechanism to distribute the sampling across the main loop, i.e., keep the
   sampling from happening all of the same time, by introducing a one-time initial delay or sampling offset.
 */
#define ENC_SCHED_OFFSET    (7)   /* ms */
#define PID_SCHED_OFFSET    (11)  /* ms */
#define ODOM_SCHED_OFFSET   (23)  /* ms */

/*----------------------------------------------------------------------------------------------------------------------

Command and Control Interface

----------------------------------------------------------------------------------------------------------------------*/
/* Define one of the following */
#define ENABLE_I2CIF
//#define ENABLE_CANIF


#endif

/* [] END OF FILE */
