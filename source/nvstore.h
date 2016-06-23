#include <project.h>

#ifndef NVSTORE_H
#define NVSTORE_H

/* EEPROM Storage layout:
 
    2048 total bytes (128 rows of 16 bytes)
    
    Calibration Storage: 1280 bytes (80 rows)
    Remaining Storage: 2048 - 1280 = 768 bytes (48 rows)
    
    Calibration Offset = 0
    Next available offset = 1280
    
    
 */
        
#define NVSTORE_CAL_EEPROM_BASE                 ((volatile CAL_EEPROM_TYPE *) CYDEV_EE_BASE);
#define NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(addr) ((uint16)((uint8 *)addr - (uint8 *) CYDEV_EE_BASE))
#define NVSTORE_CAL_EEPROM_SIZE                 (1280)

void Nvstore_Init();
void Nvstore_Start();
void Nvstore_WriteBytes(uint8 *bytes, uint16 num_bytes, uint16 offset);
void Nvstore_WriteUint16(uint16 value, uint16 offset);
void Nvstore_WriteFloat(float value, uint16 offset);

    
#endif