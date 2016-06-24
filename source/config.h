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
    
#include <project.h>
    
/* There is a known issue with printf/sprintf and float formating which can be resolved by converting floats to string
   and then using format to include into a string.  However, the gcc compiler does not support ftoa so I had to role my
   own (stolen from others who rolled their own).  This define enables the home-grown ftoa (in utils.c)
 */
#define USE_FTOA
    
typedef float (*GET_TARGET_TYPE)();
typedef float (*GET_ENCODER_TYPE)();
typedef int16 (*GET_MOTOR_TYPE)();
typedef void (*SET_MOTOR_TYPE)(float cps);
typedef uint16 (*GET_MOTOR_PWM_TYPE)();

typedef enum {LEFT_WHEEL, RIGHT_WHEEL} WHEEL_TYPE;
typedef enum {FORWARD_DIR, BACKWARD_DIR} DIR_TYPE;
    
    
/*---------------------------------------------------------------------------------------------------------------------- 
    Communication
----------------------------------------------------------------------------------------------------------------------*/

//#define COMMS_DEBUG_ENABLED


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

#define PI (3.1415926535897932384626433832795)
#define WHEEL_DIAMETER (0.1524)
#define TRACK_WIDTH (0.403)             // meter
#define METER_PER_REVOLUTION (0.4787)   // meter
#define COUNT_PER_REVOLUTION (36.0 * 4)  /* 36 teeth per revolution (4x encoder) */
#define METER_PER_COUNT (METER_PER_REVOLUTION/COUNT_PER_REVOLUTION)
#define COUNT_PER_METER (COUNTS_PER_REVOLUTION/METER_PER_REVOLUTION)
#define RADIAN_PER_COUNT (PI * (WHEEL_DIAMETER/(TRACK_WIDTH*COUNT_PER_REVOLUTION)))
#define RADIAN_PER_METER (PI * (WHEEL_DIAMETER/(TRACK_WIDTH*METER_PER_REVOLUTION)))



// Note: These are the theoretical values, so it may be prudent to back these down to more conservative values
#define MAX_METER_PER_SECOND    (0.7579)
#define MAX_RADIAN_PER_SECOND   ((2 * MAX_METER_PER_SECOND)/TRACK_WIDTH)

#define MIN_LINEAR_VELOCITY     (-MAX_METER_PER_SECOND)
#define MAX_LINEAR_VELOCITY     (MAX_METER_PER_SECOND)
#define MIN_ANGULAR_VELOCITY    (-MAX_RADIAN_PER_SECOND)
#define MAX_ANGULAR_VELOCITY    (MAX_RADIAN_PER_SECOND)

/*----------------------------------------------------------------------------------------------------------------------

Sample Rates

----------------------------------------------------------------------------------------------------------------------*/
#define ENC_SAMPLE_RATE     (20) /* Hz */
#define PID_SAMPLE_RATE     (20) /* Hz */
#define ODOM_SAMPLE_RATE    (20) /* Hz */
#define HEARTBEAT_RATE      (2)  /* Hz */
#define CONTROL_RATE        (10) /* Hz */

/* The following defines and macro provide a mechanism to distribute the sampling across the main loop, i.e., keep the
   sampling from happening all of the same time, by introducing a one-time initial delay or sampling offset.
 */
#define ENC_SCHED_OFFSET    (7)  /* ms */
#define PID_SCHED_OFFSET    (19)  /* ms */
#define ODOM_SCHED_OFFSET   (11)  /* ms */

#define APPLY_SCHED_OFFSET(offset, applied) do {                        \
                                                if (!applied)           \
                                                {                       \
                                                    applied = 1;        \
                                                    CyDelay(offset);    \
                                                }                       \
                                            } while (0);





#define CAL_LINEAR_SCALE_CORRECTION (1.085)
#define CAL_ANGULAR_SCALE_CORRECTION (1.13)



#endif

/* [] END OF FILE */
