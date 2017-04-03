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

/*---------------------------------------------------------------------------------------------------
 * Description
 *-------------------------------------------------------------------------------------------------*/
// Add a description of the module

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "i2c.h"
#include "time.h"
#include "config.h"
#include "utils.h"
#include "debug.h"
#include "cal.h"


/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    

/* Macro to wait for any outstanding master writes to the I2C buffer.  This ensures data integrity across the interface
 */
#define I2C_WAIT_FOR_ACCESS()   do  \
                                {   \
                                } while (EZI2C_STATUS_BUSY ==  EZI2C_Slave_GetActivity())
                                    

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
/*
    I2C Communication Data Layout

    offset     num bytes     name                           description
      00           2         [device control]               the control register supports 
                                                                - Bit 0: disable the HB25 motors
                                                                - Bit 1: clear odometry (which includes the following:
                                                                    left/right speed
                                                                    left/right delta distance
                                                                    heading
                                                                    encoder counts
                                                                - Bit 2: clear calibration
      02           2         [debug control]                the debug register supports
                                                                - Bit 0: Enable/Disable Encoder Debug
                                                                - Bit 1: Enable/Disable PID debug
                                                                - Bit 2: Enable/Disable Motor debug
                                                                - Bit 3: Enable/Disable Odometry debug
                                                                - Bit 4: Enable/Disable Sample debug
        <---- Commanded Velocity ---->
      04           4         [left wheel velocity]          commanded left wheel velocity in meter/second
      08           4         [right wheel velocity]         commanded right wheel velocity in meter/second
    ------------------------------ Read/Write Boundary --------------------------------------------
      12           2         [device status]                contains bits that represent the status of the Psoc device
                                                               - Bit 0: HB25 Motor Controller Initialized
      14           2         [calibration status]           contains bits that represent the calibration state
                                                               - Bit 0: Count/Sec to PWM
                                                               - Bit 1: PID
                                                               - Bit 2: Linear
                                                               - Bit 3: Angular
           <------ Odometry ------>
      16           4         [left speed]                   measured left speed
      20           4         [right speed]                  measured right speed
      24           4         [left distance]                measured left distance 
      28           4         [right distance]               measured right distance
      32           4         [heading]                      measured heading
          <------ Ultrasonic ------>
      36          64         [ultrasonic distance]          ultrasonic distance is an array of 
                                                            distances from the ultrasonic sensors in 
                                                            meters, range 0.02 to 4
                                                            
                                                            Front sensors - 32 bytes
                                                            Rear sensors - 32 bytes
           <------ Infrared ------>
     100          64         [infrared distance]            infrared distance is an array of distances 
                                                            from the infrared sensors in meters, 
                                                            range 0.1 to 0.8
                                                            
                                                            Front sensors - 32 bytes
                                                            Rear sensors - 32 bytes

     164           4         [heartbeat]                    used for testing the i2c communication
 */

/* Define the portion of the I2C Slave that Read/Write */
typedef struct
{
    uint16 device_control;
    uint16 debug_control;
    float  left_cmd_velocity;
    float  right_cmd_velocity;
} __attribute__ ((packed)) READWRITE_TYPE;

/* Define the odometry structure for communicating the position, heading and velocity of the wheel 
   Note: Each Psoc board controls a single wheel, but calculates and reports odometry for the robot
 */
typedef struct
{
    float left_speed;
    float right_speed;
    float left_delta_dist;
    float right_delta_dist;
    float heading;
} __attribute__ ((packed)) ODOMETRY;

/* Define the structure for ultrasonic sensors */
typedef struct
{
    float front[NUM_FRONT_ULTRASONIC_SENSORS];
    float rear[NUM_REAR_ULTRASONIC_SENSORS];
} __attribute__ ((packed)) ULTRASONIC;

