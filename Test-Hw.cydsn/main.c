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
#include "../source/utils.h"
#include "../source/nvstore.h"

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

static void I2cTest()
{
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

static uint8 large_num_bytes[39] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB,
                                    0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00, 
                                    0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
                                    0xFF, 0xEE};

static uint8 byte_values[4];
static uint16 uint16_value;
static float float_value;


static void EepromTest()
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

static void Ser_Update()
{
    ///* Host can send double SET_INTERFACE request. */
    if (0u != USBUART_IsConfigurationChanged())
    {
        /* Initialize IN endpoints when device is configured. */
        if (0u != USBUART_GetConfiguration())
        {
            /* Enumeration is done, enable OUT endpoint to receive data 
             * from host. */
            USBUART_CDC_Init();
        }
    }
}

static uint8 Ser_ReadByte()
{
    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            /* Read received data and re-enable OUT endpoint. */
            return USBUART_GetChar();
        }
    }
    return 0;
}

static void Ser_WriteByte(uint8 value)
{
    if (0u != USBUART_GetConfiguration())
    {
        while (0u == USBUART_CDCIsReady())
        {
        }
        
        USBUART_PutChar(value);
    }
}

static void Ser_WriteString(char *str)
{
    if (0u != USBUART_GetConfiguration())
    {
        while (0u == USBUART_CDCIsReady())
        {
        }
        
        USBUART_PutString(str);
    }
}

static uint16 Ser_ReadString(char *str)
{
    uint16 num_bytes = 0;
    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            /* Read received data and re-enable OUT endpoint. */
            num_bytes = USBUART_GetAll((uint8 *) str);
        }
    }
    return num_bytes;
}

static uint8 Ser_IsDataReady()
{
    uint8 result = 0;
    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        result = USBUART_DataIsReady();
    }
    return result;
}

static void UsbuartTest()
{
    #define USBFS_DEVICE    (0u)

    /* The buffer size is equal to the maximum packet size of the IN and OUT bulk
    * endpoints.
    */
    #define USBUART_BUFFER_SIZE (64u)
    
    uint16 count;
    uint8 buffer[USBUART_BUFFER_SIZE];
    
    /* Start USBFS operation with 5-V operation. */
    USBUART_Start(USBFS_DEVICE, USBUART_5V_OPERATION);
    
    #define PROMPT (0)
    #define WAIT_FOR_CMD (1)
    #define ECHO_CMD (2)
    #define DONE (3)
    uint8 state = PROMPT;
    uint8 index;
    
    for(;;)
    {
        Ser_Update();

        switch (state)
        {
            case PROMPT:
                Ser_WriteString("This is a string test: \r\n");
                state = WAIT_FOR_CMD;
                index = 0;
                break;
                
            case WAIT_FOR_CMD:
                if (Ser_IsDataReady())
                {
                    buffer[index] = Ser_ReadByte();
                    index++;
                    
                    if (index == 5)
                    {
                        state = ECHO_CMD;
                    }
                }
                break;
                
            case ECHO_CMD:
                Ser_WriteString("Received command\r\n");
                state = DONE;
                break;
                
            case DONE:
                break;
        }
        
        
        #ifdef XXX
        /* Service USB CDC when device is configured. */
        if (0u != USBUART_GetConfiguration())
        {
            /* Check for input data from host. */
            if (0u != USBUART_DataIsReady())
            {
                /* Read received data and re-enable OUT endpoint. */
                count = USBUART_GetAll(buffer);

                if (0u != count)
                {
                    /* Wait until component is ready to send data to host. */
                    while (0u == USBUART_CDCIsReady())
                    {
                    }

                    /* Send data back to host. */
                    USBUART_PutData(buffer, count);

                    /* If the last sent packet is exactly the maximum packet 
                    *  size, it is followed by a zero-length packet to assure
                    *  that the end of the segment is properly identified by 
                    *  the terminal.
                    */
                    if (USBUART_BUFFER_SIZE == count)
                    {
                        /* Wait until component is ready to send data to PC. */
                        while (0u == USBUART_CDCIsReady())
                        {
                        }

                        /* Send zero-length packet to PC. */
                        USBUART_PutData(NULL, 0u);
                    }
                }
            }
        }
        #endif
    }
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //I2cTest();
    //EepromTest();
    UsbuartTest();
}

/* [] END OF FILE */
