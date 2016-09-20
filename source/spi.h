#ifndef SPI_H
#define SPI_H
    
#include <project.h>
    
/* Move these to a command file between the infrared node and the ultrasonic node */
#define CMD_REQUEST (1)
#define SOM (0x01)
#define US_TYPE (0x01)
#define IR_TYPE (0x02)
#define EOM (0xFE)
#define US_MAX_DIST (255) // in cm
#define IR_MAX_DIST (80)  // in cm
    
#define MAX_MSG_SIZE (11)

#ifdef IR_SENSOR
#define SENSOR_TYPE (IR_TYPE)
#define MAX_SENSOR_DIST (IR_MAX_DIST)
#endif    
#ifdef US_SENSOR
#define SENSOR_TYPE (US_TYPE)
#define MAX_SENSOR_DIST (US_MAX_DIST)
#endif    
#define NUM_SENSORS (8)
    
   
void Spi_Init();
void Spi_Start();
void Spi_Update();
    
#endif