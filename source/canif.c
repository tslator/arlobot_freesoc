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
   Description: This module provides the implementation for communicating over CAN.
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "can.h"
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

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef union _cmd_vel_tag
{
    UINT8 bytes[8];
    struct
    {
        FLOAT linear;
        FLOAT angular;
    };
} __attribute__ ((packed)) CMD_VELOCITY_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static UINT32 last_cmd_velocity_time;
static UINT32 cmd_velocity_timeout;

static UINT16 device_control;
static UINT16 debug_control;

static UINT8 write_occurred;
static CMD_VELOCITY_TYPE cmd_velocity;

static UINT16 device_status;
static UINT16 calibration_status;

/*--------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static void EnableInterrupt()
{
    
}

static void DisableInterrupt()
{

}

/* Each CAN receive message mailbox has an associated callback.  When a message arrives at the mailbox, 
   the callback is called and the module variable is updated.
   
   Note: Callbacks are called under interrupt, so accessors to the module variables must disable/
   enable interrupts to preserve integrity.
 */
void CAN_Rx_RX_Control_Callback()
{
    /* Read the control message and parse */
    UINT8 hi;
    UINT8 lo;
    
    hi = CAN_RX_DATA_BYTE1(CAN_RX_MAILBOX_Control);
    lo = CAN_RX_DATA_BYTE2(CAN_RX_MAILBOX_Control);
    device_control = hi << 8 | lo;

    hi = CAN_RX_DATA_BYTE3(CAN_RX_MAILBOX_Control);
    lo = CAN_RX_DATA_BYTE4(CAN_RX_MAILBOX_Control);
    debug_control = hi << 8 | lo;
}

void CAN_Rx_RX_Command_Callback()
{
    /* Read the command message and parse */
    
    write_occurred = 1;
    
    cmd_velocity.bytes[0] = CAN_RX_DATA_BYTE1(CAN_RX_MAILBOX_Command);
    cmd_velocity.bytes[1] = CAN_RX_DATA_BYTE2(CAN_RX_MAILBOX_Command);
    cmd_velocity.bytes[2] = CAN_RX_DATA_BYTE3(CAN_RX_MAILBOX_Command);
    cmd_velocity.bytes[3] = CAN_RX_DATA_BYTE4(CAN_RX_MAILBOX_Command);
    cmd_velocity.bytes[4] = CAN_RX_DATA_BYTE5(CAN_RX_MAILBOX_Command);
    cmd_velocity.bytes[5] = CAN_RX_DATA_BYTE6(CAN_RX_MAILBOX_Command);
    cmd_velocity.bytes[6] = CAN_RX_DATA_BYTE7(CAN_RX_MAILBOX_Command);
    cmd_velocity.bytes[7] = CAN_RX_DATA_BYTE8(CAN_RX_MAILBOX_Command);
}
 
/*---------------------------------------------------------------------------------------------------
 * Name: CAN_Init
 * Description: Initializes the CAN module variables
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void CANIF_Init()
{
    
}

/*---------------------------------------------------------------------------------------------------
 * Name: CAN_Start
 * Description: Starts the CAN component and .
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void CANIF_Start()
{
    CAN_Start();
}


UINT16 CANIF_ReadDeviceControl()
{
    UINT16 control;
    
    DisableInterrupt();
    control = device_control;    
    EnableInterrupt();
    
    return control;
}

UINT16 CANIF_ReadDebugControl()
{
    UINT16 control;
    
    DisableInterrupt();
    control = debug_control;    
    EnableInterrupt();
    
    return control;
}

void CANIF_ReadCmdVelocity(FLOAT* const linear, FLOAT* const angular, UINT32* const timeout)
{
    DisableInterrupt();
    if (write_occurred)
    {
        write_occurred = 0;
        cmd_velocity_timeout = 0;
    }
    else
    {
        cmd_velocity_timeout += (millis() - last_cmd_velocity_time);
        last_cmd_velocity_time = millis();
    }    
    
    *linear = cmd_velocity.linear;
    *angular = cmd_velocity.angular;
    
    *timeout = cmd_velocity_timeout;
    EnableInterrupt();
}

static void WriteStatusMsg()
{
    /* Write a status message to the CAN bus */
    CAN_TX_DATA_BYTE1(CAN_TX_MAILBOX_Status) = (device_status & 0xF0) >> 8;
    CAN_TX_DATA_BYTE2(CAN_TX_MAILBOX_Status) = device_status & 0x0F;
    CAN_TX_DATA_BYTE3(CAN_TX_MAILBOX_Status) = (calibration_status & 0xF0) >> 8;
    CAN_TX_DATA_BYTE4(CAN_TX_MAILBOX_Status) = calibration_status & 0x0F;
}

