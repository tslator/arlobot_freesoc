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

#include "eepromtest.h"
#include "../source/utils.h"


#include "../source/nvstore.h"

static uint8 large_num_bytes[39] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB,
                                    0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00, 
                                    0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
                                    0xFF, 0xEE};

static uint8 byte_values[4];
static uint16 uint16_value;
static float float_value;


void EepromTest()
{
    EEPROM_Start();
    
    /* Map a pointer to the EEPROM memory */
    volatile uint8 *p_eeprom = (volatile uint8 *) CYDEV_EE_BASE;
    
    //EEPROM_WriteByte(0x11, 0);
    //EEPROM_WriteByte(0x22, 1);
    //EEPROM_WriteByte(0x33, 2);
    //EEPROM_WriteByte(0x44, 3);

    uint16_value = TwoBytesToUint16((uint8 *) p_eeprom);
    Uint16ToTwoBytes(uint16_value, byte_values);
    
    float_value = 716.5322875976562;
    FloatToFourBytes(float_value, byte_values);
    float_value = FourBytesToFloat((uint8 *) p_eeprom);
    
    uint16_value = 0;
    Nvstore_WriteUint16(0x5566, 4);
    uint16_value = TwoBytesToUint16((uint8 *) (p_eeprom + 4));
    Uint16ToTwoBytes(uint16_value, byte_values);
        
    float_value = 3.14;
    Nvstore_WriteFloat(float_value, 6);
    float_value = 0;
    float_value = FourBytesToFloat((uint8 *) (p_eeprom + 6));
    FloatToFourBytes(float_value, byte_values);

    Nvstore_WriteBytes(large_num_bytes, sizeof(large_num_bytes), 10);
    
    for(;;)
    {
    }
}


/* [] END OF FILE */
