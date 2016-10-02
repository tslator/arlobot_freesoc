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
#include <stdio.h>
#include "spi.h"
#include "time.h"
#include "i2c.h"
#include "serial.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/
#define PSOC5LP
#ifdef PSOC5LP
#define SPIM_SPI_SELECT0  0
#define SPIM_SPI_SELECT1  1
#define SPIM_SPI_SELECT2  2
#define SPIM_SPI_SELECT3  3
    
static uint8 sensor_select[NUM_SPI_NODES] = {SPIM_SPI_SELECT0, SPIM_SPI_SELECT1, SPIM_SPI_SELECT2, SPIM_SPI_SELECT3};                    
    
#define SPIM_SLAVE_SELECT(x)        Slave_Select_Write(sensor_select[x])
#define SPIM_CLEAR_RX_BUFFER()      SPIM_ClearRxBuffer()
#define SPIM_CLEAR_TX_BUFFER()      SPIM_ClearTxBuffer()
#define SPIM_PUT_ARRAY(data, size)  SPIM_PutArray(data, size)
#define SPIM_GET_RX_BUFFER_SIZE()   SPIM_GetRxBufferSize()
#define SPIM_READ_RX_DATA SPIM_ReadRxData
#else
    
#define SPIM_SPI_SELECT0  SPIM_SPI_SLAVE_SELECT0
#define SPIM_SPI_SELECT1  SPIM_SPI_SLAVE_SELECT1
#define SPIM_SPI_SELECT2  SPIM_SPI_SLAVE_SELECT2
#define SPIM_SPI_SELECT3  SPIM_SPI_SLAVE_SELECT3

static uint8 sensor_select[NUM_NODES] = {SPIM_SPI_SELECT0, SPIM_SPI_SELECT1, SPIM_SPI_SELECT2, SPIM_SPI_SELECT3};                    

#define SPIM_SLAVE_SELECT(x)        SPIM_SpiSetActiveSlaveSelect(x)
#define SPIM_CLEAR_RX_BUFFER()      SPIM_SpiUartClearRxBuffer()
#define SPIM_CLEAR_TX_BUFFER()      SPIM_SpiUartClearTxBuffer()
#define SPIM_PUT_ARRAY(data, size)  SPIM_SpiUartPutArray(data, size)
#define SPIM_GET_RX_BUFFER_SIZE()   SPIM_SpiUartGetRxBufferSize()
#define SPIM_READ_RX_DATA SPIM_SpiUartReadRxData
#endif

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define SOM (0xF1)
#define US_TYPE (0x01)
#define IR_TYPE (0x02)
#define EOM (0xF8)

typedef float DATA_TYPE;

#define NUM_SENSORS_PER_NODE (8)
#define SENSOR_DATA_SIZE (NUM_SENSORS_PER_NODE * sizeof(DATA_TYPE))
    
#define MSG_FRAME_SIZE (3)
#define DATUM_SIZE (sizeof(DATA_TYPE))
#define MSG_DATA_SIZE (NUM_SENSORS_PER_NODE * DATUM_SIZE)
#define MAX_MSG_SIZE (MSG_FRAME_SIZE + MSG_DATA_SIZE)


/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef struct msg_type_tag
{
    uint8 som;
    uint8 type;
    union {
        uint8 bytes[MSG_DATA_SIZE];
        float floats[NUM_SENSORS_PER_NODE];
    } data;
    uint8 eom;
} __attribute__ ((packed)) SPI_MSG_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
/* Dummy message the master pushes when reading the slave data */
static uint8 dummy[MAX_MSG_SIZE] = 
                       {0xff, 0xff,             // SOM, SENSOR_TYPE 
                        0xff, 0xff, 0xff, 0xff, // Sensor 1 Data
                        0xff, 0xff, 0xff, 0xff, // Sensor 2 Data 
                        0xff, 0xff, 0xff, 0xff, // Sensor 3 Data 
                        0xff, 0xff, 0xff, 0xff, // Sensor 4 Data 
                        0xff, 0xff, 0xff, 0xff, // Sensor 5 Data 
                        0xff, 0xff, 0xff, 0xff, // Sensor 6 Data 
                        0xff, 0xff, 0xff, 0xff, // Sensor 7 Data 
                        0xff, 0xff, 0xff, 0xff, // Sensor 8 Data 
                        0xff                    // EOM
                       };
                    

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
                   
