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
#include "control.h"

#define NO_CMD              '0'
#define CAL_REQUEST         'c'
#define MOTOR_CAL_CMD       '1'
#define MOTOR_VAL_CMD       '2'
#define MOTOR_DUMP_CMD      '3'
#define PID_LEFT_CAL_CMD    '4'
#define PID_RIGHT_CAL_CMD   '5'
#define PID_VAL_CMD         '6'
#define LIN_CAL_CMD         '7'
#define LIN_VAL_CMD         '8'
#define ANG_CAL_CMD         '9'
#define ANG_VAL_CMD         'a'
#define EXIT_CMD            'x'


typedef uint8 (*INIT_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);
typedef uint8 (*START_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);
typedef uint8 (*UPDATE_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);
typedef uint8 (*STOP_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);
typedef uint8 (*RESULTS_FUNC_TYPE)(CAL_STAGE_TYPE stage, void *params);

typedef struct calibration_type_tag
{
    CAL_STATE_TYPE state;
    CAL_STAGE_TYPE stage;
    void *params;
    INIT_FUNC_TYPE init;
    START_FUNC_TYPE start;
    UPDATE_FUNC_TYPE update;
    STOP_FUNC_TYPE stop;
    RESULTS_FUNC_TYPE results;
} CALIBRATION_TYPE;

static CALIBRATION_TYPE motor_calibration = {CAL_INIT_STATE, 
                                             CAL_CALIBRATE_STAGE,
                                             0,
                                             CalMotor_Init, 
                                             CalMotor_Start, 
                                             CalMotor_Update, 
                                             CalMotor_Stop, 
                                             CalMotor_Results };

static CAL_PID_PARAMS left_pid_params = {"left", LEFT_WHEEL};
static CAL_PID_PARAMS right_pid_params = {"right", RIGHT_WHEEL};

static CALIBRATION_TYPE pid_calibration = {CAL_INIT_STATE,
                                           CAL_CALIBRATE_STAGE,
                                           0,
                                           CalPid_Init,
                                           CalPid_Start,
                                           CalPid_Update,
                                           CalPid_Stop,
                                           CalPid_Results};
static CALIBRATION_TYPE ctrl_linear_pid_calibration = {};
static CALIBRATION_TYPE ctrl_angular_pid_calibration = {};
static CALIBRATION_TYPE linear_bias_calibration = {};
static CALIBRATION_TYPE angular_bias_calibration = {};

static uint8 is_cal_invoked;
static CALIBRATION_TYPE *active_cal;

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

