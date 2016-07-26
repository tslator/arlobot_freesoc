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
#define VAL_REQUEST         'v'
#define SETTING_REQUEST     's'
#define EXIT_CMD            'x'

#define MOTOR_CAL_CMD       '1'
#define PID_LEFT_CAL_CMD    '2'
#define PID_RIGHT_CAL_CMD   '3'
#define LIN_PID_CAL_CMD     '4'
#define ANG_PID_CAL_CMD     '5'
#define LIN_BIAS_CAL_CMD    '6'
#define ANG_BIAS_CAL_CMD    '7'

#define MOTOR_VAL_CMD       '1'
#define PID_VAL_CMD         '2'
#define LIN_VAL_CMD         '3'
#define ANG_VAL_CMD         '4'

#define MOTOR_LEFT_DISP_CMD  '1'
#define MOTOR_RIGHT_DISP_CMD '2'
#define PID_DISP_CMD         '3'
#define BIAS_DISP_CMD        '4'
#define ALL_DISP_CMD         '5'


typedef enum {UI_STATE_INIT, UI_STATE_CALIBRATION, UI_STATE_VALIDATION, UI_STATE_SETTINGS, UI_STATE_EXIT} UI_STATE_ENUM;


static CALIBRATION_TYPE ctrl_linear_pid_calibration = {};
static CALIBRATION_TYPE ctrl_angular_pid_calibration = {};
static CALIBRATION_TYPE linear_bias_calibration = {};
static CALIBRATION_TYPE angular_bias_calibration = {};

static UI_STATE_ENUM ui_state;

static CALIBRATION_TYPE *active_cal;
static CALIBRATION_TYPE *active_val;

static float left_cmd_velocity;
static float right_cmd_velocity;

static UI_STATE_ENUM ui_state;

static float LeftTarget()
{
    return left_cmd_velocity;
}

static float RightTarget()
{
    return right_cmd_velocity;
}

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

