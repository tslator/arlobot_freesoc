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
#define I2C_ADDRESS (0x08)

#define LEFT_PID_DUMP_ENABLED
#define LEFT_ENC_DUMP_ENABLED

#define RIGHT_PID_DUMP_ENABLED
#define RIGHT_ENC_DUMP_ENABLED

#define MOTOR_DUMP_ENABLED

#define ENC_UPDATE_DELTA_ENABLED
#define PID_UPDATE_DELTA_ENABLED
#define ODOM_UPDATE_DELTA_ENABLED

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


#define TRACK_WIDTH (0.403)             // meter
#define METER_PER_REVOLUTION (0.4787)   // meter
#define COUNTS_PER_REVOLUTION (36.0 * 4)  /* 36 teeth per revolution (4x encoder) */
#define METER_PER_COUNT (METER_PER_REVOLUTION/COUNTS_PER_REVOLUTION)
#define COUNT_PER_METER (COUNTS_PER_REVOLUTION/METER_PER_REVOLUTION)

// Note: These are the theoretical values, so it may be prudent to back these down to more conservative values
#define MAX_METER_PER_SECOND    (0.7579)
#define MAX_RADIAN_PER_SECOND   ((2 * MAX_METER_PER_SECOND)/TRACK_WIDTH)

#define MIN_LINEAR_VELOCITY     (-MAX_METER_PER_SECOND)
#define MAX_LINEAR_VELOCITY     (MAX_METER_PER_SECOND)
#define MIN_ANGULAR_VELOCITY    (MAX_RADIAN_PER_SECOND)
#define MAX_ANGULAR_VELOCITY    (-MAX_RADIAN_PER_SECOND)


    


/* The unity range used for mapping forward/reverse adjustments to the PWM output */
#define MIN_UNITY_VALUE (-1.0)
#define MAX_UNITY_VALUE (1.0)

/*----------------------------------------------------------------------------------------------------------------------

Sample Rates

----------------------------------------------------------------------------------------------------------------------*/
#define ENC_SAMPLE_RATE     (20) /* Hz */
#define PID_SAMPLE_RATE     (20) /* Hz */
#define ODOM_SAMPLE_RATE    (10) /* Hz */
#define HEARTBEAT_RATE      (2)  /* Hz */
#define CONTROL_RATE        (20) /* Hz */

/* The following defines and macro provide a mechanism to distribute the sampling across the main loop, i.e., keep the
   sampling from happening all of the same time, by introducing a one-time initial delay or sampling offset.
 */
#define ENC_SCHED_OFFSET    (0)  /* ms */
#define PID_SCHED_OFFSET    (10)  /* ms */
#define ODOM_SCHED_OFFSET   (15)  /* ms */

#define APPLY_SCHED_OFFSET(offset, applied) do {                        \
                                                if (!applied)           \
                                                {                       \
                                                    applied = 1;        \
                                                    CyDelay(offset);    \
                                                }                       \
                                            } while (0);

/*----------------------------------------------------------------------------------------------------------------------

PID Tuning

----------------------------------------------------------------------------------------------------------------------*/



#endif

/* [] END OF FILE */
