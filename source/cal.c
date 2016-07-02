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
#include <stdio.h>
#include <stdlib.h>
#include "cal.h"
#include "motor.h"
#include "i2c.h"
#include "pwm.h"
#include "encoder.h"
#include "utils.h"
#include "serial.h"
#include "pid.h"
#include "nvstore.h"
#include "calmotor.h"
#include "calpid.h"
#include "callin.h"
#include "calang.h"
#include "debug.h"

#define NO_CMD              (0)
#define CAL_REQUEST         (1)
#define MOTOR_CAL_CMD       (2)
#define MOTOR_VAL_CMD       (3)
#define PID_LEFT_CAL_CMD    (4)
#define PID_RIGHT_CAL_CMD   (5)
#define PID_VAL_CMD         (6)
#define LIN_CAL_CMD         (7)
#define LIN_VAL_CMD         (8)
#define ANG_CAL_CMD         (9)
#define ANG_VAL_CMD         (10)
#define EXIT_CMD            (11)

static void ClearCalibrationStatusBit(uint16 bit)
{
    uint16 status = p_cal_eeprom->status &= ~bit;
    Nvstore_WriteUint16(status, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->status));
    I2c_ClearCalibrationStatusBit(bit);
}

static void SetCalibrationStatusBit(uint16 bit)
{
    uint16 status = p_cal_eeprom->status | bit;
    Nvstore_WriteUint16(status, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->status));
    I2c_SetCalibrationStatusBit(bit);   
}

void Cal_Init()
{
    p_cal_eeprom = NVSTORE_CAL_EEPROM_BASE;
}

void Cal_Start()
{
    //Nvstore_WriteBytes((uint8 *) &left_fwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_motor_fwd));
    //Nvstore_WriteBytes((uint8 *) &left_bwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_motor_bwd));
    //Nvstore_WriteBytes((uint8 *) &right_fwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_motor_fwd));
    //Nvstore_WriteBytes((uint8 *) &right_bwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_motor_bwd));
    //Nvstore_WriteUint16(0, 0x000f);
    //Nvstore_WriteFloat(1.07, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->linear_bias));
    //Nvstore_WriteFloat(1.0625, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->angular_bias));

    I2c_SetCalibrationStatusBit(p_cal_eeprom->status);
}

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
            return MOTOR_VAL_CMD;
            break;
        
        case '3':
            Ser_WriteByte(value);
            return PID_LEFT_CAL_CMD;
            break;
            
        case '4':
            Ser_WriteByte(value);
            return PID_RIGHT_CAL_CMD;
            break;
            
        case '5':
            Ser_WriteByte(value);
            return PID_VAL_CMD;
            break;
        
        case '6':
            Ser_WriteByte(value);
            return LIN_CAL_CMD;
            break;
            
        case '7':
            Ser_WriteByte(value);
            return LIN_VAL_CMD;
    
        case '8':
            Ser_WriteByte(value);
            return ANG_CAL_CMD;
            break;
            
        case '9':
            Ser_WriteByte(value);
            return ANG_VAL_CMD;

        case 'x':
        case 'X':            
            Ser_WriteByte(value);
            is_cal_active = 0;
            return EXIT_CMD;
            break;
        
        default:
            return NO_CMD;
    }    
}

