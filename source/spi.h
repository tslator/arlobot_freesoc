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

#ifndef SPI_H
#define SPI_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <project.h>
    
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef enum {SPI_NODE_1, SPI_NODE_2, SPI_NODE_3, SPI_NODE_4, NUM_SPI_NODES} SPI_NODE_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void Spi_Init();
void Spi_Start();
void Spi_ReadNode(SPI_NODE_TYPE node, void *data);
    
#endif

/* [] END OF FILE */
