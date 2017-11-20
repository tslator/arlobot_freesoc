#ifndef CONSTS_H
#define CONSTS_H

#include "freesoc.h"

#define TRUE (true)
#define FALSE (false)    
    
/*---------------------------------------------------------------------------------------------------
 * Motors
 *-------------------------------------------------------------------------------------------------*/
#define PI (M_PI)   /* Note: M_PI is defined in <math.h> */
#define TWOPI (2 * PI)
#define MILLIS_PER_SECOND (1000)

#define SEC_PER_MIN (60.0)
#define RADIAN_PER_REV (TWOPI)
 
#define MAX_WHEEL_RPM  (95.0)               // rpm
#define WHEEL_RADIUS   (0.0775)             // meter
#define WHEEL_DIAMETER (2 * WHEEL_RADIUS)   // meter
#define TRACK_WIDTH (0.3968)                // meter

#define WHEEL_CIRCUMFERENCE (PI * WHEEL_DIAMETER)          // meter  Note: 0.4869
#define WHEEL_METER_PER_REV (WHEEL_CIRCUMFERENCE)         // meter
#define WHEEL_ENCODER_TICK_PER_REV (500)   // ticks or counts
#define WHEEL_COUNT_PER_REV (WHEEL_ENCODER_TICK_PER_REV * 4)  /* encoder tick per revolution times 4x encoder (quadrature encoding) */
#define WHEEL_METER_PER_COUNT (WHEEL_METER_PER_REV/WHEEL_COUNT_PER_REV)
#define WHEEL_COUNT_PER_METER (WHEEL_COUNT_PER_REV/WHEEL_METER_PER_REV)
#define WHEEL_COUNT_PER_RADIAN (WHEEL_COUNT_PER_REV / TWOPI)

/* Wheel maximum radian/second: 
    omega = theta / sec = RPM/60 * 2PI
 */
#define MAX_WHEEL_RADIAN_PER_SECOND   ((MAX_WHEEL_RPM / SEC_PER_MIN) * RADIAN_PER_REV)

/* Wheel maximum count/second:
    vc = omega * count / radian
*/
#define MAX_WHEEL_COUNT_PER_SECOND    (MAX_WHEEL_RADIAN_PER_SECOND * WHEEL_COUNT_PER_RADIAN)

/* Wheel maximum meter/second:
    vm = vc * meter / count
 */
#define MAX_WHEEL_METER_PER_SECOND    (MAX_WHEEL_COUNT_PER_SECOND * WHEEL_METER_PER_COUNT)

#define MAX_WHEEL_FORWARD_LINEAR_VELOCITY  (MAX_WHEEL_METER_PER_SECOND)     // meter/sec
#define MAX_WHEEL_BACKWARD_LINEAR_VELOCITY (-MAX_WHEEL_METER_PER_SECOND)    // meter/sec
#define MAX_WHEEL_CW_ANGULAR_VELOCITY      (MAX_WHEEL_RADIAN_PER_SECOND)    // radian/sec
#define MIN_WHEEL_CCW_ANGULAR_VELOCITY     (-MAX_WHEEL_RADIAN_PER_SECOND)   // radian/sec

#define MAX_WHEEL_FORWARD_COUNT_PER_SEC (MAX_WHEEL_RADIAN_PER_SECOND * WHEEL_COUNT_PER_RADIAN)
#define MAX_WHEEL_BACKWARD_COUNT_PER_SEC (-MAX_WHEEL_RADIAN_PER_SECOND * WHEEL_COUNT_PER_RADIAN)
#define MAX_WHEEL_CW_COUNT_PER_SEC (MAX_WHEEL_CW_ANGULAR_VELOCITY / WHEEL_RADIAN_PER_COUNT)
#define MAX_WHEEL_CCW_COUNT_PER_SEC (MIN_WHEEL_CCW_ANGULAR_VELOCITY / WHEEL_RADIAN_PER_COUNT)
                

/* Calculate the maximum rotation of the robot */

/* This is the circumference of the circle made when the robot turns in place, i.e., one wheel max forward, one wheel 
   max reverse.  Only in this case is the angular velocity maximum.   
 */

/* Calculate angular velocity of robot rotation from linear velocity of wheel:
    v = w * r => w = v / r
    where,
        r is half the track width
        v is MAX_WHEEL_METER_PER_SECOND
 */
#define MAX_ROBOT_RADIAN_PER_SECOND (MAX_WHEEL_METER_PER_SECOND / (TRACK_WIDTH / 2))

/* Calculate robot max RPM 
    w = 2 * PI * n / 60 => n = (w * 60) / (2 * PI) 
    where,
        w is the angular velocity of the robot
*/
#define MAX_ROBOT_RPM ((MAX_ROBOT_RADIAN_PER_SECOND * 60) / (PI * TRACK_WIDTH))


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


#endif