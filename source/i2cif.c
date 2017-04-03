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
#include "i2cif.h"
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
#define I2CIF_WAIT_FOR_ACCESS()   do  \
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
      36           4         [heartbeat]                    used for testing the i2c communication
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

/* Define the I2C Slave that Read Only */
typedef struct
{
    uint16     device_status;
    uint16     calibration_status;
    ODOMETRY   odom;
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

static uint32 last_cmd_velocity_time;
static uint32 cmd_velocity_timeout;

static uint16 i2c_debug;
static uint16 calibration_status;
static uint16 device_status;


/*--------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_Init
 * Description: Initializes the I2C memory buffer and module variables
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_Init()
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
 * Name: I2CIF_Start
 * Description: Starts the I2C slave component and sets the I2C read/write buffer.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_Start()
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
 * Name: I2CIF_ReadDeviceControl
 * Description: Accessor function used to read the device control status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
uint16 I2CIF_ReadDeviceControl()
{
    uint16 value;

    value = i2c_buf.read_write.device_control;
    i2c_buf.read_write.device_control = 0;
    
    return value;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_ReadDebugControl
 * Description: Accessor function used to read the debug control status.
 * Parameters: None
 * Return: Control value
 * 
 *-------------------------------------------------------------------------------------------------*/
uint16 I2CIF_ReadDebugControl()
{
    return i2c_buf.read_write.debug_control;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_ReadCmdVelocity
 * Description: Accessor function used to read the commanded left/right wheel velocity.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_ReadCmdVelocity(float *left, float *right, uint32 *timeout)
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
        cmd_velocity_timeout += (millis() - last_cmd_velocity_time);
        last_cmd_velocity_time = millis();
    }

    *left = i2c_buf.read_write.left_cmd_velocity;
    *right = i2c_buf.read_write.right_cmd_velocity;
    
    *timeout = cmd_velocity_timeout;    
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_SetDeviceStatusBit
 * Description: Accessor function used to set a bit in the device status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_SetDeviceStatusBit(uint16 bit)
{
    device_status |= bit;
    i2c_buf.read_only.device_status = device_status;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_ClearDeviceStatusBit
 * Description: Accessor function used to clear a bit in the device status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_ClearDeviceStatusBit(uint16 bit)
{
    device_status &= ~bit;
    i2c_buf.read_only.device_status = device_status;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_SetCalibrationStatus
 * Description: Accessor function used to initialize the calibration status as read from non-volatile
 *              storage.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_SetCalibrationStatus(uint16 status)
{
    calibration_status = status;
    i2c_buf.read_only.calibration_status = calibration_status;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_SetCalibrationStatusBit
 * Description: Accessor function used to set a bit in the calibration status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_SetCalibrationStatusBit(uint16 bit)
{
    calibration_status |= bit;
    i2c_buf.read_only.calibration_status = calibration_status;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_ClearCalibrationStatusBit
 * Description: Accessor function used to clear a bit in the calibration status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_ClearCalibrationStatusBit(uint16 bit)
{
    calibration_status &= ~bit;
    i2c_buf.read_only.calibration_status = calibration_status;
}

void I2CIF_WriteSpeed(float left_speed, float right_speed)
{
    i2c_buf.read_only.odom.left_speed = left_speed;
    i2c_buf.read_only.odom.right_speed = right_speed;
}

void I2CIF_WriteDistance(float left_dist, float right_dist)
{
    i2c_buf.read_only.odom.left_delta_dist = left_dist;
    i2c_buf.read_only.odom.right_delta_dist = right_dist;
}

void I2CIF_WriteHeading(float heading)
{
    i2c_buf.read_only.odom.heading = heading;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_UpdateHeartbeat
 * Description: Accessor function used to write the current heartbeat value to I2C.
 * Parameters: counter - the current heartbeat counter value.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_UpdateHeartbeat(uint32 heartbeat)
{
    i2c_buf.read_only.heartbeat = heartbeat;
}

#ifdef TEST_I2C
void I2CIF_Test()
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
