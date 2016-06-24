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
#include "cal.h"

#define MAX_CMD_VELOCITY_TIMEOUT (2000)

/* Macro to wait for any outstanding master writes to the I2C buffer.  This ensures data integrity across the interface
 */
#define I2C_WAIT_FOR_ACCESS()   do  \
                                {   \
                                } while (0 !=  EZI2C_Slave_GetActivity())
/*
    I2C Communication Data Layout

    offset     num bytes     name                           description
      00           2         [device control]               the control register supports 
                                                                - Bit 0: disable the HB25 motors
                                                                - Bit 1: clear odometry (which includes the following:
                                                                    x,y distances
                                                                    heading
                                                                    linear,angular velocity
                                                                    encoder counts
      02           2         [debug control]                the debug register supports
                                                                - Bit 0: Enable/Disable Encoder Debug
                                                                - Bit 1: Enable/Disable PID debug
                                                                - Bit 2: Enable/Disable Motor debug
                                                                - Bit 3: Enable/Disable Odometry debug
                                                                - Bit 4: Enable/Disable Sample debug
      04           2         [calibration control]          the calibration register controls robot calibration
                                                                - bit 0: Count/Sec to PWM
                                                                - bit 1: PID
                                                                - bit 2: Linear Bias
                                                                - bit 3: Angular Bias
                                                                - bit 7: Verbose (writes data to serial port)
        <---- Commanded Velocity ---->
      06           4         [linear commanded velocity]    commanded linear velocity in meter/second
      10           4         [angular commanded velocity]   commanded angular velocity in radian/second
    ------------------------------ Read/Write Boundary --------------------------------------------
      14           2         [device status]                contains bits that represent the status of the Psoc device
                                                               - Bit 0: HB25 Motor Controller Initialized
      16           2         [calibration status]           contains bits that represent the calibration state
                                                               - Bit 0: Count/Sec to PWM
                                                               - Bit 1: PID
                                                               - Bit 2: Linear Bias
                                                               - Bit 3: Angular Bias
           <------ Odometry ------>
      18           4         [x distance]                   measured x distance
      22           4         [y distance]                   measured y distance
      26           4         [heading]                      measured heading
      30           4         [linear velocity]              measured linear velocity
      34           4         [angular velocity]             measured angular velocity
          <------ Ultrasonic ------>
      38          16         [front ultrasonic distance]    ultrasonic distance is an array of 
                                                            distances from the ultrasonic sensors in 
                                                            centimeters, range 2 to 500
      54          16         [rear ultrasonic distance]     ultrasonic distance is an array of 
                                                            distances from the ultrasonic sensors in 
                                                            centimeters, range 2 to 500
           <------ Infrared ------>
      70           8         [infrared distance]            infrared distance is an array of distances 
                                                            from the infrared sensors in centimeters, 
                                                            range 10 to 80
      78           8         [infrared distance]            infrared distance is an array of distances 
                                                            from the infrared sensors in centimeters, 
                                                            range 10 to 80
      86           4         [heartbeat]                    used for testing the i2c communication
 */

