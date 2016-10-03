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
 * Includes
 *-------------------------------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/

/* Calibration interface commands */
#define NO_CMD              '0'
#define CAL_REQUEST         'c'
#define VAL_REQUEST         'v'
#define SETTING_REQUEST     's'
#define EXIT_CMD            'x'

#define MOTOR_CAL_CMD       '1'
#define PID_LEFT_CAL_CMD    '2'
#define PID_RIGHT_CAL_CMD   '3'

#define MOTOR_VAL_CMD           '1'
#define PID_VAL_CMD             '2'
#define PID_LEFT_FWD_VAL_CMD    '3'
#define PID_LEFT_BWD_VAL_CMD    '4'
#define PID_RIGHT_FWD_VAL_CMD   '5'
#define PID_RIGHT_BWD_VAL_CMD   '6'
#define LINEAR_FWD_VAL_CMD      '7'
#define LINEAR_BWD_VAL_CMD      '8'

#define MOTOR_LEFT_DISP_CMD  '1'
#define MOTOR_RIGHT_DISP_CMD '2'
#define PID_DISP_CMD         '3'
#define ALL_DISP_CMD         '4'

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef enum {UI_STATE_INIT, UI_STATE_CALIBRATION, UI_STATE_VALIDATION, UI_STATE_SETTINGS, UI_STATE_EXIT} UI_STATE_ENUM;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/

char* cal_stage_to_string[] = {"CALIBRATION", "VALIDATE"};
char* cal_state_to_string[] = {"INIT STATE", "START STATE", "RUNNING STATE", "STOP STATE", "RESULTS STATE", "DONE STATE"};

static UI_STATE_ENUM ui_state;

static CALIBRATION_TYPE *active_cal;
static CALIBRATION_TYPE *active_val;

static float left_cmd_velocity;
static float right_cmd_velocity;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: LeftTarget/RightTarget
 * Description: Local functions used to replace the left/right velocity source for calibration/validation.
 *              Normally, the left/right velocity is received from the I2C module via the Control
 *              module.  However, for calibration/validation left/right velocity values under control
 *              of the calibration module via Left/RightTarget.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static float LeftTarget()
{
    return left_cmd_velocity;
}

static float RightTarget()
{
    return right_cmd_velocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: ClearCalibrationStatusBit/SetCalibrationStatusBit
 * Description: Clears/Sets the specified bit in the calibration status register and the EEPROM 
 *              status field.
 * Parameters: bit - the bit number to be Cleared/Set - 0 .. 15.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void ClearCalibrationStatusBit(uint16 bit)
{
    uint16 status = p_cal_eeprom->status &= ~bit;
    Nvstore_WriteUint16(status, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->status));
    I2c_ClearCalibrationStatusBit(bit);
}

void SetCalibrationStatusBit(uint16 bit)
{
    uint16 status = p_cal_eeprom->status | bit;
    Nvstore_WriteUint16(status, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->status));
    I2c_SetCalibrationStatusBit(bit);   
}


