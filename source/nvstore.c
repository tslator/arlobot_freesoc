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

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "nvstore.h"
#include "assert.h"
#include "utils.h"


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Name: Nvstore_Init
 * Description: Initializes module variables
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Nvstore_Init()
{
}

/*---------------------------------------------------------------------------------------------------
 * Name: Nvstore_Start
 * Description: Starts the EEPROM component used for storing calibration information.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Nvstore_Start()
{
    EEPROM_Start();
    
    //EEPROM_EraseSector(0);
    //EEPROM_EraseSector(1);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Nvstore_WriteBytes
 * Description: Writes the specified number of bytes to the location given in offset.  
 *              Note: paging is handled within this routine.
 * Parameters: bytes - pointer to array of bytes to be written
 *             num_bytes - the number of bytes to be written
 *             offset - the offset from the base address of non-volatile storage
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Nvstore_WriteBytes(UINT8 *bytes, UINT16 num_bytes, UINT16 offset)
{
    UINT16 ii;
    
    // Need to calculate rowoffset from offset provided
    //
    //   Use EEPROM_WriteByte to write bytes that occur before the first complete row
    //   Use EEPROM_Write to a full row of bytes
    //   Use EEPROM_WriteByte to write remaining bytes that are less than a complete row
    
    UINT16 index = 0;
    UINT16 row_offset = offset / CYDEV_EEPROM_ROW_SIZE;
    UINT16 byte_offset = offset % CYDEV_EEPROM_ROW_SIZE;
    
    UINT16 first_partial_row_start = byte_offset;
    
    if (first_partial_row_start)
    {
        // write all the bytes up to the next row using EEPROM_WriteByte
        
        UINT16 partial_row_num_bytes = CYDEV_EEPROM_ROW_SIZE - first_partial_row_start;
        UINT16 start = (row_offset * CYDEV_EEPROM_ROW_SIZE) + first_partial_row_start;
        for (ii = 0; ii < partial_row_num_bytes; ++ii)
        {
            EEPROM_WriteByte(bytes[index], start + ii);
            index++;
            num_bytes--;
        }
        row_offset++;
    }

    UINT16 num_full_rows = num_bytes / CYDEV_EEPROM_ROW_SIZE;
    
    if (num_full_rows)
    {
        // Write out all of the full rows
        for (ii = 0; ii < num_full_rows; ++ii)
        {
            EEPROM_Write(&bytes[index], row_offset);
            index += CYDEV_EEPROM_ROW_SIZE;
            num_bytes -= CYDEV_EEPROM_ROW_SIZE;
            row_offset++;
        }        
    }
    
    UINT16 num_remaining = num_bytes;
    
    if (num_remaining)
    {
        // write all the bytes up to the next row using EEPROM_WriteByte
        
        UINT16 start = row_offset * CYDEV_EEPROM_ROW_SIZE;
        for (ii = 0; ii < num_remaining; ++ii)
        {
            EEPROM_WriteByte(bytes[index], start + ii);
            index++;
            num_bytes--;
        }
    }
    
    assert(num_bytes == 0);    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Nvstore_WriteUINT16
 * Description: Writes the specified 16-bit value to the location given in offset.
 * Parameters: value - a 16-bit value to be written
 *             offset - the offset from the base address of non-volatile storage
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Nvstore_WriteUint16(UINT16 value, UINT16 offset)
{
    UINT8 ii;
    UINT8 bytes[sizeof(UINT16)];
    
    Uint16ToTwoBytes(value, bytes);
    for (ii = 0; ii < sizeof(UINT16); ++ii)
    {
        EEPROM_WriteByte(bytes[ii], offset + ii);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Nvstore_WriteFloat
 * Description: Writes the specified FLOAT value to the location given in offset.
 * Parameters: value - a FLOATing point value to be written
 *             offset - the offset from the base address of non-volatile storage
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Nvstore_WriteFloat(FLOAT value, UINT16 offset)
{
    UINT8 ii;
    UINT8 bytes[sizeof(FLOAT)];
    
    FloatToFourBytes(value, bytes);
    for (ii = 0; ii < sizeof(FLOAT); ++ii)
    {
        EEPROM_WriteByte(bytes[ii], offset + ii); 
    }
}

/* [] END OF FILE */
