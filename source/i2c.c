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

#include "i2c.h"
#include "time.h"
#include "config.h"
#include "utils.h"
#include "debug.h"

/*
    I2C Communication Data Layout

    offset     num bytes     name                           description
      00           2         [control register]             the control register supports 
                                                                - Bit 0: disable the HB25 motors
                                                                - Bit 1: clear odometry (which includes the following:
                                                                    x,y distances
                                                                    heading
                                                                    linear,angular velocity
                                                                    encoder counts
                                                                - Bit 2: calibrate - requests the Psoc to start calibrating
                                                                - Bit 3: validate calibration
        <---- Commanded Velocity ---->
      02           4         [linear commanded velocity]    commanded linear velocity in meter/second
      06           4         [angular commanded velocity]   commanded angular velocity in radian/second
      10           2         [calibration port]             the register through which calibration is passed to the Psoc
                                                            when loading calibration from the Raspberry Pi and also how
                                                            calibration data is passed from the Psoc to the Raspberry Pi
                                                            for storage in a file
    ------------------------------ Read/Write Boundary --------------------------------------------
      12           2         [device status]                contains bits that represent the status of the Psoc device
                                                               - Bit 0: HB25 Motor Controller Initialized
                                                               - Bit 1: Calibrated - indicates whether the calibration
                                                                        values have been loaded; 0 - no, 1 - yes
                                                               - Bit 2: Calibrating - indicates when the Psoc is in
                                                                        calibration; 0 - no, 1 - yes
           <------ Odometry ------>
      14           4         [x distance]                   measured x distance
      18           4         [y distance]                   measured y distance
      22           4         [heading]                      measured heading
      26           4         [linear velocity]              measured linear velocity
      30           4         [angular velocity]             measured angular velocity
          <------ Ultrasonic ------>
      34           8         [front ultrasonic distance]    ultrasonic distance is an array of 
                                                            distances from the ultrasonic sensors in 
                                                            centimeters, range 2 to 500
      42           8         [rear ultrasonic distance]     ultrasonic distance is an array of 
                                                            distances from the ultrasonic sensors in 
                                                            centimeters, range 2 to 500
           <------ Infrared ------>
      50           8         [infrared distance]            infrared distance is an array of distances 
                                                            from the infrared sensors in centimeters, 
                                                            range 10 to 80
      58           8         [infrared distance]            infrared distance is an array of distances 
                                                            from the infrared sensors in centimeters, 
                                                            range 10 to 80
      66           4         [heartbeat]                    used for testing the i2c communication
 */

/* Define the portion of the I2C Slave that Read/Write */
typedef struct
{
    uint16 control;
    float  linear_cmd_velocity;
    float  angular_cmd_velocity;
    uint16 calibration_port;
} __attribute__ ((packed)) READWRITE_TYPE;

/* Define the odometry structure for communicating the position, heading and velocity of the wheel 
   Note: Each Psoc board controls a single wheel, but calculates and reports odometry for the robot
 */
typedef struct
{
    float x_dist;
    float y_dist;
    float heading;
    float linear_velocity;
    float angular_velocity;
} __attribute__ ((packed)) ODOMETRY;

/* Define the structure for ultrasonic sensors */
typedef struct
{
    uint16 front[NUM_FRONT_ULTRASONIC_SENSORS];
    uint16 rear[NUM_REAR_ULTRASONIC_SENSORS];
} __attribute__ ((packed)) ULTRASONIC;

/* Define the structure for infrared sensors */
typedef struct
{
    uint8 front[NUM_FRONT_INFRARED_SENSORS];
    uint8 rear[NUM_REAR_INFRARED_SENSORS];
} __attribute__((packed)) INFRARED;

/* Define the I2C Slave that Read Only */
typedef struct
{
    uint16     status;
    ODOMETRY   odom;
    ULTRASONIC us;
    INFRARED   ir;
    uint32     heartbeat;
} __attribute__ ((packed)) READONLY_TYPE;

/* Define the I2C Slave data interface */
typedef struct
{
    READWRITE_TYPE read_write;
    /*---------R/W Boundary -----------*/
    READONLY_TYPE read_only;
} __attribute__ ((packed)) I2C_DATASTRUCT;


/* The following structure was created to allow the encoder count to be read from the other board.  The API doesn't
   provide for reading at an offset on the slave.  So, unfortunately, we need to read from offset 0 through the encoder
   count.

   Note: It is possible to move the encoder count to the read/write section of the slave.  Something to think about.
 */
typedef struct
{
    int32  count;
} __attribute__((packed)) SLAVE_DATA_TYPE;