/*---------------------------------------------------------------------------------------------------
 * Calibration Print Routines 
 *  
 * Routines used by other calibration modules for printing values to the serial port 
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintSamples
 * Description: Prints the count/sec and pwm samples.  Called from the CalMotor module.
 * Parameters: label - string containing the motor identifier, e.g., left or right 
 *             cps_samples - an array of count/sec samples
 *             pwm_samples - an array of pwm samples
 *             Note: Array size is given by CAL_NUM_SAMPLES
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_PrintSamples(char *label, int32 *cps_samples, uint16 *pwm_samples)
{
    uint8 ii;
    char  buffer[100];
    char label_str[20];
    
    sprintf(label_str, "%s\r\n", label);
    Ser_PutString(label_str);
    
    for (ii = 0; ii < CAL_NUM_SAMPLES - 1; ++ii)
    {
        sprintf(buffer, "%ld:%d ", cps_samples[ii], pwm_samples[ii]);
        Ser_PutString(buffer);
    }
    sprintf(buffer, "%ld:%d\r\n\r\n", cps_samples[ii], pwm_samples[ii]);
    Ser_PutString(buffer);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintGains
 * Description: Prints the PID gains.  Called from the CalPid module.
 * Parameters: label - string containing the pid identifier, e.g., left pid or right pid. 
 *             gains - an array of float values corresponding to Kp, Ki, and Kd. 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_PrintGains(char *label, float *gains)
{
    char output[64];
    char pgain_str[10];
    char igain_str[10];
    char dgain_str[10];
    
    ftoa(gains[0], pgain_str, 3);
    ftoa(gains[1], igain_str, 3);
    ftoa(gains[2], dgain_str, 3);
    
    sprintf(output, "%s - P: %s, I: %s, D: %s\r\n", label, pgain_str, igain_str, dgain_str);
    Ser_PutString(output);
}

/*---------------------------------------------------------------------------------------------------
 * Calibration Menu Routines 
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Name: DisplayCalMenu
 * Description: Prints the Calibration Menu to the serial port
 * Parameters: None 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void DisplayCalMenu()
{
    Ser_PutString("\r\nWelcome to the Arlobot calibration interface.\r\n");
    Ser_PutString("The following calibration operations are allowed:\r\n");
    Ser_PutString("    1. Motor Calibration - creates mapping between count/sec and PWM.\r\n");
    Ser_PutString("    2. PID Left Calibration - enter gains, execute step input, print velocity response.\r\n");
    Ser_PutString("    3. PID Right Calibration - enter gains, execute step input, print velocity response.\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("\r\nEnter X to exit calibration\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("Make an entry [1-3,X]: ");
}

/*---------------------------------------------------------------------------------------------------
 * Name: DisplayValMenu
 * Description: Prints the Validation Menu to the serial port
 * Parameters: None 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void DisplayValMenu()
{
    Ser_PutString("\r\nWelcome to the Arlobot validation interface.\r\n");
    Ser_PutString("The following validation operations are allowed:\r\n");
    Ser_PutString("    1. Motor Validation - operates the motors at varying velocities.\r\n");
    Ser_PutString("    2. PID Validation - operates the motors at varying velocities, moves in a straight line and rotates in place.\r\n");
    Ser_PutString("    3. Left PID (forward) Validation - operates the left motor in the forward direction at various velocities.\r\n");
    Ser_PutString("    4. Left PID (backward) Validation - operates the left motor in the backward direction at various velocities.\r\n");
    Ser_PutString("    5. Right PID (forward) Validation - operates the right motor in the forward direction at various velocities.\r\n");
    Ser_PutString("    6. Right PID (backward) Validation - operates the right motor in the backward direction at various velocities.\r\n");
    Ser_PutString("    7. Straight Line (forward) Validation - operates the robot in the forward direction at a constant (slow) velocity.\r\n");
    Ser_PutString("    8. Straight Line (backward) Validation - operates the robot in the backward direction at a constant (slow) velocity.\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("\r\nEnter X to exit validation\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("Make an entry [1-8,X]: ");
}

/*---------------------------------------------------------------------------------------------------
 * Name: DisplaySettingsMenu
 * Description: Prints the settings menu used to display the current calibration state.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void DisplaySettingsMenu()
{
    Ser_PutString("\r\nWelcome to the Arlobot settings display\r\n");
    Ser_PutString("The following settings can be displayed\r\n");
    Ser_PutString("    1. Left Motor Calibration\r\n");
    Ser_PutString("    2. Right Motor Calibration\r\n");
    Ser_PutString("    3. PID Gains: left pid and right pid\r\n");
    Ser_PutString("    4. Display All\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("\r\nEnter X to exit validation\r\n");
    Ser_PutString("\r\n");
    Ser_PutString("Make an entry [1-4,X]: ");
}

/*---------------------------------------------------------------------------------------------------
 * Name: DisplayExit
 * Description: Prints the exit menu.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void DisplayExit()
{
    Ser_PutString("\r\nExiting Arlobot calibration/validation interface.");
    Ser_PutString("\r\nType 'C' to enter calibration, 'V' to enter validation, 'D' to display settings\r\n");
}

/*---------------------------------------------------------------------------------------------------
 * Name: DisplayMenu
 * Description: Displays a menu, either calibration or validation, depending on the stage
 * Parameters: stage - the current calibration/validation stage 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void DisplayMenu(CAL_STAGE_TYPE stage)
{
    debug_control_enabled = 0;
    
    if (stage == CAL_CALIBRATE_STAGE)
    {
        DisplayCalMenu();
    }
    else if (stage == CAL_VALIDATE_STAGE)
    {
        DisplayValMenu();
    }

}

/*---------------------------------------------------------------------------------------------------
 * Name: ProcessSettingsCmd
 * Description: Processes the setting command request to display the requested calibration values
 * Parameters: cmd - the calibration/validation command requested
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void ProcessSettingsCmd(uint8 cmd)
{
    switch (cmd)
    {
        case MOTOR_LEFT_DISP_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nDisplaying left motor calibration: count/sec, pwm mapping");
            Ser_PutString("\r\n");
            Cal_PrintSamples("Left-Backward", (int32 *) p_cal_eeprom->left_motor_bwd.cps_data, (uint16 *) p_cal_eeprom->left_motor_bwd.pwm_data);
            Cal_PrintSamples("Left-Forward", (int32 *) p_cal_eeprom->left_motor_fwd.cps_data, (uint16 *) p_cal_eeprom->left_motor_fwd.pwm_data);
            Ser_PutString("\r\n");
            
            DisplaySettingsMenu();
            break;
            
        case MOTOR_RIGHT_DISP_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nDisplaying right motor calibration: count/sec, pwm mapping");
            Ser_PutString("\r\n");
            Cal_PrintSamples("Right-Backward", (int32 *) p_cal_eeprom->right_motor_bwd.cps_data, (uint16 *) p_cal_eeprom->right_motor_bwd.pwm_data);
            Cal_PrintSamples("Right-Forward", (int32 *) p_cal_eeprom->right_motor_fwd.cps_data, (uint16 *) p_cal_eeprom->right_motor_fwd.pwm_data);
            Ser_PutString("\r\n");
            
            DisplaySettingsMenu();
            break;
            
        case PID_DISP_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nDisplaying all PID gains: left, right");
            Ser_PutString("\r\n");
            Cal_PrintGains("Left PID", (float *) &p_cal_eeprom->left_gains);
            Cal_PrintGains("Right PID", (float *) &p_cal_eeprom->right_gains);

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

/*---------------------------------------------------------------------------------------------------
 * Name: GetCommand
 * Description: Gets the calibration/validation/display command from the serial port
 * Parameters: None
 * Return: the command entered on the serial port.
 * 
 *-------------------------------------------------------------------------------------------------*/
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
            
        case 'd':
        case 'D':
            value = SETTING_REQUEST;
            break;

        case 'x':
        case 'X':            
            value = EXIT_CMD;
            break;
    }
    
    return value;
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetCalibration
 * Description: Gets the calibration specified by command
 * Parameters: cmd - the requested calibration.
 * Return: the specified calibration type; otherwise, NULL.
 * 
 *-------------------------------------------------------------------------------------------------*/