static void DisplayCalMenu()
{
    debug_control_enabled = 0;

    Ser_PutString("\r\nWelcome to the Arlobot calibration interface.\r\n");
    Ser_PutString("The following calibration operations are allowed:\r\n");
    Ser_PutString("    1. Motor Calibration - creates mapping between count/sec and PWM.\r\n");
    Ser_PutString("    2. PID Left Calibration - enter gains, execute step input, print velocity response.\r\n");
    Ser_PutString("    3. PID Right Calibration - enter gains, execute step input, print velocity response.\r\n");
    Ser_PutString("    4. Linear PID Calibration - moves forward 1 meter and allows user to enter a ratio to be applied as a bias in linear motion\r\n");
    Ser_PutString("    5. Angular PID Calibration - moves forward 1 meter applying the linear bias calculated in linear bias calibration\r\n");
    Ser_PutString("    6. Linear Bias - moves forward 1 meter and allows user to enter a ratio to be applied as a bias in linear motion\r\n");
    Ser_PutString("    7. Angular Bias - moves forward 1 meter applying the linear bias calculated in linear bias calibration\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("\r\nEnter X to exit calibration\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("Make an entry [1-7,X]: ");
}

static void DisplayValMenu()
{
    debug_control_enabled = 0;

    Ser_PutString("\r\nWelcome to the Arlobot validation interface.\r\n");
    Ser_PutString("The following validation operations are allowed:\r\n");
    Ser_PutString("    1. Motor Validation - operates the motors at varying velocities.\r\n");
    Ser_PutString("    2. Left/Right/Control PID Validation - operates the motors at varying velocities, moves in a straight line and rotates in place.\r\n");
    Ser_PutString("    3. Linear Bias Validation - moves forward 1 meter and allows user to enter a ratio to be applied as a bias in linear motion\r\n");
    Ser_PutString("    4. Angular Bias Validation - moves forward 1 meter applying the linear bias calculated in linear bias calibration\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("\r\nEnter X to exit validation\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("Make an entry [1-4,X]: ");
}

static void DisplaySettingsMenu()
{
    Ser_PutString("\r\nWelcome to the Arlobot settings display\r\n");
    Ser_PutString("The following settings can be displayed\r\n");
    Ser_PutString("    1. Left Motor Calibration\r\n");
    Ser_PutString("    2. Right Motor Calibration\r\n");
    Ser_PutString("    3. PID Gains: left pid, right pid, linear pid, angular pid\r\n");
    Ser_PutString("    4. Linear/Angular Bias\r\n");
    Ser_PutString("    5. Display All\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("\r\nEnter X to exit validation\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("Make an entry [1-5,X]: ");
}

static void DisplayExit()
{
    Ser_PutString("\r\nExiting Arlobot calibration/validation interface.");
    Ser_PutString("\r\nType 'C' to enter calibration, 'V' to enter validation, 'S' to display settings\r\n");
}

static uint8 GetCommand()
{
    char value = Ser_ReadByte();
        
    switch (value)
    {
        case 'c':
        case 'C':
            value = CAL_REQUEST;
            break;
            
        case 'v':
        case 'V':
            value = VAL_REQUEST;
            break;
            
        case 's':
        case 'S':
            value = SETTING_REQUEST;
            break;

        case 'x':
        case 'X':            
            value = EXIT_CMD;
            break;
    }
    
    return value;
}

static CALIBRATION_TYPE* GetCalibration(uint8 cmd)
{
    switch (cmd)
    {
        case MOTOR_CAL_CMD:
            Ser_WriteByte(cmd);
            CalMotor_Calibration->state = CAL_INIT_STATE;
            return CalMotor_Calibration;
            
        case PID_LEFT_CAL_CMD:
            Ser_WriteByte(cmd);
            CalPid_LeftCalibration->state = CAL_INIT_STATE;
            return CalPid_LeftCalibration;
            break;
        
        case PID_RIGHT_CAL_CMD:
            Ser_WriteByte(cmd);
            CalPid_RightCalibration->state = CAL_INIT_STATE;
            return CalPid_RightCalibration;
            break;

        case LIN_PID_CAL_CMD:
            Ser_WriteByte(cmd);
            CalPid_LinearCalibration->state = CAL_INIT_STATE;
            return CalPid_LinearCalibration;
            break;
        
        case ANG_PID_CAL_CMD:
            Ser_WriteByte(cmd);
            CalPid_AngularCalibration->state = CAL_INIT_STATE;
            return CalPid_AngularCalibration;
            break;
        
        case LIN_BIAS_CAL_CMD:
            Ser_WriteByte(cmd);
            CalAng_Calibration->state = CAL_INIT_STATE;
            return CalAng_Calibration;
            break;
        
        case ANG_BIAS_CAL_CMD:
            Ser_WriteByte(cmd);
            CalAng_Calibration->state = CAL_INIT_STATE;
            return CalAng_Calibration;
            break;
        
        default:
            break;
    }
    
    return (CALIBRATION_TYPE *) 0;
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
            /* Figure out how to output a progress indication ... that will work for all calibration/validation */
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
                cal->state = CAL_DONE_STATE;                
            }
            else
            {
                /* Handle the error */
            }
            break;
            
        case CAL_DONE_STATE:
        default:
            if (active_cal)
            {
                active_cal = (CALIBRATION_TYPE *) 0;
                DisplayCalMenu();
            }
            break;
    }
}

static CALIBRATION_TYPE* GetValidation(uint8 cmd)
{
    switch (cmd)
    {
        case MOTOR_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Motor Validation by operating the motors at varying velocities.\r\n");
            CalMotor_Validation->state = CAL_INIT_STATE;
            return (CALIBRATION_TYPE *) CalMotor_Validation;
            break;
            
        case PID_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Left/Right PID Validation by operating the motors at varying velocities.\r\n");
            Ser_PutString("\r\nPerforming Control PID Validation by driving in a straight line, rotating left/right 360 degrees.\r\n");
            return (CALIBRATION_TYPE *) 1;
            break;
        
        case LIN_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Linear Bias Validation by moving forward 1 meter then moving backward 1 meter, returning to the original position\r\n");
            return (CALIBRATION_TYPE *) 1;
            break;
            
        case ANG_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Angular Bias Validation by rotating right 360 degrees then rotating left 360 degress, returning to the original position\r\n");
            return (CALIBRATION_TYPE *) 1;
            break;

            
        default:            
            break;

    }
    
    return (CALIBRATION_TYPE *) 0;
}

static void ProcessValidation(CALIBRATION_TYPE *val)
{
    if (active_val)
    {
        active_val = (CALIBRATION_TYPE *) 0;
        DisplayValMenu();
    }
}

static void ProcessSettings(uint8 cmd)
{
    switch (cmd)
    {
        case MOTOR_LEFT_DISP_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nDisplaying left motor calibration: count/sec, pwm mapping");
            Ser_PutString("\r\n");
            DisplaySettingsMenu();
            break;
            
        case MOTOR_RIGHT_DISP_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nDisplaying right motor calibration: count/sec, pwm mapping");
            Ser_PutString("\r\n");
            DisplaySettingsMenu();
            break;
            
        case PID_DISP_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nDisplaying all PID gains: left, right, linear, and angular");
            Ser_PutString("\r\n");
            DisplaySettingsMenu();
            break;
            
        case BIAS_DISP_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nDisplaying linear and angular bias settings");
            Ser_PutString("\r\n");
            DisplaySettingsMenu();
            break;
            
        case ALL_DISP_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nDisplaying all calibration/settings");
            Ser_PutString("\r\n");
            DisplaySettingsMenu();
            break;        
    }
}

void Cal_Init()
{
    p_cal_eeprom = NVSTORE_CAL_EEPROM_BASE;
    Cal_LeftTarget = LeftTarget;
    Cal_RightTarget = RightTarget;
    
    CalMotor_Init();
    CalPid_Init();
    CalLin_Init();
    CalAng_Init();
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

void Cal_Update()
/* 
    The following options are available in this modules:
        1. Calibration
        2. Validation
        3. Settings Display
    Only one can be active at a time

    State Machine:
        STATE_INIT - waiting for command to engage one of the above functionalities.  
            If command is 'c'|'C', then enter STATE_CALIBRATION
            If command is 'v'|'V', then enter STATE_VALIDATION
            If command is 's'|'S', then enter STATE_SETTINGS
            If command is 'x'|'X', then return to STATE_INIT

        STATE_CALIBRATION - waiting for calibration commands (as shown in calibration menu)
            ...
            If command is 'x'|'X', then return to STATE_INIT

        STATE_VALIDATION - waiting for validation commands (as shown in validation menu)
            ...
            If command is 'x'|'X', then return to STATE_INIT
        
        STATE_SETTINGS - waiting for setting commands (as shown in the settings menu)
            ...
            If command is 'x'|'X', then return to STATE_INIT

        STATE_EXIT - display exit menu and transitions to STATE_INIT
 */
{
    uint8 cmd = GetCommand();
    
    switch (ui_state)
    {
        case UI_STATE_INIT:
            {        
                switch (cmd)
                {
                    case CAL_REQUEST:
                        DisplayCalMenu();
                        ui_state = UI_STATE_CALIBRATION;
                        break;
                    
                    case VAL_REQUEST:
                        DisplayValMenu();
                        ui_state = UI_STATE_VALIDATION;
                        break;
                    
                    case SETTING_REQUEST:
                        DisplaySettingsMenu();
                        ui_state = UI_STATE_SETTINGS;
                        break;
                }
                break;
            }
            break;
        
        case UI_STATE_CALIBRATION:
            if (!active_cal)
            {
                active_cal = GetCalibration(cmd);
            }            
                
            if (active_cal)
            {
                ProcessCalibration(active_cal);
            }                        
            
            if (cmd == EXIT_CMD)
            {
                ui_state = UI_STATE_EXIT;
            }
            break;
            
        case UI_STATE_VALIDATION:
            if (!active_val)
            {
                active_val = GetValidation(cmd);
            }
            
            if (active_val)
            {
                ProcessValidation(active_val);
            }
            
            if (cmd == EXIT_CMD)
            {
                ui_state = UI_STATE_EXIT;
            }
            break;
            
        case UI_STATE_SETTINGS:
            ProcessSettings(cmd);
            
            if (cmd == EXIT_CMD)
            {
                ui_state = UI_STATE_EXIT;
            }
            break;

        case UI_STATE_EXIT:
            DisplayExit();
            ui_state = UI_STATE_INIT;
            break;
            
        default:
            ui_state = UI_STATE_INIT;                
            break;
    }
    
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

CAL_PID_TYPE* Cal_LinearGetPidGains()
{
    return (CAL_PID_TYPE*) &p_cal_eeprom->linear_gains;
}

CAL_PID_TYPE* Cal_AngularGetPidGains()
{
    return (CAL_PID_TYPE*) &p_cal_eeprom->angular_gains;
}

void Cal_SetLeftRightVelocity(float left, float right)
{
    left_cmd_velocity = left;
    right_cmd_velocity = right;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