static void DisplayMenu()
{
    debug_control_enabled = 0;

    Ser_PutString("\r\nWelcome to the Arlobot calibration interface.\r\n");
    Ser_PutString("The following calibration operations are allowed:\r\n");
    Ser_PutString("    1. Motor Calibration - creates mapping between count/sec and PWM.\r\n");
    Ser_PutString("    2. Motor Validation - operates the motors at varying velocities.\r\n");
    Ser_PutString("    3. Motor Verification - prints the motor count/sec and PWM mapping.\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("    4. PID Left Calibration - enter gains, execute step input, print velocity response.\r\n");
    Ser_PutString("    5. PID Right Calibration - enter gains, execute step input, print velocity response.\r\n");
    Ser_PutString("    6. PID Validation - operates the motors at varying velocities.\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("    7. Linear Bias - moves forward 1 meter and allows user to enter a ratio to be applied as a bias in linear motion\r\n");
    Ser_PutString("    8. Validate Linear Bias - moves forward 1 meter applying the linear bias calculated in linear bias calibration\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("    9. Angular Bias - rotates 360 degrees and allows user to enter a ratio to be applied as a bias in angular motion\r\n");
    Ser_PutString("    a. Validate Angular Bias - rotates 360 degrees applying the angular bias calculated in angular bias calibration\r\n");
    Ser_PutString("\r\nEnter X to exit calibration\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("Make an entry [1-8,X]: ");
}

static void DisplayExit()
{
    Ser_PutString("\r\nExiting Arlobot calibration interface.  Type 'C' to enter calibration\r\n");
}

static uint8 GetCommand()
{
    char value = Ser_ReadByte();
        
    switch (value)
    {
        case 'c':
        case 'C':
            if (!is_cal_invoked) 
            {
                is_cal_invoked = 1;
                DisplayMenu();
            }
            value = CAL_REQUEST;
            break;

        case 'x':
        case 'X':            
            Ser_WriteByte(value);
            is_cal_invoked = 0;
            DisplayExit();
            value = EXIT_CMD;
            break;
        
        default:
            break;
    }
    
    return value;
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

static void ProcessCalibration(CALIBRATION_TYPE *cal)
{
    uint8 result;
    
    switch (cal->state)
    {
        case CAL_INIT_STATE:
            result = cal->init(cal->stage, cal->params);
            if (result == CAL_OK)
            {
                cal->state = CAL_START_STATE;
            }
            else
            {
                /* Handle the error */
            }
            break;
            
        case CAL_START_STATE:
            result = cal->start(cal->stage, cal->params);
            if (result == CAL_OK)
            {
                cal->state = CAL_RUNNING_STATE;
            }
            else
            {
                /* Handle the error */
            }
            break;
            
        case CAL_RUNNING_STATE:
            result = cal->update(cal->stage, cal->params);
            if (result == CAL_COMPLETE)
            {
                cal->state = CAL_STOP_STATE;
            }
            else
            {
                /* Handle the error */
            }
            break;
            
        case CAL_STOP_STATE:
            result = cal->stop(cal->stage, cal->params);
            if (result == CAL_OK)
            {
                cal->state = CAL_RESULTS_STATE;
            }
            else
            {
                /* Handle the error */
            }
            break;
            
        case CAL_RESULTS_STATE:
            result = cal->results(cal->stage, cal->params);
            if (result == CAL_OK)
            {
                cal->state = CAL_INIT_STATE;
            }
            else
            {
                /* Handle the error */
            }
    }
}

static CALIBRATION_TYPE* GetCalibration(uint8 cmd)
{
    switch (cmd)
    {
        case MOTOR_CAL_CMD:
            return &motor_calibration;
            
        case PID_LEFT_CAL_CMD:
            pid_calibration.params = &left_pid_params;
            return &pid_calibration;
        
        case PID_RIGHT_CAL_CMD:
            pid_calibration.params = &right_pid_params;
            return &pid_calibration;

        default:
            break;

    }
    
    return (CALIBRATION_TYPE *) 0;
}

void Cal_Update()
/* Check if we are in calibration mode
   if we are not in calibration mode then
       check if calibration has been requested
   if we are in calibration mode then           
       if there is an active calibration then process the calibration
       if there is no active calibration then check for a calibration command
 */
{
    if (is_cal_invoked)
    {
        /* User has previously requested to enter calibration 
        
           If there is an active calibration then continue processing until it completes
           If there is no active calibration then check for input from the user
        
        */
        
        if (active_cal)
        {
            ProcessCalibration(active_cal);
        }
        else
        {            
            uint8 cmd = GetCommand();
    
            active_cal = GetCalibration(cmd);
        }

                
    }
    else
    {
        /* Check for user request to enter calibration */
        uint8 cmd = GetCommand();
    }
}

void Cal_SetLeftRightVelocity(float left, float right)
{
    left_cmd_velocity = left;
    right_cmd_velocity = right;
}

float Cal_LeftTarget()
{
    return left_cmd_velocity;
}

float Cal_RightTarget()
{
    return right_cmd_velocity;
}


/*-------------------------------------------------------------------------------*/

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
    
    uint16 old_debug_control_enabled = debug_control_enabled;    
    
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
                ClearCalibrationStatusBit(CAL_MOTOR_BIT);
                CalibrateMotorVelocity();
                SetCalibrationStatusBit(CAL_MOTOR_BIT);
                DisplayMenu();
                break;
                
            case MOTOR_VAL_CMD:
                ValidateMotorVelocity();
                DisplayMenu();
                break;
                
            case MOTOR_DUMP_CMD:
                PrintMotorVelocity();
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
                Pid_SetLeftRightTarget(Control_LeftGetCmdVelocity, Control_RightGetCmdVelocity);
                Ser_PutString("\r\nExiting calibration\r\n");
                break; 
                
            default:
                /* No command, so do nothing */
                break;
        }
    } while (cmd != EXIT_CMD && cmd != NO_CMD);
    
    debug_control_enabled = old_debug_control_enabled;
}

/* [] END OF FILE */
