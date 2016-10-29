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
void Nvstore_WriteBytes(uint8 *bytes, uint16 num_bytes, uint16 offset)
{
    uint16 ii;
    
    // Need to calculate rowoffset from offset provided
    //
    //   Use EEPROM_WriteByte to write bytes that occur before the first complete row
    //   Use EEPROM_Write to a full row of bytes
    //   Use EEPROM_WriteByte to write remaining bytes that are less than a complete row
    
    uint16 index = 0;
    uint16 row_offset = offset / CYDEV_EEPROM_ROW_SIZE;
    uint16 byte_offset = offset % CYDEV_EEPROM_ROW_SIZE;
    
    uint16 first_partial_row_start = byte_offset;
    
    if (first_partial_row_start)
    {
        // write all the bytes up to the next row using EEPROM_WriteByte
        
        uint16 partial_row_num_bytes = CYDEV_EEPROM_ROW_SIZE - first_partial_row_start;
        uint16 start = (row_offset * CYDEV_EEPROM_ROW_SIZE) + first_partial_row_start;
        for (ii = 0; ii < partial_row_num_bytes; ++ii)
        {
            EEPROM_WriteByte(bytes[index], start + ii);
            index++;
            num_bytes--;
        }
        row_offset++;
    }

    uint16 num_full_rows = num_bytes / CYDEV_EEPROM_ROW_SIZE;
    
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
    
    uint16 num_remaining = num_bytes;
    
    if (num_remaining)
    {
        // write all the bytes up to the next row using EEPROM_WriteByte
        
        uint16 start = row_offset * CYDEV_EEPROM_ROW_SIZE;
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
 * Name: Nvstore_WriteUint16
 * Description: Writes the specified 16-bit value to the location given in offset.
 * Parameters: value - a 16-bit value to be written
 *             offset - the offset from the base address of non-volatile storage
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Nvstore_WriteUint16(uint16 value, uint16 offset)
{
    uint8 ii;
    uint8 bytes[sizeof(uint16)];
    
    Uint16ToTwoBytes(value, bytes);
    for (ii = 0; ii < sizeof(uint16); ++ii)
    {
        EEPROM_WriteByte(bytes[ii], offset + ii);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Nvstore_WriteFloat
 * Description: Writes the specified float value to the location given in offset.
 * Parameters: value - a floating point value to be written
 *             offset - the offset from the base address of non-volatile storage
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Nvstore_WriteFloat(float value, uint16 offset)
{
    uint8 ii;
    uint8 bytes[sizeof(float)];
    
    FloatToFourBytes(value, bytes);
    for (ii = 0; ii < sizeof(float); ++ii)
    {
        EEPROM_WriteByte(bytes[ii], offset + ii); 
    }
}