/* Define the portion of the I2C Slave that Read/Write */
typedef struct
{
    uint16 device_control;
    uint16 debug_control;
    uint16 calibration_control;
    float  linear_cmd_velocity;
    float  angular_cmd_velocity;
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
    uint16     device_status;
    uint16     calibration_status;
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


/* Define the I2C Slave buffer (as seen by the slave on this Psoc) */
static I2C_DATASTRUCT i2c_buf;
/* Define a persistant device status that can be set and cleared */
static uint16 device_status;
/* Define a persistant calibration status that can be set and cleared */
static uint16 calibration_status;

static uint32 last_cmd_velocity_time = 0;
static uint32 cmd_velocity_timeout = 0;

/*----------------------------------------------------------------------------------------------------------------------

  Static Functions

 ---------------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------------------------------

  Public Functions

 ---------------------------------------------------------------------------------------------------------------------*/

void I2c_Init()
{
    memset( &i2c_buf, 0, sizeof(i2c_buf));
    device_status = 0;
    calibration_status = 0;
}

void I2c_Start()
{
    /* Note: The start routine must be called before the set buffer routine.  Start applies the customizations settings
       to the component including initializing the buffer pointers, so calling start after setting the buffer effectively
       re-initializes the component buffer pointers wiping out the set buffer setting.
     */
    EZI2C_Slave_Start();
    EZI2C_Slave_SetBuffer1(sizeof(i2c_buf), sizeof(i2c_buf.read_write), (volatile uint8 *) &i2c_buf);
    
    /* Read the calibration status from EEPROM and mirror in calibration_status */
    i2c_buf.read_write.calibration_control = p_cal_eeprom->status;
    //i2c_buf.read_write.calibration_control = 0x0004;
}

uint16 I2c_ReadDeviceControl()
{
    uint16 value;

    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    value = i2c_buf.read_write.device_control;
    i2c_buf.read_write.device_control = 0;
    EZI2C_Slave_EnableInt();
    
    return value;
}

uint16 I2c_ReadDebugControl()
{
    uint16 value;
    
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    value = i2c_buf.read_write.debug_control;
    i2c_buf.read_write.debug_control = 0;
    EZI2C_Slave_EnableInt();
    
    return value;
}

uint16 I2c_ReadCalibrationControl()
{
    uint16 value;
    
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    value = i2c_buf.read_write.calibration_control;
    i2c_buf.read_write.calibration_control = 0;
    EZI2C_Slave_EnableInt();
    
    return value;
}

void I2c_ReadCmdVelocity(float *linear, float *angular)
{
    /* GetActivity() returns the status of the I2C activity: write, read, busy, or error
       Wrt to I2C writes, only the first 12 bytes can be written to.  Of those 12 bytes, 2 are for the control register,
       2 are for the calibration register, and 8 are for the commanded velocity (linear and angular).  The commanded 
       velocity will always be written to more often than the control and calibration registers, so checking for write
       status is reasonably good way to know if we have received any recent velocity commands.
    
       When a write has occurred on the I2C bus, we assume it was a velocity command, and reset the command velocity 
       timeout; otherwise, we accumulate time which will be checked against the maximum command velocity timeout.
    
       As long as the timeout is less then the maximum timeout, we will process the command values received via I2C.  If
       the timeout exceeded the maximum timeout, we set the commanded velocity to 0.
       
     */
    uint8 i2c_write_occurred = EZI2C_Slave_GetActivity();
    
    if (i2c_write_occurred & EZI2C_Slave_STATUS_WRITE1)
    {
        cmd_velocity_timeout = 0;
    }
    else
    {
        cmd_velocity_timeout += millis() - last_cmd_velocity_time;
        last_cmd_velocity_time = millis();
    }

    *linear = 0;
    *angular = 0;
    
    if (cmd_velocity_timeout < MAX_CMD_VELOCITY_TIMEOUT)
    {
        I2C_WAIT_FOR_ACCESS();
        EZI2C_Slave_DisableInt();
        //i2c_buf.read_write.linear_cmd_velocity = 0.15;
        //i2c_buf.read_write.angular_cmd_velocity = 0.75;
        *linear = i2c_buf.read_write.linear_cmd_velocity;
        *angular = i2c_buf.read_write.angular_cmd_velocity;
        EZI2C_Slave_EnableInt();
    }
    
    *linear = max(MIN_LINEAR_VELOCITY, min(*linear, MAX_LINEAR_VELOCITY));
    *angular = max(MIN_ANGULAR_VELOCITY, min(*angular, MAX_ANGULAR_VELOCITY));
}

void I2c_SetDeviceStatusBit(uint16 bit)
{
    device_status |= bit;
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.device_status = device_status;
    EZI2C_Slave_EnableInt();
}

void I2c_ClearDeviceStatusBit(uint16 bit)
{
    device_status &= ~bit;
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.device_status = device_status;
    EZI2C_Slave_EnableInt();
}

void I2c_SetCalibrationStatusBit(uint16 bit)
{
    calibration_status |= bit;
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.device_status = calibration_status;
    EZI2C_Slave_EnableInt();
}

void I2c_ClearCalibrationStatusBit(uint16 bit)
{
    calibration_status &= ~bit;
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.calibration_status = calibration_status;
    EZI2C_Slave_EnableInt();
}

void I2c_WriteOdom(float x_dist, float y_dist, float heading, float linear_speed, float angular_speed)
{
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.x_dist = x_dist;
    EZI2C_Slave_EnableInt();
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.y_dist = y_dist;
    EZI2C_Slave_EnableInt();
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.heading = heading;
    EZI2C_Slave_EnableInt();
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.linear_velocity = linear_speed;
    EZI2C_Slave_EnableInt();
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.angular_velocity = angular_speed;
    EZI2C_Slave_EnableInt();
}

void I2c_WriteFrontUltrasonicDistance(uint8 offset, uint16 distance)
{
    if (offset >= FIRST_FRONT_ULTRASONIC_SENSOR && offset <= LAST_FRONT_ULTRASONIC_SENSOR)
    {
        I2C_WAIT_FOR_ACCESS();
        EZI2C_Slave_DisableInt();
        i2c_buf.read_only.us.front[offset] = distance;
        EZI2C_Slave_EnableInt();
    }
}

void I2c_WriteRearUltrasonicDistance(uint8 offset, uint16 distance)
{
    if (offset >= FIRST_REAR_ULTRASONIC_SENSOR && offset <= LAST_REAR_ULTRASONIC_SENSOR)
    {
        I2C_WAIT_FOR_ACCESS();
        EZI2C_Slave_DisableInt();
        i2c_buf.read_only.us.rear[offset] = distance;
        EZI2C_Slave_EnableInt();
    }            
}

void I2c_WriteFrontInfraredDistance(uint8 offset, uint8 distance)
{
    if (offset >= FIRST_REAR_INFRARED_SENSOR && offset <= LAST_REAR_INFRARED_SENSOR)
    {
        I2C_WAIT_FOR_ACCESS();
        EZI2C_Slave_DisableInt();
        i2c_buf.read_only.ir.front[offset] = distance;
        EZI2C_Slave_EnableInt();
    }
}

void I2c_WriteRearInfraredDistance(uint8 offset, uint8 distance)
{
    if (offset >= FIRST_REAR_INFRARED_SENSOR && offset <= LAST_REAR_INFRARED_SENSOR)
    {
        I2C_WAIT_FOR_ACCESS();
        EZI2C_Slave_DisableInt();
        i2c_buf.read_only.ir.rear[offset] = distance;
        EZI2C_Slave_EnableInt();
    }            
}

void I2c_UpdateHeartbeat(uint32 counter)
{
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.heartbeat = counter;
    EZI2C_Slave_EnableInt();
}

/* [] END OF FILE */
