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

#include "serialtest.h"
#include <stdlib.h>
#include <stdio.h>
#include "../source/utils.h"

#define USBFS_DEVICE    (0u)

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)

uint16 count;
uint8 buffer[USBUART_BUFFER_SIZE];
    


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

static uint16 Ser_ReadBytes(uint8 *bytes, uint16 num_bytes)
{
    uint16 count;
    
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {            
            while ((count = USBUART_GetCount()) < num_bytes)
            {
                ;
            }
            
            /* Read received data and re-enable OUT endpoint. */
            //count = USBUART_GetAll(bytes);
            count = USBUART_GetData(bytes, num_bytes);
        }
    }
    
    return count;
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

static void Ser_WriteBytes(uint8 *bytes, uint8 num_bytes)
{
    if (0u != USBUART_GetConfiguration())
    {
        while (0u == USBUART_CDCIsReady())
        {
        }
        
        USBUART_PutData(bytes, num_bytes);
    }
}

static void Ser_PutString(char *str)
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

static void example()
{
    for (;;)
    {
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
    }
}

static void echochar()
{
    for (;;)
    {
        Ser_Update();
        
        uint8 value = Ser_ReadByte();
        Ser_WriteByte(value);
    }
}

#define NO_CMD          (0)
#define CAL_REQUEST     (1)
#define MOTOR_CAL_CMD   (2)
#define PID_CAL_CMD     (3)
#define LIN_CAL_CMD     (4)
#define ANG_CAL_CMD     (5)
#define EXIT_CMD        (6)


static uint8 GetCommand()
{
    static uint8 is_cal_active = 0;
    char value = Ser_ReadByte();
        
    switch (value)
    {
        case 'c':
        case 'C':
            if (is_cal_active) return 0;
            is_cal_active = 1;
            return CAL_REQUEST;
            break;
        
        case '1':
            Ser_WriteByte(value);
            return MOTOR_CAL_CMD;
            break;
        
        case '2':
            Ser_WriteByte(value);
            return PID_CAL_CMD;
            break;
        
        case '3':
            Ser_WriteByte(value);
            return LIN_CAL_CMD;
            break;
    
        case '4':
            Ser_WriteByte(value);
            return ANG_CAL_CMD;
            break;

        case 'x':
        case 'X':            
            Ser_WriteByte(value);
            is_cal_active = 0;
            return EXIT_CMD;
            break;
        
        default:
            return 0;
    }    
}

static void DisplayMenu()
{
    Ser_PutString("\r\nWelcome to the Arlobot calibration interface.\r\n");
    Ser_PutString("The following calibration operations are allowed:\r\n");
    Ser_PutString("    1. Motor Calibration - creates mapping between count/sec and PWM.\r\n");
    Ser_PutString("    2. PID Calibration - determines the PID gains that minimizes velocity error.\r\n");
    Ser_PutString("    3. Linear Bias - moves forward 1 meter and allows user to enter a ratio to be applied as a bias in linear motion\r\n");
    Ser_PutString("    4. Angular Bias - rotates 360 degrees and allows user to enter a ratio to be applied as a bias in angular motion\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("Enter a number [1-4]: ");
}

static float ReadResponse()
{
    uint8 digits[6];
    uint8 index = 0;
    uint8 value;
    do
    {
        Ser_Update();
        
        value = Ser_ReadByte();
        Ser_WriteByte(value);
        if ( (value >= '0' && value <= '9') || value == '.')
        {
            digits[index] = value;
            index++;
        }
    } while (index < 5);
    
    digits[5] = '\0';
    
    float result = atof((char *) digits);
    
    return result;
}

static void DisplayBias(char *label, float bias)
{
    char bias_str[6];
    char output[64];
    
    ftoa(bias, bias_str, 3);
    sprintf(output, "\r\n%s bias: %s\r\n", label, bias_str);
    Ser_PutString(output);
}

static void writeread()
{
    for (;;)
    {
        Ser_Update();

        uint8 cmd = GetCommand();

        switch (cmd)
        {
            case CAL_REQUEST:
                DisplayMenu();
                break;           
            case MOTOR_CAL_CMD:
                Ser_PutString("\r\nPerforming motor calibration\r\n");
                CyDelay(1000);
                Ser_PutString("Motor calibration complete\r\n");
                DisplayMenu();
                break;           
            case PID_CAL_CMD:
                Ser_PutString("\r\nPerforming pid calibration\r\n");
                CyDelay(1000);
                Ser_PutString("PID calibration complete\r\n");
                DisplayMenu();
                break;           
            case LIN_CAL_CMD:
                Ser_PutString("\r\nPerforming linear bias calibration\r\n");
                CyDelay(1000);
                Ser_PutString("Enter the measured distance in meters, e.g. 1.023, (5 characters max): ");
                float meas_dist = ReadResponse();
                float linear_bias = 1.0 / meas_dist;
                DisplayBias("linear", linear_bias);
                Ser_PutString("Linear bias calibration complete\r\n");
                DisplayMenu();
                break;           
            case ANG_CAL_CMD:
                Ser_PutString("\r\nPerforming angular bias calibration\r\n");
                CyDelay(1000);
                Ser_PutString("Enter the measured rotation in degrees, e.g. 345.0, (5 characters max): ");
                float meas_rotation = ReadResponse();
                float angular_bias = 360 / meas_rotation;
                DisplayBias("angular", angular_bias);
                Ser_PutString("Angular bias calibration complete\r\n");
                DisplayMenu();
                break;           
            case EXIT_CMD:
                Ser_PutString("\r\nExiting calibration\r\n");
                break; 
                
            default:
                /* No command, so do nothing */
                break;
        }
    }
}

void UsbuartTest()
{
    /* Start USBFS operation with 5-V operation. */
    USBUART_Start(USBFS_DEVICE, USBUART_5V_OPERATION);

    //example();
    //echochar();
    writeread();
}



/* [] END OF FILE */
