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
#include <project.h>

typedef struct _buffer_tag
{
    uint8 byte_0;
    uint8 byte_1;
    uint16 word_0;
    int16 word_1;
    uint32 dword_0;
    int32 dword_1;
    float float_0;
    uint32 counter;
} __attribute__ ((packed)) BUFFER_TYPE;

static BUFFER_TYPE buffer;

#define I2C_WAIT_FOR_ACCESS()   do  \
                                {   \
                                } while (0 !=  EZI2C_Slave_GetActivity())

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    buffer.byte_0 = 255;
    buffer.byte_1 = 45;
    buffer.word_0 = 2000;
    buffer.word_1 = -2000;
    buffer.dword_0 = 100000;
    buffer.dword_1 = -100000;
    buffer.float_0 = 3.14;
    buffer.counter = 0;

    EZI2C_Slave_Start();
    EZI2C_Slave_SetBuffer1(sizeof(BUFFER_TYPE), sizeof(BUFFER_TYPE), (uint8 *) &buffer);

    for(;;)
    {
        I2C_WAIT_FOR_ACCESS();
        EZI2C_Slave_DisableInt();
        buffer.counter++;
        EZI2C_Slave_EnableInt();
        CyDelay(1000);
        
    }
}

/* [] END OF FILE */
