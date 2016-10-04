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

/* The sensor module support a variety of sensors including infrared and ultrasonic distance sensors and an IMU.

   The sensor module abstracts the details of how these sensors are connected to the Psoc, e.g., SPI, IC, Digital or 
   Analog I/O.

 */


/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include "config.h"
#include "sensor.h"
#include "spi.h"
#include "i2c.h"
#include "time.h"
#include "debug.h"
#include "utils.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/
#ifdef SENSOR_UPDATE_DELTA_ENABLED
#define SENSOR_DEBUG_DELTA(delta) DEBUG_DELTA_TIME("sen", delta)
#else
#define SENSOR_DEBUG_DELTA(delta)
#endif    

#define SENSOR_SAMPLE_TIME_MS  SAMPLE_TIME_MS(SENSOR_SAMPLE_RATE)

#define NUM_SENSOR_ENTRIES (2)


/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef void (*SENSOR_READ_FUNC_TYPE)();

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
/* Note: The main loop timing is very sensitive so it is very important that module "update" functions
   limit their processing time.  There are a number of sensors that need to be aread and this is the
   area where extension is most likely.  To minimize processing time, only one "sensor group" is
   read on each update call.  The sensor array is an array of functions that is looped through continuously
   to ensure that all the sensors are read, but at the same time, each "read" minimizes the amount of time
   spent in the update call.
 */
static SENSOR_READ_FUNC_TYPE sensor_array[NUM_SENSOR_ENTRIES];


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

static void PrintDistanceSensorData(char *label, float *data)
{
    /* Need to write to the serial port an array of floats as a string */
}


/*---------------------------------------------------------------------------------------------------
 * Name: ReadInfraredFront/ReadInfraredRear
 * Description: Reads the infrared front/rear distances from the SPI network
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void ReadInfraredFront()
{
    float data[NUM_FRONT_INFRARED_SENSORS];
    
    Spi_ReadNode(SPI_NODE_1, (void *) data);
    I2c_WriteInfraredFrontDistances(data);
}

static void ReadInfraredRear()
{
    float data[NUM_REAR_INFRARED_SENSORS];
    
    Spi_ReadNode(SPI_NODE_2, (void *) data);
    I2c_WriteInfraredRearDistances(data);
}

/*---------------------------------------------------------------------------------------------------
 * Name: ReadUltrasonicFront/ReadUltratronicRear
 * Description: Reads the ultrasonic front/rear distances from the SPI network
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void ReadUltrasonicFront()
{
    float data[NUM_FRONT_ULTRASONIC_SENSORS];
    
    Spi_ReadNode(SPI_NODE_3, (void *) data);
    I2c_WriteUltrasonicFrontDistances(data);
}

static void ReadUltratronicRear()
{
    float data[NUM_REAR_ULTRASONIC_SENSORS];
    
    Spi_ReadNode(SPI_NODE_4, (void *) data);
    I2c_WriteUltrasonicRearDistances(data);
}

/*---------------------------------------------------------------------------------------------------
 * Name: ReadFrontDistanceSensors/ReadReadDistanceSensors
 * Description: Reads the front/rear distance sensors
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void ReadFrontDistanceSensors()
{
    ReadInfraredFront();
    ReadUltrasonicFront();
}

static void ReadReadDistanceSensors()
{
    ReadInfraredRear();
    ReadUltratronicRear();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Sensor_Init
 * Description: Initializes the sensor module.  The sensor module is an abstraction for various sensors
 *              supported on the Psoc.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Sensor_Init()
{
    Spi_Init();
    
    sensor_array[0] = ReadFrontDistanceSensors;
    sensor_array[1] = ReadReadDistanceSensors;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Sensor_Start
 * Description: Starts components within the sensor module.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Sensor_Start()
{
    Spi_Start();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Sensor_Update
 * Description: Called peridically from the main loop to update the sensor values.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Sensor_Update()
{
    static uint32 last_update_time = 0;
    static uint8 sensor_index = 0;
    uint32 delta_time;
    
    delta_time = millis() - last_update_time;
    SENSOR_DEBUG_DELTA(delta_time);
    if (delta_time >= SENSOR_SAMPLE_TIME_MS)
    {
        sensor_array[sensor_index]();        
        sensor_index = (sensor_index + 1) % NUM_SENSOR_ENTRIES;
    }
}


/* [] END OF FILE */
