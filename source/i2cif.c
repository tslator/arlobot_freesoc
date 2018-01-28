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
   Description: This module provides the implementation for communicating over I2C.
 *-------------------------------------------------------------------------------------------------*/    

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
DEFINE_THIS_FILE;

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
                                                                - level Bits0:7
                                                                - enable/disable Bits8:15
                                                                - Bit  8: Enable/Disable Encoder Debug
                                                                - Bit  9: Enable/Disable PID debug
                                                                - Bit 10: Enable/Disable Motor debug
                                                                - Bit 11: Enable/Disable Odometry debug
                                                                - Bit 12: Enable/Disable Sample debug                                                            
        <---- Commanded Velocity ---->
      04           4         [linear velocity]              commanded linear velocity in meter/second
      08           4         [angular velocity]             commanded angular velocity in radian/second
    ------------------------------ Read/Write Boundary --------------------------------------------
      12           2         [device status]                contains bits that represent the status of the Psoc device
                                                               - Bit 0: HB25 Motor Controller Initialized
      14           2         [calibration status]           contains bits that represent the calibration state
                                                               - Bit 0: Count/Sec to PWM
                                                               - Bit 1: PID
                                                               - Bit 2: Linear
                                                               - Bit 3: Angular
           <------ Odometry ------>
      16           4         [linear velocity]              measured linear velocity
      20           4         [angular velocity]             measured angular velocity
      24           4         [x position]                   measured x position 
      28           4         [y position]                   measured y position
      32           4         [heading]                      measured heading
      36           4         [heartbeat]                    used for testing the i2c communication
 */

/* Define the portion of the I2C Slave that Read/Write */
typedef struct
{
    UINT16 device_control;
    UINT16 debug_control;
    FLOAT  linear_cmd_velocity;
    FLOAT  angular_cmd_velocity;
} __attribute__ ((packed)) READWRITE_TYPE;

/* Define the odometry structure for communicating the position, heading and velocity of the wheel 
 */
typedef struct
{
    FLOAT linear_velocity;
    FLOAT angular_velocity;
    FLOAT x_position;
    FLOAT y_position;
    FLOAT heading;
} __attribute__ ((packed)) ODOMETRY;

/* Define the I2C Slave that Read Only */
typedef struct
{
    UINT16     device_status;
    UINT16     calibration_status;
    ODOMETRY   odom;
    UINT32     heartbeat;
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
        UINT8 bytes[8];
        UINT16 words[4];
        UINT32 longs[2];
        FLOAT FLOATs[2];
    };
} __attribute__ ((packed)) I2C_TEST_READ_WRITE;

typedef struct _read_only
{
    UINT32 rd1_busy;
    UINT32 busy;
    UINT32 err;
    UINT32 read1;
    UINT32 wr1_busy;
    UINT32 write1;
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

static UINT32 last_cmd_velocity_time;
static UINT32 cmd_velocity_timeout;

static UINT16 i2c_debug;
static UINT16 calibration_status;
static UINT16 device_status;


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
    EZI2C_Slave_SetBuffer1(sizeof(i2c_test), sizeof(i2c_test.read_write), (volatile UINT8 *) &i2c_test);
#else
    EZI2C_Slave_SetBuffer1(sizeof(i2c_buf), sizeof(i2c_buf.read_write), (volatile UINT8 *) &i2c_buf);
#endif
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_ReadDeviceControl
 * Description: Accessor function used to read the device control status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT16 I2CIF_ReadDeviceControl()
{
    UINT16 value;

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
UINT16 I2CIF_ReadDebugControl()
{
    return i2c_buf.read_write.debug_control;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_ReadCmdVelocity
 * Description: Accessor function used to read the commanded linear/angular velocity.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_ReadCmdVelocity(FLOAT* const linear, FLOAT* const angular, UINT32* const timeout)
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
    UINT8 i2c_write_occurred = EZI2C_Slave_GetActivity();
    
    if (i2c_write_occurred & EZI2C_Slave_STATUS_WRITE1)
    {
        cmd_velocity_timeout = 0;
    }
    else
    {
        cmd_velocity_timeout += (millis() - last_cmd_velocity_time);
        last_cmd_velocity_time = millis();
    }

    *linear = i2c_buf.read_write.linear_cmd_velocity;
    *angular = i2c_buf.read_write.angular_cmd_velocity;

    *timeout = cmd_velocity_timeout;
}

/*---------------------------------------------------------------------------------------------------
 * Name: I2CIF_SetDeviceStatusBit
 * Description: Accessor function used to set a bit in the device status.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void I2CIF_SetDeviceStatusBit(UINT16 bit)
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
void I2CIF_ClearDeviceStatusBit(UINT16 bit)
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
void I2CIF_SetCalibrationStatus(UINT16 status)
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
void I2CIF_SetCalibrationStatusBit(UINT16 bit)
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
void I2CIF_ClearCalibrationStatusBit(UINT16 bit)
{
    calibration_status &= ~bit;
    i2c_buf.read_only.calibration_status = calibration_status;
}

void I2CIF_WriteSpeed(FLOAT linear, FLOAT angular)
{
    i2c_buf.read_only.odom.linear_velocity = linear;
    i2c_buf.read_only.odom.angular_velocity = angular;
}

void I2CIF_WritePosition(FLOAT x_position, FLOAT y_position)
{
    i2c_buf.read_only.odom.x_position = x_position;
    i2c_buf.read_only.odom.y_position = y_position;
}

void I2CIF_WriteHeading(FLOAT heading)
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
void I2CIF_UpdateHeartbeat(UINT32 heartbeat)
{
    i2c_buf.read_only.heartbeat = heartbeat;
}

#ifdef TEST_I2C
void I2CIF_Test()
{
    UINT32 activity = EZI2C_Slave_GetActivity();
    
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
