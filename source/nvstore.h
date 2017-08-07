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
   Description: This module provides a wrapper around NVRAM storage.  NVRAM is being used for 
   parameter storage at the moment.
 *-------------------------------------------------------------------------------------------------*/


#ifndef NVSTORE_H
#define NVSTORE_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <project.h>


/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
/* EEPROM Storage layout:
 
    2048 total bytes (128 rows of 16 bytes)
    
    Calibration Storage: 1280 bytes (80 rows)
    Remaining Storage: 2048 - 1280 = 768 bytes (48 rows)
    
    Calibration Offset = 0
    Next available offset = 1280
    
    
 */
        
#define NVSTORE_CAL_EEPROM_BASE                 ((volatile CAL_EEPROM_TYPE *) CYDEV_EE_BASE);
#define NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(addr) ((uint16)((uint8 *)addr - (uint8 *) CYDEV_EE_BASE))

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define NVSTORE_CAL_EEPROM_SIZE                 (1280)

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Nvstore_Init();
void Nvstore_Start();
void Nvstore_WriteBytes(uint8 *bytes, uint16 num_bytes, uint16 offset);
void Nvstore_WriteUint16(uint16 value, uint16 offset);
void Nvstore_WriteFloat(float value, uint16 offset);

    
#endif