/*---------------------------------------------------------------------------------------------------
 * Name: IsValidMessage
 * Description: Determines if the received message is valid or not
 * Parameters: msg - an SPI message
 * Return: 1 if the message is valid; otherwise, 0
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 IsValidMessage(SPI_MSG_TYPE *msg)
{
    return msg->som == SOM && msg->eom == EOM;
}

/*---------------------------------------------------------------------------------------------------
 * Name: RecvMessage
 * Description: Reads a message from the SPI node
 * Parameters: node - the node from which data will be read
 *             message - buffer into which the SPI message will be read
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void RecvMessage(SPI_NODE_TYPE node, SPI_MSG_TYPE *message)
{
    uint8 *p_msg;
    
    /* Get a pointer to the start of the message since the SPI interface is byte-based */
    p_msg = (uint8 *) message;
    
    SPIM_SLAVE_SELECT(node);
    
    /* Start transfer */
    SPIM_PUT_ARRAY(dummy, MAX_MSG_SIZE);

    /* Wait for the end of the transfer. The number of transmitted data
     * elements has to be equal to the number of received data elements.
     */
    uint8 size = SPIM_GET_RX_BUFFER_SIZE();
    while (MAX_MSG_SIZE != size)
    {
        size = SPIM_GET_RX_BUFFER_SIZE();
    }

    /* Clear dummy bytes from TX buffer */
    SPIM_CLEAR_TX_BUFFER();

    /* Read data from the RX buffer */
    uint8 ii = 0;
    while (0u != SPIM_GET_RX_BUFFER_SIZE() && ii < MAX_MSG_SIZE)
    {
        static uint8 value;
        value = SPIM_READ_RX_DATA();
        p_msg[ii] = value;
        ii++;
    }
    SPIM_CLEAR_RX_BUFFER();
    
//#define FAKE_SENSOR    
#ifdef FAKE_SENSOR
    message->som = SOM;
    message->eom = EOM;
    message->data.floats[0] = 1.0;
    message->data.floats[1] = 2.0;
    message->data.floats[2] = 3.0;
    message->data.floats[3] = 4.0;
    message->data.floats[4] = 5.0;
    message->data.floats[5] = 6.0;
    message->data.floats[6] = 7.0;
    message->data.floats[7] = 8.0;
    message->type = IR_TYPE;

    if (sensor == 1 || sensor == 3)
    {
        message->type = US_TYPE;
    }
#endif    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Resync
 * Description: Synchronizes the SPI master with the slave so the message aligns
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void Resync()
{
}

/*---------------------------------------------------------------------------------------------------
 * Name: HandleInvalidMessage
 * Description: Called in the case where the SPI message is invalid.
 * Parameters: node - the SPI node from which the message was read
 *             msg - the invalid SPI message
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void HandleInvalidMessage(SPI_NODE_TYPE node, SPI_MSG_TYPE *msg)
{
    /* Not sure what to do in terms of printing.  We could print the invalid message to the serial port along with the
       node indentifier.
    
       Beyond that there may be the need to resync the SPI master with the slave so the data is aligned, but I'm not 
       exactly sure how to do that or if it is really needed.  Possibly the reason the message gets out of sync is
       some other error the implementation.
     */
    Resync();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Spi_Init
 * Description: Initializes the SPI module at it components
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Spi_Init()
{
}

/*---------------------------------------------------------------------------------------------------
 * Name: Spi_Start
 * Description: Starts the components contained with the SPI module
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Spi_Start()
{
    SPIM_CLEAR_RX_BUFFER();
    SPIM_CLEAR_TX_BUFFER();
    SPIM_Start();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Spi_ReadNode
 * Description: Reads a message from the specified SPI node
 * Parameters: node - the node from which to read data
 *             data - the data read from the SPI node
 * Return: None
 * Note: The data parameter is void* in order to support a variety of SPI sensors and data.  The 
 * called knows the data type based on what device the node is mapped.
 *-------------------------------------------------------------------------------------------------*/
void Spi_ReadNode(SPI_NODE_TYPE node, void *data)
{
    SPI_MSG_TYPE msg;
    
    /* Note: RecvMessage returns the number of bytes in the data, not the number of bytes in the message */
    RecvMessage(sensor_select[node], &msg);
    if (IsValidMessage(&msg))
    {
        memcpy(data, (void *) &msg.data.bytes[0], MSG_DATA_SIZE);
    }
    else
    {
        HandleInvalidMessage(node, &msg);
    }
}

/* [] END OF FILE */
