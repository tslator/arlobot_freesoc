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

#ifndef HWCONFIG_H
#define HWCONFIG_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <project.h>
    
/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/

/* There is a known issue with printf/sprintf and float formating which can be resolved by converting floats to string
   and then using format to include into a string.  However, the gcc compiler does not support ftoa so I had to role my
   own (stolen from others who rolled their own).  This define enables the home-grown ftoa (in utils.c)
 */
#define USE_FTOA
    
/* Testing Macros    
 */    
    
/* Enable this define and flash before running the i2c.py module test */    
//#define TEST_I2C

    
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef float (*GET_TARGET_FUNC_TYPE)();

typedef enum {WHEEL_LEFT, WHEEL_RIGHT, WHEEL_BOTH} WHEEL_TYPE;
typedef enum {DIR_FORWARD, DIR_BACKWARD, DIR_CW, DIR_CCW} DIR_TYPE;
    
    
/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------------------- 
    Communication
----------------------------------------------------------------------------------------------------------------------*/

#define COMMS_DEBUG_ENABLED


/*---------------------------------------------------------------------------------------------------------------------- 
    Sensors
----------------------------------------------------------------------------------------------------------------------*/
#define NUM_FRONT_ULTRASONIC_SENSORS (8)
#define NUM_REAR_ULTRASONIC_SENSORS (8)
#define NUM_ULTRASONIC_SENSORS (NUM_FRONT_ULTRASONIC_SENSORS + NUM_REAR_ULTRASONIC_SENSORS)
#define FIRST_FRONT_ULTRASONIC_SENSOR (0)
#define LAST_FRONT_ULTRASONIC_SENSOR (NUM_FRONT_ULTRASONIC_SENSORS - 1)
#define FIRST_REAR_ULTRASONIC_SENSOR (0)
#define LAST_REAR_ULTRASONIC_SENSOR (NUM_REAR_ULTRASONIC_SENSORS - 1)
   
#define NUM_FRONT_INFRARED_SENSORS (8)
#define NUM_REAR_INFRARED_SENSORS (8)
#define NUM_INFRARED_SENSORS (NUM_FRONT_INFRARED_SENSORS + NUM_REAR_INFRARED_SENSORS)
#define FIRST_FRONT_INFRARED_SENSOR (0)    
#define LAST_FRONT_INFRARED_SENSOR (NUM_FRONT_INFRARED_SENSORS - 1)    
#define FIRST_REAR_INFRARED_SENSOR (0)    
#define LAST_REAR_INFRARED_SENSOR (NUM_REAR_INFRARED_SENSORS - 1)    

/*---------------------------------------------------------------------------------------------------
 * Motors
 *-------------------------------------------------------------------------------------------------*/

#define PI (3.1415926535897932384626433832795)
#define WHEEL_RADIUS   (0.0785)       // meter
#define WHEEL_DIAMETER (2 * WHEEL_RADIUS)
#define TRACK_WIDTH (0.392)           // meter
#define PI_D (PI * WHEEL_DIAMETER)
#define METER_PER_REVOLUTION (PI_D)   // meter
#define ENCODER_TICK_PER_REVOLUTION (500)
#define COUNT_PER_REVOLUTION (ENCODER_TICK_PER_REVOLUTION * 4)  /* encoder tick per revolution times 4x encoder (quadrature encoding) */
#define METER_PER_COUNT (METER_PER_REVOLUTION/COUNT_PER_REVOLUTION)
#define COUNT_PER_METER (COUNTS_PER_REVOLUTION/METER_PER_REVOLUTION)
#define RADIAN_PER_COUNT (PI * (WHEEL_DIAMETER/(TRACK_WIDTH*COUNT_PER_REVOLUTION)))
#define RADIAN_PER_METER (PI * (WHEEL_DIAMETER/(TRACK_WIDTH*METER_PER_REVOLUTION)))

// Note: These are the theoretical values, so it may be prudent to back these down to more conservative values

#define MIN_LINEAR_VELOCITY     (-MAX_METER_PER_SECOND)
#define MAX_LINEAR_VELOCITY     (MAX_METER_PER_SECOND)
#define MIN_ANGULAR_VELOCITY    (-MAX_RADIAN_PER_SECOND)
#define MAX_ANGULAR_VELOCITY    (MAX_RADIAN_PER_SECOND)

#define MAX_WHEEL_RPM       (95)
#define WHEEL_CIRCUMFERENCE (2 * PI * WHEEL_RADIUS)
#define MAX_WHEEL_VELOCITY  (MAX_WHEEL_RPM * WHEEL_CIRCUMFERENCE)/60
#define MIN_LEFT_VELOCITY   (-MAX_WHEEL_VELOCITY)
#define MAX_LEFT_VELOCITY   (MAX_WHEEL_VELOCITY)
#define MIN_RIGHT_VELOCITY   (-MAX_WHEEL_VELOCITY)
#define MAX_RIGHT_VELOCITY   (MAX_WHEEL_VELOCITY)

#define MAX_METER_PER_SECOND    ((MAX_WHEEL_RPM / 60) * PI_D)
#define MAX_RADIAN_PER_SECOND   ((2 * MAX_METER_PER_SECOND)/TRACK_WIDTH)

/*----------------------------------------------------------------------------------------------------------------------

Sample Rates

----------------------------------------------------------------------------------------------------------------------*/
#define ENC_SAMPLE_RATE     (20) /* Hz */
#define PID_SAMPLE_RATE     (15) /* Hz */
#define ODOM_SAMPLE_RATE    (20) /* Hz */
#define HEARTBEAT_RATE       (2) /* Hz */
#define CTRL_VELOCITY_RATE  (15) /* Hz */
#define SENSOR_SAMPLE_RATE  (20) /* Hz */

/* The following defines and macro provide a mechanism to distribute the sampling across the main loop, i.e., keep the
   sampling from happening all of the same time, by introducing a one-time initial delay or sampling offset.
 */
#define ENC_SCHED_OFFSET    (7)  /* ms */
#define PID_SCHED_OFFSET    (19)  /* ms */
#define ODOM_SCHED_OFFSET   (11)  /* ms */


/*----------------------------------------------------------------------------------------------------------------------

Command and Control Interface

----------------------------------------------------------------------------------------------------------------------*/
/* Define one of the following */
//#define ENABLE_I2CIF
#define ENABLE_CANIF


#endif

/* [] END OF FILE */
