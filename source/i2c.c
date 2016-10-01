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
#define ENCODER_DEBUG_BIT   (0x0001)
#define PID_DEBUG_BIT       (0x0002)
#define MOTOR_DEBUG_BIT     (0x0004)
#define ODOM_DEBUG_BIT      (0x0008)
#define SAMPLE_DEBUG_BIT    (0x0010)

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    

/* Macro to wait for any outstanding master writes to the I2C buffer.  This ensures data integrity across the interface
 */
#define I2C_WAIT_FOR_ACCESS()   do  \
                                {   \
                                } while (0 !=  EZI2C_Slave_GetActivity())
                                    

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
           <------ Odometry ------>
      16           4         [left speed]                   measured left speed
      20           4         [right speed]                  measured right speed
      24           4         [left delta distance]          measured delta left distance 
      28           4         [right delta distance]         measured delta right distance
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
    float dist[NUM_ULTRASONIC_SENSORS];
} __attribute__ ((packed)) ULTRASONIC;

/* Define the structure for infrared sensors */
typedef struct
{
    float dist[NUM_INFRARED_SENSORS];
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

static uint16 i2c_debug;

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
    memset( &i2c_buf, 0, sizeof(i2c_buf));
    device_status = 0;
    calibration_status = 0;
    i2c_debug = 0;
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
    EZI2C_Slave_SetBuffer1(sizeof(i2c_buf), sizeof(i2c_buf.read_write), (volatile uint8 *) &i2c_buf);
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

    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    value = i2c_buf.read_write.device_control;
    i2c_buf.read_write.device_control = 0;
    EZI2C_Slave_EnableInt();
    
    return value;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_ReadDebugControl
 * Description: Accessor function used to read the debug control status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_ReadDebugControl()
{
    uint16 value;
    
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    value = i2c_buf.read_write.debug_control;
    EZI2C_Slave_EnableInt();
    
    /* Internally, there is control over debug for each encoder, pid and motor; however, that is not exposed via i2c.
       Via the i2c interface, turning on debug for encoder, pid, and motor, turns on debug for both wheels.
     */

#ifdef COMMS_DEBUG_ENABLED
    // When debug is enabled, the bitmap can be used to turn on/off specific debug, e.g., encoder, pid, odom, etc.

    i2c_debug &= ~value;
    i2c_debug |= value;
    
    debug_control_enabled = 0;
    
    if (i2c_debug & ENCODER_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_LEFT_ENCODER_ENABLE_BIT | DEBUG_RIGHT_ENCODER_ENABLE_BIT;
    }
    
    if (i2c_debug & PID_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_LEFT_PID_ENABLE_BIT | DEBUG_RIGHT_PID_ENABLE_BIT;
    }
    
    if (i2c_debug & MOTOR_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_LEFT_MOTOR_ENABLE_BIT | DEBUG_RIGHT_MOTOR_ENABLE_BIT;
    }

    if (i2c_debug & ODOM_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_ODOM_ENABLE_BIT;
    }
    
    if (i2c_debug & SAMPLE_DEBUG_BIT)
    {
        debug_control_enabled |= DEBUG_SAMPLE_ENABLE_BIT;
    }
    
#endif
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
        char lcv_str[10];
        char rcv_str[10];
        char out_buf[64];
        ftoa(i2c_buf.read_write.left_cmd_velocity, lcv_str, 3);
        ftoa(i2c_buf.read_write.right_cmd_velocity, rcv_str, 3);
        sprintf(out_buf, "%s - %s\r\n", lcv_str, rcv_str);
        Ser_PutString(out_buf);
    }
    else
    {
        cmd_velocity_timeout += millis() - last_cmd_velocity_time;
        last_cmd_velocity_time = millis();
    }

    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    *left = i2c_buf.read_write.left_cmd_velocity;
    *right = i2c_buf.read_write.right_cmd_velocity;
    EZI2C_Slave_EnableInt();
    
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
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.device_status = device_status;
    EZI2C_Slave_EnableInt();
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
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.device_status = device_status;
    EZI2C_Slave_EnableInt();
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
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.calibration_status = calibration_status;
    EZI2C_Slave_EnableInt();
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
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.calibration_status = calibration_status;
    EZI2C_Slave_EnableInt();
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
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.calibration_status = calibration_status;
    EZI2C_Slave_EnableInt();    
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
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.left_speed = left_speed;
    EZI2C_Slave_EnableInt();
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.right_speed = right_speed;
    EZI2C_Slave_EnableInt();
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.left_delta_dist = left_delta_dist;
    EZI2C_Slave_EnableInt();
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.right_delta_dist = right_delta_dist;
    EZI2C_Slave_EnableInt();
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.odom.heading = heading;
    EZI2C_Slave_EnableInt();
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_WriteInfraredDistances
 * Description: Accessor function used to write the current infrared distance values to I2C.
 * Parameters: offset - an offset into the ir.dist array.  The array is partitioned into front and
 *             read distance values such that the front values are written to the first 8 entries
 *             and the rear values are written to the last 8 entries.
 *             distances - array of distance values
 *             num_entries - the number of entries in the array.  Typically will be a constant
 *             derived from the configuration of the number of sensors, e.g., 8 per call.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_WriteInfraredDistances(uint8 offset, float* distances, uint8 num_entries)
{
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    memcpy(&i2c_buf.read_only.ir.dist[offset], distances, num_entries);
    EZI2C_Slave_EnableInt();
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_WriteUltrasonicDistances
 * Description: Accessor function used to write the current infrared distance values to I2C.
 * Parameters: offset - an offset into the us.dist array.  The array is partitioned into front and
 *             read distance values such that the front values are written to the first 8 entries
 *             and the rear values are written to the last 8 entries.
 *             distances - array of distance values
 *             num_entries - the number of entries in the array.  Typically will be a constant
 *             derived from the configuration of the number of sensors, e.g., 8 per call.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_WriteUltrasonicDistances(uint8 offset, float* distances, uint8 num_entries)
{
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    memcpy(&i2c_buf.read_only.us.dist[offset], distances, num_entries);
    EZI2C_Slave_EnableInt();
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2c_UpdateHeartbeat
 * Description: Accessor function used to write the current heartbeat value to I2C.
 * Parameters: counter - the current heartbeat counter value.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2c_UpdateHeartbeat(uint32 counter)
{
    I2C_WAIT_FOR_ACCESS();
    EZI2C_Slave_DisableInt();
    i2c_buf.read_only.heartbeat = counter;
    EZI2C_Slave_EnableInt();
}

/* [] END OF FILE */