static void WriteSpeedMsg(FLOAT linear, FLOAT angular)
{
    UINT8 *p_bytes;
    
    p_bytes = (UINT8 *) &linear;
    CAN_TX_DATA_BYTE1(CAN_TX_MAILBOX_LeftRightSpeed) = p_bytes[0];
    CAN_TX_DATA_BYTE2(CAN_TX_MAILBOX_LeftRightSpeed) = p_bytes[1];
    CAN_TX_DATA_BYTE3(CAN_TX_MAILBOX_LeftRightSpeed) = p_bytes[2];
    CAN_TX_DATA_BYTE4(CAN_TX_MAILBOX_LeftRightSpeed) = p_bytes[3];
    
    p_bytes = (UINT8 *) &angular;
    CAN_TX_DATA_BYTE1(CAN_TX_MAILBOX_LeftRightSpeed) = p_bytes[0];
    CAN_TX_DATA_BYTE2(CAN_TX_MAILBOX_LeftRightSpeed) = p_bytes[1];
    CAN_TX_DATA_BYTE3(CAN_TX_MAILBOX_LeftRightSpeed) = p_bytes[2];
    CAN_TX_DATA_BYTE4(CAN_TX_MAILBOX_LeftRightSpeed) = p_bytes[3];
}

static void WritePositionMsg(FLOAT left, FLOAT right)
{
    UINT8 *p_bytes;
    
    p_bytes = (UINT8 *) &left;
    CAN_TX_DATA_BYTE1(CAN_TX_MAILBOX_LeftRightDistance) = p_bytes[0];
    CAN_TX_DATA_BYTE2(CAN_TX_MAILBOX_LeftRightDistance) = p_bytes[1];
    CAN_TX_DATA_BYTE3(CAN_TX_MAILBOX_LeftRightDistance) = p_bytes[2];
    CAN_TX_DATA_BYTE4(CAN_TX_MAILBOX_LeftRightDistance) = p_bytes[3];
    
    p_bytes = (UINT8 *) &right;
    CAN_TX_DATA_BYTE1(CAN_TX_MAILBOX_LeftRightDistance) = p_bytes[0];
    CAN_TX_DATA_BYTE2(CAN_TX_MAILBOX_LeftRightDistance) = p_bytes[1];
    CAN_TX_DATA_BYTE3(CAN_TX_MAILBOX_LeftRightDistance) = p_bytes[2];
    CAN_TX_DATA_BYTE4(CAN_TX_MAILBOX_LeftRightDistance) = p_bytes[3];
}

static void WriteHeadingMsg(FLOAT heading)
{
    UINT8 *p_bytes;
    
    p_bytes = (UINT8 *) &heading;
    CAN_TX_DATA_BYTE1(CAN_TX_MAILBOX_Heading) = p_bytes[0];
    CAN_TX_DATA_BYTE2(CAN_TX_MAILBOX_Heading) = p_bytes[1];
    CAN_TX_DATA_BYTE3(CAN_TX_MAILBOX_Heading) = p_bytes[2];
    CAN_TX_DATA_BYTE4(CAN_TX_MAILBOX_Heading) = p_bytes[3];
}

static void WriteHeartbeatMsg(UINT32 heartbeat)
{
    UINT8 *p_bytes;
    
    p_bytes = (UINT8 *) &heartbeat;
    CAN_TX_DATA_BYTE1(CAN_TX_MAILBOX_Heartbeat) = p_bytes[0];
    CAN_TX_DATA_BYTE2(CAN_TX_MAILBOX_Heartbeat) = p_bytes[1];
    CAN_TX_DATA_BYTE3(CAN_TX_MAILBOX_Heartbeat) = p_bytes[2];
    CAN_TX_DATA_BYTE4(CAN_TX_MAILBOX_Heartbeat) = p_bytes[3];
}

void CANIF_SetDeviceStatusBit(UINT16 bit)
{
    /* Update the device status bit */
    device_status |= bit;
    WriteStatusMsg();
}

void CANIF_ClearDeviceStatusBit(UINT16 bit)
{
    /* Update the device status bit */
    device_status &= ~bit;
    WriteStatusMsg();
}

void CANIF_SetCalibrationStatus(UINT16 status)
{
    /* Update calibration status */
    calibration_status = status;
    WriteStatusMsg();
}

void CANIF_SetCalibrationStatusBit(UINT16 bit)
{
    /* Update calibration status bit */
    calibration_status |= bit;
    WriteStatusMsg();
}

void CANIF_ClearCalibrationStatusBit(UINT16 bit)
{
    /* Update calibration status bit */
    calibration_status &= ~bit;
    WriteStatusMsg();
}

void CANIF_WriteSpeed(FLOAT linear, FLOAT angular)
{
    WriteSpeedMsg(linear, angular);
}

void CANIF_WritePosition(FLOAT x_position, FLOAT y_position)
{
    WritePositionMsg(x_position, y_position);
}

void CANIF_WriteHeading(FLOAT heading)
{
    WriteHeadingMsg(heading);
}

void CANIF_UpdateHeartbeat(UINT32 heartbeat)
{
    WriteHeartbeatMsg(heartbeat);
}

/* [] END OF FILE */