/* Define the structure for infrared sensors */
typedef struct
{
    float front[NUM_FRONT_INFRARED_SENSORS];
    float rear[NUM_REAR_INFRARED_SENSORS];
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

#ifdef TEST_I2C
typedef struct _read_write
{
    union 
    {
        uint8 bytes[8];
        uint16 words[4];
        uint32 longs[2];
        float floats[2];
    };
} __attribute__ ((packed)) I2C_TEST_READ_WRITE;

typedef struct _read_only
{
    uint32 rd1_busy;
    uint32 busy;
    uint32 err;
    uint32 read1;
    uint32 wr1_busy;
    uint32 write1;
} __attribute__ ((packed)) I2C_TEST_READ_ONLY;

typedef struct
{
    I2C_TEST_READ_WRITE read_write;
    I2C_TEST_READ_ONLY read_only;
} __attribute__ ((packed)) I2C_TEST;

static volatile I2C_TEST i2c_test;
#endif

/* Define the I2C Slave buffer (as seen by the slave on this Psoc) */
static volatile I2C_DATASTRUCT i2c_buf;

static uint32 last_cmd_velocity_time = 0;
static uint32 cmd_velocity_timeout = 0;

static uint16 i2c_debug;
static uint16 calibration_status;
static uint16 device_status;


/*--------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_Init
 * Description: Initializes the I2C memory buffer and module variables
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_Init()
{
    memset( (void *) &i2c_buf, 0, sizeof(i2c_buf));
#ifdef TEST_I2C    
    memset( (void *) &i2c_test, 0, sizeof(i2c_test));
#endif    
    i2c_debug = 0;
    calibration_status = 0;
    device_status = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_Start
 * Description: Starts the I2C slave component and sets the I2C read/write buffer.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_Start()
{
    /* Note: The start routine must be called before the set buffer routine.  Start applies the customizations settings
       to the component including initializing the buffer pointers, so calling start after setting the buffer effectively
       re-initializes the component buffer pointers wiping out the set buffer setting.
     */
    EZI2C_Slave_Start();
#ifdef TEST_I2C    
    EZI2C_Slave_SetBuffer1(sizeof(i2c_test), sizeof(i2c_test.read_write), (volatile uint8 *) &i2c_test);
#else
    EZI2C_Slave_SetBuffer1(sizeof(i2c_buf), sizeof(i2c_buf.read_write), (volatile uint8 *) &i2c_buf);
#endif
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_ReadDeviceControl
 * Description: Accessor function used to read the device control status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
uint16 I2c_ReadDeviceControl()
{
    uint16 value;

    value = CY_GET_REG16(&i2c_buf.read_write.device_control);
    CY_SET_REG16(&i2c_buf.read_write.device_control, 0);
    
    return value;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_ReadDebugControl
 * Description: Accessor function used to read the debug control status.
 * Parameters: None
 * Return: Control value
 * 
 *-------------------------------------------------------------------------------------------------*/
uint16 I2c_ReadDebugControl()
{
    return i2c_buf.read_write.debug_control;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_ReadCmdVelocity
 * Description: Accessor function used to read the commanded left/right wheel velocity.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_ReadCmdVelocity(float *left, float *right, uint32 *timeout)
{
    /* GetActivity() returns the status of the I2C activity: write, read, busy, or error
       Wrt to I2C writes, only the first 12 bytes can be written to.  Of those 12 bytes, 2 are for the control register,
       2 are for the calibration register, and 8 are for the commanded velocity (left and right).  The commanded 
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

    /* Note: For all of the i2c accesses that are integers, I'm using the CY_GET/SET macros.  Its not clear how the
       cast to reg32 would affect/change a floating point value.  That needs to be tested.
    
       Also, while I'm thinking of it.  It may not be necessary to use these macros.  As I understand them are they are
       intended to maintain endianess over the communication channel, but both the Beaglebone Black and the Psoc5LP are
       little endian, so it doesn't make much difference unless some is going to interface with a different endianess
       processor.
    
       Another alternative would be to take the "network" approach and convert all data to network (or big endian) order.
       That's the industry standard.  Something to think about.
     */
    *left = i2c_buf.read_write.left_cmd_velocity;
    *right = i2c_buf.read_write.right_cmd_velocity;
    
    *timeout = cmd_velocity_timeout;    
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_SetDeviceStatusBit
 * Description: Accessor function used to set a bit in the device status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_SetDeviceStatusBit(uint16 bit)
{
    device_status |= bit;
    CY_SET_REG16(&i2c_buf.read_only.device_status, device_status);
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_ClearDeviceStatusBit
 * Description: Accessor function used to clear a bit in the device status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_ClearDeviceStatusBit(uint16 bit)
{
    device_status &= ~bit;
    CY_SET_REG16(&i2c_buf.read_only.device_status, device_status);
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_SetCalibrationStatusBit
 * Description: Accessor function used to set a bit in the calibration status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_SetCalibrationStatusBit(uint16 bit)
{
    calibration_status |= bit;
    CY_SET_REG16(&i2c_buf.read_only.calibration_status, calibration_status);
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_ClearCalibrationStatusBit
 * Description: Accessor function used to clear a bit in the calibration status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_ClearCalibrationStatusBit(uint16 bit)
{
    calibration_status &= ~bit;
    CY_SET_REG16(&i2c_buf.read_only.calibration_status, calibration_status);
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_SetCalibrationStatus
 * Description: Accessor function used to initialize the calibration status as read from non-volatile
 *              storage.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_SetCalibrationStatus(uint16 status)
{
    calibration_status = status;
    CY_SET_REG16(&i2c_buf.read_only.calibration_status, calibration_status);
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_WriteOdom
 * Description: Accessor function used to write odometry values to I2C
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_WriteOdom(float left_speed, float right_speed, float left_delta_dist, float right_delta_dist, float heading)
{
    i2c_buf.read_only.odom.left_speed = left_speed;
    i2c_buf.read_only.odom.right_speed = right_speed;
    i2c_buf.read_only.odom.left_delta_dist = left_delta_dist;
    i2c_buf.read_only.odom.right_delta_dist = right_delta_dist;
    i2c_buf.read_only.odom.heading = heading;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_WriteUltrasonicFrontDistances/I2c_WriteUltrasonicRearDistances
 * Description: Accessor function used to write the current infrared distance values to I2C.
 * Parameters: distances - array of distance values
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_WriteUltrasonicFrontDistances(float* distances)
{
    uint8 ii;
    
    for (ii = 0; ii < NUM_FRONT_ULTRASONIC_SENSORS; ++ii)
    {
        i2c_buf.read_only.us.front[ii] = distances[ii];
    }
}

void I2c_WriteUltrasonicRearDistances(float* distances)
{
    uint8 ii;
    
    for (ii = 0; ii < NUM_REAR_ULTRASONIC_SENSORS; ++ii)
    {
        i2c_buf.read_only.us.rear[ii] = distances[ii];
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_WriteInfraredFrontDistances/I2c_WriteInfraredRearDistances
 * Description: Accessor function used to write the current infrared distance values to I2C.
 * Parameters: distances - array of distance values
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_WriteInfraredFrontDistances(float* distances)
{
    uint8 ii;
    
    for (ii = 0; ii < NUM_FRONT_INFRARED_SENSORS; ++ii)
    {
        i2c_buf.read_only.ir.front[ii] = distances[ii];
    }
}

void I2c_WriteInfraredRearDistances(float* distances)
{
    uint8 ii;
    
    for (ii = 0; ii < NUM_REAR_INFRARED_SENSORS; ++ii)
    {
        i2c_buf.read_only.ir.rear[ii] = distances[ii];
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_UpdateHeartbeat
 * Description: Accessor function used to write the current heartbeat value to I2C.
 * Parameters: counter - the current heartbeat counter value.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_UpdateHeartbeat()
{
    i2c_buf.read_only.heartbeat++;
}

#ifdef TEST_I2C
void I2c_Test()
{
    uint32 activity = EZI2C_Slave_GetActivity();
    
    if (activity & EZI2C_Slave_STATUS_RD1BUSY)
    {
        i2c_test.read_only.rd1_busy++;
    }
    if (activity & EZI2C_Slave_STATUS_BUSY)
    {
        i2c_test.read_only.busy++;
    }
    if (activity & EZI2C_Slave_STATUS_ERR)
    {
        i2c_test.read_only.err++;
    }
    if (activity & EZI2C_Slave_STATUS_READ1)
    {
        i2c_test.read_only.read1++;
    }
    if (activity & EZI2C_Slave_STATUS_WR1BUSY)
    {
        i2c_test.read_only.wr1_busy++;
    }
    if (activity & EZI2C_Slave_STATUS_WRITE1)
    {
        i2c_test.read_only.write1++;
    }
}
#endif    

/* [] END OF FILE */