static CALIBRATION_TYPE* GetCalibration(uint8 cmd)
{
    CAL_PID_PARAMS *p_pid_params;
    
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

        default:
            break;
    }
    
    return (CALIBRATION_TYPE *) 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetValidation
 * Description: Gets the validation specified by command
 * Parameters: cmd - the requested validation.
 * Return: the specified validation type; otherwise, NULL.
 * 
 *-------------------------------------------------------------------------------------------------*/
static CALIBRATION_TYPE* GetValidation(uint8 cmd)
{
    switch (cmd)
    {
        case MOTOR_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Motor Validation by operating the motors at varying velocities.\r\n");
            CalMotor_Validation->stage = CAL_VALIDATE_STAGE;
            CalMotor_Validation->state = CAL_INIT_STATE;
            return (CALIBRATION_TYPE *) CalMotor_Validation;
            break;
            
        case PID_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Left/Right PID Validation by operating the motors at varying velocities.\r\n");
            CalPid_LeftValidation->stage = CAL_VALIDATE_STAGE;
            CalPid_LeftValidation->state = CAL_INIT_STATE;
            return (CALIBRATION_TYPE *) CalPid_LeftValidation;
            break;
                    
        case PID_LEFT_FWD_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Left PID validation in the forward direction.\r\n");
            CalPid_LeftValidation->stage = CAL_VALIDATE_STAGE;
            CalPid_LeftValidation->state = CAL_INIT_STATE;
            ((CAL_PID_PARAMS *)(CalPid_LeftValidation->params))->direction = DIR_FORWARD;
            return (CALIBRATION_TYPE *) CalPid_LeftValidation;
            break;

        case PID_LEFT_BWD_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Left PID validation in the backward direction.\r\n");
            CalPid_LeftValidation->stage = CAL_VALIDATE_STAGE;
            CalPid_LeftValidation->state = CAL_INIT_STATE;
            ((CAL_PID_PARAMS *)(CalPid_LeftValidation->params))->direction = DIR_BACKWARD;
            return (CALIBRATION_TYPE *) CalPid_LeftValidation;
            break;

        case PID_RIGHT_FWD_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Right PID validation in the forward direction.\r\n");
            CalPid_RightValidation->stage = CAL_VALIDATE_STAGE;
            CalPid_RightValidation->state = CAL_INIT_STATE;
            ((CAL_PID_PARAMS *)(CalPid_RightValidation->params))->direction = DIR_FORWARD;
            return (CALIBRATION_TYPE *) CalPid_RightValidation;
            break;

        case PID_RIGHT_BWD_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming Right PID validation in the backward direction.\r\n");
            CalPid_RightValidation->stage = CAL_VALIDATE_STAGE;
            CalPid_RightValidation->state = CAL_INIT_STATE;
            ((CAL_PID_PARAMS *)(CalPid_RightValidation->params))->direction = DIR_BACKWARD;
            return (CALIBRATION_TYPE *) CalPid_RightValidation;
            break;
                       
        case LINEAR_FWD_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming straight line validation in the forward direction.\r\n");
            CalLin_Validation->stage = CAL_VALIDATE_STAGE;
            CalLin_Validation->state = CAL_INIT_STATE;
            ((CAL_LIN_PARAMS *)(CalLin_Validation->params))->direction = DIR_FORWARD;
            return (CALIBRATION_TYPE *) CalLin_Validation;
            break;
                
        case LINEAR_BWD_VAL_CMD:
            Ser_WriteByte(cmd);
            Ser_PutString("\r\nPerforming straight line validation in the backward direction.\r\n");
            CalLin_Validation->stage = CAL_VALIDATE_STAGE;
            CalLin_Validation->state = CAL_INIT_STATE;
            ((CAL_LIN_PARAMS *)(CalLin_Validation->params))->direction = DIR_BACKWARD;
            return (CALIBRATION_TYPE *) CalLin_Validation;
            break;

        default:            
            break;

    }
    
    return (CALIBRATION_TYPE *) 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: HandleError
 * Description: Prints an error message to the serial for the specified calibration/validation
 * Parameters: calval - the requested calibration/validation.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void HandleError(CALIBRATION_TYPE *calval)
{
    char outbuf[64];
    
    /* Maybe more can go here, but for now, just print an error */
    
    sprintf(outbuf, "Error processing stage %s, state %s\r\n", CAL_STAGE_TO_STRING(calval->stage), CAL_STATE_TO_STRING(calval->state));
    Ser_PutString(outbuf);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Process
 * Description: The main routine for processing a calibration/validation.  Each calibration/validation 
 *              module provides a common function interface: init, start, update, stop, results. 
 * Parameters: calval - the requested calibration/validation.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void Process(CALIBRATION_TYPE *calval)
{
    uint8 result;
    
    switch (calval->state)
    {
        case CAL_INIT_STATE:
            result = calval->init(calval->stage, calval->params);
            if (result == CAL_OK)
            {
                calval->state = CAL_START_STATE;
            }
            else
            {
                HandleError(calval);
            }
            break;
            
        case CAL_START_STATE:
            result = calval->start(calval->stage, calval->params);
            if (result == CAL_OK)
            {
                calval->state = CAL_RUNNING_STATE;
            }
            else
            {
                HandleError(calval);
            }
            break;
            
        case CAL_RUNNING_STATE:
            result = calval->update(calval->stage, calval->params);
            if (result == CAL_COMPLETE)
            {
                calval->state = CAL_STOP_STATE;
            }
            else if (result == CAL_OK)
            {
                /* Figure out how to output a progress indication ... that will work for all calibration/validation */
            }
            else
            {
                HandleError(calval);
            }
            break;
            
        case CAL_STOP_STATE:
            result = calval->stop(calval->stage, calval->params);
            if (result == CAL_OK)
            {
                calval->state = CAL_RESULTS_STATE;
            }
            else
            {
                HandleError(calval);
            }
            break;
            
        case CAL_RESULTS_STATE:
            result = calval->results(calval->stage, calval->params);
            if (result == CAL_OK)
            {
                calval->state = CAL_DONE_STATE;                
            }
            else
            {
                HandleError(calval);
            }
            break;
            
        case CAL_DONE_STATE:
        default:
            active_cal = (CALIBRATION_TYPE *) 0;
            active_val = (CALIBRATION_TYPE *) 0;
            DisplayMenu(calval->stage);
            break;
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: CpsToPwm
 * Description: Converts count/sec to PWM 
 *              This routine searches the count/sec array (cps_data) to find values immediately less 
 *              than and greater than the specified count/sec value (cps) to obtain the corresponding
 *              indicies - upper/lower.  The indicies are then used to interpolate a PWM value.
 *  
 * Parameters: cps       - the specified count/sec
 *             cps_data  - an array of count/sec values to be searched
 *             pwm_data  - an array of pwm values for selection
 *             data_size - the number of values in each array
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint16 CpsToPwm(int32 cps, int32 *cps_data, uint16 *pwm_data, uint8 data_size)
{   
    PWM_TYPE pwm = PWM_STOP;
    uint8 lower = 0;
    uint8 upper = 0;

    if (cps > 0 || cps < 0)
    {
        BinaryRangeSearch(cps, cps_data, data_size, &lower, &upper);
        
        pwm = Interpolate(cps, cps_data[lower], cps_data[upper], pwm_data[lower], pwm_data[upper]);

        return constrain(pwm, MIN_PWM_VALUE, MAX_PWM_VALUE);
    }

    return pwm;
}

/*----------------------------------------------------------------------------------------------------------------------
 * Module Interface Routines
 *---------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_Init
 * Description: Initializes the calibration module and submodules.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_Init()
{
    p_cal_eeprom = NVSTORE_CAL_EEPROM_BASE;
    Cal_LeftTarget = LeftTarget;
    Cal_RightTarget = RightTarget;
    
    CalMotor_Init();
    CalPid_Init();
    CalLin_Init();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_Start
 * Description: Reads the current calibration status from EEPROM and sets it into the I2C module. 
 *              Note: This can only be done after the non-volatile memory module has been initialized. 
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_Start()
{
    //Nvstore_WriteBytes((uint8 *) &left_fwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_motor_fwd));
    //Nvstore_WriteBytes((uint8 *) &left_bwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_motor_bwd));
    //Nvstore_WriteBytes((uint8 *) &right_fwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_motor_fwd));
    //Nvstore_WriteBytes((uint8 *) &right_bwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_motor_bwd));
    //Nvstore_WriteUint16(0, 0x000f);
    //Nvstore_WriteFloat(1.07, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->linear_bias));
    //Nvstore_WriteFloat(1.0625, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->angular_bias));

    uint16 status = p_cal_eeprom->status;
    I2c_SetCalibrationStatus(status);        
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_Update
 * Description: Called from the main loop to process calibration/validation. 
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
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
            If command is 'd'|'D', then enter STATE_SETTINGS
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
                Control_OverrideDebug(TRUE);
                Process(active_cal);
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
                Control_OverrideDebug(TRUE);
                Process(active_val);
            }
            
            if (cmd == EXIT_CMD)
            {
                ui_state = UI_STATE_EXIT;
            }
            break;
            
        case UI_STATE_SETTINGS:
            ProcessSettingsCmd(cmd);
            
            if (cmd == EXIT_CMD)
            {
                ui_state = UI_STATE_EXIT;
            }
            break;

        case UI_STATE_EXIT:
            DisplayExit();
            Control_OverrideDebug(FALSE);
            ui_state = UI_STATE_INIT;
            break;
            
        default:
            ui_state = UI_STATE_INIT;                
            break;
    }
    
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_ReadResponse
 * Description: Read a floating point value from the serial port. 
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_LeftGetPidGains/Cal_RightGetPidGains
 * Description: Returns the left/right PID gains from EEPROM. 
 * Parameters: None
 * Return: pointer to CAL_PID_TYPE
 * 
 *-------------------------------------------------------------------------------------------------*/
CAL_PID_TYPE * Cal_LeftGetPidGains()
{
    return (CAL_PID_TYPE *) &p_cal_eeprom->left_gains;
}

CAL_PID_TYPE * Cal_RightGetPidGains()
{
    return (CAL_PID_TYPE *) &p_cal_eeprom->right_gains;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_SetLeftRightVelocity
 * Description: Sets the left/right velocity for calibration/validation.  This routine is called from 
 *              calibration submodules to set wheel speed. 
 * Parameters: left - the left wheel speed in meter/second 
 *             right - the right wheel speed in meter/second 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_SetLeftRightVelocity(float left, float right)
{
    left_cmd_velocity = left;
    right_cmd_velocity = right;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_CpsToPwm
 * Description: Public routine used to obtain a PWM value for a given wheel and count/sec 
 * Parameters: wheel - left/right wheel 
 *             cps - count/second
 * Return: PWM_TYPE - PWM
 * 
 *-------------------------------------------------------------------------------------------------*/
PWM_TYPE Cal_CpsToPwm(WHEEL_TYPE wheel, float cps)
{
    static uint8 send_once = 0;
    PWM_TYPE pwm;
    
    
    pwm = PWM_STOP;
    
    /* The conversion from CPS to PWM is valid only when calibration has been performed */
    
    if (p_cal_eeprom->status & CAL_MOTOR_BIT)
    {
    
        CAL_DATA_TYPE *p_cal_data = WHEEL_DIR_TO_CAL_DATA[wheel][cps > 0 ? 0 : 1];
        
        cps = constrain((int32)(cps * p_cal_data->cps_scale), p_cal_data->cps_min, p_cal_data->cps_max);
        pwm = CpsToPwm(cps, &p_cal_data->cps_data[0], &p_cal_data->pwm_data[0], CAL_DATA_SIZE);
    }
    else
    {
        if (!send_once)
        {
            Ser_PutString("Motor calibration status not set\r\n");
            send_once = 1;
        }
    }

    return pwm;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_Clear
 * Description: Clears the motor and PID calibration bits. 
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_Clear()
{
    ClearCalibrationStatusBit(CAL_MOTOR_BIT);
    ClearCalibrationStatusBit(CAL_PID_BIT);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_CalcTriangularProfile
 * Description: Calculates an array of values using a triangle profile based on a calibrated motor 
 *              values. 
 * Parameters: num_points - the number of points in the profile (must be odd) 
 *             lower_limit - the start/end value as a percentage of the maximum motor value
 *             upper_limit - the maximum value as a percentage of the maximum motor value
 *             forward_profile - profile for the forward direction
 *             backward_profile - profile for the backward direction
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_CalcTriangularProfile(uint8 num_points, float lower_limit, float upper_limit, float *forward_profile, float *backward_profile)
/* This routine calculates a series of count/second values in a triangle profile (slow, fast, slow).  It uses the motor
   calibration data to determine a range  of forward and reverse values for each wheel.  The routine is called from
   motor validation to confirm that motor calibration conversion from count/second to pwm is reasonably accurate.
 */
{
    uint8 ii;

    memset(forward_profile, 0, num_points * sizeof(float));
    memset(backward_profile, 0, num_points * sizeof(float));

    if( num_points % 2 == 0 )
    {
        return;
    }
    
    if (p_cal_eeprom->status & CAL_MOTOR_BIT)
    {    
        float left_forward_cps_max = p_cal_eeprom->left_motor_fwd.cps_max / p_cal_eeprom->left_motor_fwd.cps_scale;
        float right_forward_cps_max = p_cal_eeprom->right_motor_fwd.cps_max / p_cal_eeprom->right_motor_fwd.cps_scale;
        float left_backward_cps_min = p_cal_eeprom->left_motor_bwd.cps_min / p_cal_eeprom->left_motor_bwd.cps_scale;
        float right_backward_cps_min = p_cal_eeprom->right_motor_bwd.cps_min / p_cal_eeprom->right_motor_bwd.cps_scale;

        float forward_cps_max = min(left_forward_cps_max, right_forward_cps_max);
        /* Note: Backward values are negative */
        float backward_cps_max = max(left_backward_cps_min, right_backward_cps_min);
        
        float fwd_cps_start = lower_limit * forward_cps_max;
        float fwd_cps_end = upper_limit * forward_cps_max;
        float bwd_cps_start = lower_limit * backward_cps_max;
        float bwd_cps_end = upper_limit * backward_cps_max;

        uint8 mid_sample_offset = num_points / 2;
        
        float fwd_cps_delta = (fwd_cps_end - fwd_cps_start)/mid_sample_offset;
        float bwd_cps_delta = (bwd_cps_end - bwd_cps_start)/mid_sample_offset;

        
        forward_profile[mid_sample_offset] = fwd_cps_end;
        backward_profile[mid_sample_offset] = bwd_cps_end;
        
        float fwd_value = fwd_cps_start;
        float bwd_value = bwd_cps_start;
        for (ii = 0; ii < mid_sample_offset; ++ii)
        {
            forward_profile[ii] = fwd_value;
            backward_profile[ii] = bwd_value;
            
            fwd_value += fwd_cps_delta;
            bwd_value += bwd_cps_delta;
        }
        
        fwd_value = fwd_cps_end;
        bwd_value = bwd_cps_end;
        for (ii = mid_sample_offset; ii < num_points; ++ii)
        {
            forward_profile[ii] = fwd_value;
            backward_profile[ii] = bwd_value;

            fwd_value -= fwd_cps_delta;
            bwd_value -= bwd_cps_delta;
        }
    }
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