/* Define the I2C Slave buffer (as seen by the slave on this Psoc) */
static I2C_DATASTRUCT i2c_buf;
/* Define the I2C Slave buffer (as seen by the master on this Psoc) */
static SLAVE_DATA_TYPE i2c_slave_data;
/* Define a persistant status that can be set and cleared */
static uint16 i2c_status;


/*----------------------------------------------------------------------------------------------------------------------

  Static Functions

 ---------------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------------------------------

  Public Functions

 ---------------------------------------------------------------------------------------------------------------------*/

void I2c_Init()
{
    memset( &i2c_buf, 0, sizeof(i2c_buf));
    memset( &i2c_slave_data, 0, sizeof(i2c_slave_data));
    i2c_status = 0;
}

void I2c_Start()
{
    /* Note: The start routine must be called before the set buffer routine.  Start applies the customizations settings
       to the component including initializing the buffer pointers, so calling start after setting the buffer effectively
       re-initializes the component buffer pointers wiping out the set buffer setting.
     */
    EZI2C_Slave_Start();
    EZI2C_Slave_SetBuffer1(sizeof(i2c_buf), sizeof(i2c_buf.read_write), (volatile uint8 *) &i2c_buf);
}

uint16 I2c_ReadControl()
{
    uint16 value;
    
    value = i2c_buf.read_write.control;
    i2c_buf.read_write.control = 0;
    
    //value |= CONTROL_ENABLE_CALIBRATION_BIT;
    //value |= CONTROL_VALIDATE_CALIBRATION_BIT;
    return value;
}

#define STEP_INPUT (700)

void I2c_ReadCmdVelocity(float *linear, float *angular)
{
    *linear = max(MIN_LINEAR_VELOCITY, min(i2c_buf.read_write.linear_cmd_velocity, MAX_LINEAR_VELOCITY));
    *angular = max(MIN_ANGULAR_VELOCITY, min(i2c_buf.read_write.angular_cmd_velocity, MAX_ANGULAR_VELOCITY));
    
    //*linear = 0.2;
    //*angular = 0.0;
}

void I2c_WriteCalReg(uint16 value)
{
    i2c_buf.read_write.calibration_port = value;
}

uint16 I2c_ReadCalReg()
{
    return i2c_buf.read_write.calibration_port;
}

void I2c_SetStatusBit(uint8 bit)
{
    i2c_status |= bit;
    i2c_buf.read_only.status = i2c_status;
}

void I2c_ClearStatusBit(uint8 bit)
{
    i2c_status &= ~bit;
    i2c_buf.read_only.status = i2c_status;
}

void I2c_WriteOdom(float x_dist, float y_dist, float heading, float linear_speed, float angular_speed)
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
    
    i2c_buf.read_only.odom.x_dist = x_dist;
    i2c_buf.read_only.odom.y_dist = y_dist;
    i2c_buf.read_only.odom.heading = heading;
    i2c_buf.read_only.odom.linear_velocity = linear_speed;
    i2c_buf.read_only.odom.angular_velocity = angular_speed;
    
    DEBUG_PRINT("x: %s, y: %s, h: %s, l: %s, a: %s\r\n", x_dist_str, y_dist_str, heading_str, linear_str, angular_str);
}

void I2c_WriteFrontUltrasonicDistance(uint8 offset, uint16 distance)
{
    if (offset >= FIRST_FRONT_ULTRASONIC_SENSOR && offset <= LAST_FRONT_ULTRASONIC_SENSOR)
    {
        i2c_buf.read_only.us.front[offset] = distance;
    }
}

void I2c_WriteRearUltrasonicDistance(uint8 offset, uint16 distance)
{
    if (offset >= FIRST_REAR_ULTRASONIC_SENSOR && offset <= LAST_REAR_ULTRASONIC_SENSOR)
    {
        i2c_buf.read_only.us.rear[offset] = distance;
    }            
}

void I2c_WriteFrontInfraredDistance(uint8 offset, uint8 distance)
{
    if (offset >= FIRST_REAR_INFRARED_SENSOR && offset <= LAST_REAR_INFRARED_SENSOR)
    {
        i2c_buf.read_only.ir.front[offset] = distance;
    }
}

void I2c_WriteRearInfraredDistance(uint8 offset, uint8 distance)
{
    if (offset >= FIRST_REAR_INFRARED_SENSOR && offset <= LAST_REAR_INFRARED_SENSOR)
    {
        i2c_buf.read_only.ir.rear[offset] = distance;
    }            
}

void I2c_UpdateHeartbeat()
{
    i2c_buf.read_only.heartbeat++;
}

/* [] END OF FILE */