static void DisplayMenu()
{
    debug_control_enabled = 0;

    Ser_PutString("\r\nWelcome to the Arlobot calibration interface.\r\n");
    Ser_PutString("The following calibration operations are allowed:\r\n");
    Ser_PutString("    1. Motor Calibration - creates mapping between count/sec and PWM.\r\n");
    Ser_PutString("    2. Motor Validation - operates the motors at varying velocities.\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("    3. PID Left Calibration - enter gains, execute step input, print velocity response.\r\n");
    Ser_PutString("    4. PID Right Calibration - enter gains, execute step input, print velocity response.\r\n");
    Ser_PutString("    5. PID Validation - operates the motors at varying velocities.\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("    6. Linear Bias - moves forward 1 meter and allows user to enter a ratio to be applied as a bias in linear motion\r\n");
    Ser_PutString("    7. Validate Linear Bias - moves forward 1 meter applying the linear bias calculated in linear bias calibration\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("    8. Angular Bias - rotates 360 degrees and allows user to enter a ratio to be applied as a bias in angular motion\r\n");
    Ser_PutString("    9. Validate Angular Bias - rotates 360 degrees applying the angular bias calculated in angular bias calibration\r\n");
    Ser_PutString("\r\nEnter X to exit calibration, C to enter calibration\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("Make an entry [1-8,X]: ");
}

float Cal_ReadResponse()
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

void Cal_DisplayBias(char *label, float bias)
{
    char bias_str[6];
    char output[64];
    
    ftoa(bias, bias_str, 3);
    sprintf(output, "\r\n%s bias: %s\r\n", label, bias_str);
    Ser_PutString(output);
}

void Cal_CheckRequest()
{    
    uint8 cmd;
    float gains[3];
    
    do
    {
        Ser_Update();

        cmd = GetCommand();

        switch (cmd)
        {
            case CAL_REQUEST:
                DisplayMenu();
                break;
                
            case MOTOR_CAL_CMD:
                ClearCalibrationStatusBit(CAL_COUNT_PER_SEC_TO_PWM_BIT);
                CalibrateMotorVelocity();
                SetCalibrationStatusBit(CAL_COUNT_PER_SEC_TO_PWM_BIT);
                DisplayMenu();
                break;
                
            case MOTOR_VAL_CMD:
                ValidateMotorVelocity();
                DisplayMenu();
                break;
                
            case PID_LEFT_CAL_CMD:
                ClearCalibrationStatusBit(CAL_PID_BIT);
                Ser_PutString("\r\nEnter prop gain: ");
                gains[0] = Cal_ReadResponse();
                Ser_PutString("\r\nEnter integ gain: ");
                gains[1] = Cal_ReadResponse();
                Ser_PutString("\r\nEnter deriv gain: ");
                gains[2] = Cal_ReadResponse();
                CalibrateLeftPid(gains);
                SetCalibrationStatusBit(CAL_PID_BIT);
                DisplayMenu();
                break;
                
            case PID_RIGHT_CAL_CMD:
                ClearCalibrationStatusBit(CAL_PID_BIT);
                Ser_PutString("\r\nEnter prop gain: ");
                gains[0] = Cal_ReadResponse();
                Ser_PutString("\r\nEnter integ gain: ");
                gains[1] = Cal_ReadResponse();
                Ser_PutString("\r\nEnter deriv gain: ");
                gains[2] = Cal_ReadResponse();
                CalibrateRightPid(gains);
                SetCalibrationStatusBit(CAL_PID_BIT);
                DisplayMenu();
                break;
                
            case PID_VAL_CMD:
                ValidatePid();
                DisplayMenu();
                break;
                
            case LIN_CAL_CMD:
                ClearCalibrationStatusBit(CAL_LINEAR_BIAS_BIT);
                CalibrateLinearBias();
                SetCalibrationStatusBit(CAL_LINEAR_BIAS_BIT);
                DisplayMenu();
                break;
                
            case LIN_VAL_CMD:
                ValidateLinearBias();
                DisplayMenu();
                break;
                
            case ANG_CAL_CMD:
                ClearCalibrationStatusBit(CAL_ANGULAR_BIAS_BIT);
                CalibrateAngularBias();
                SetCalibrationStatusBit(CAL_ANGULAR_BIAS_BIT);
                DisplayMenu();
                break;
                
            case ANG_VAL_CMD:
                ValidateAngularBias();
                DisplayMenu();
                break;
                
            case EXIT_CMD:
                Ser_PutString("\r\nExiting calibration\r\n");
                break; 
                
            default:
                /* No command, so do nothing */
                break;
        }
    } while (cmd != EXIT_CMD && cmd != NO_CMD);
    
}

void Cal_LeftGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data)
{
    *fwd_cal_data = (CAL_DATA_TYPE *) &(p_cal_eeprom->left_motor_fwd);
    *bwd_cal_data = (CAL_DATA_TYPE *) &(p_cal_eeprom->left_motor_bwd);
}

void Cal_RightGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data)
{
    *fwd_cal_data = (CAL_DATA_TYPE *) &(p_cal_eeprom->right_motor_fwd);
    *bwd_cal_data = (CAL_DATA_TYPE *) &(p_cal_eeprom->right_motor_bwd);
}

CAL_PID_TYPE * Cal_LeftGetPidGains()
{
    return (CAL_PID_TYPE *) &p_cal_eeprom->left_gains;
}

CAL_PID_TYPE * Cal_RightGetPidGains()
{
    return (CAL_PID_TYPE *) &p_cal_eeprom->right_gains;
}

/* [] END OF FILE */
