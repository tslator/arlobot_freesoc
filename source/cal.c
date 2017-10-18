/* 
MIT License

Copyright (c) 2017 Tim Slator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*---------------------------------------------------------------------------------------------------
   Description: This module provides the implementation for calibration.
 *-------------------------------------------------------------------------------------------------*/    


/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "cal.h"
#include "motor.h"
#include "pwm.h"
#include "encoder.h"
#include "utils.h"
#include "serial.h"
#include "pid.h"
#include "nvstore.h"
#include "calmotor.h"
#include "valmotor.h"
#include "calpid.h"
#include "valpid.h"
#include "callin.h"
#include "vallin.h"
#include "calang.h"
#include "valang.h"
#include "debug.h"
#include "control.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/

/* Calibration interface commands */
#define NO_CMD              '0'
#define CAL_REQUEST         1
#define VAL_REQUEST         2
#define SETTING_REQUEST     3
#define EXIT_CMD            255
#define NULL_CMD            0
#define CAL_REQUEST_CHR      'c'
#define CAL_REQUEST_UCHR     'C'
#define VAL_REQUEST_CHR      'v'
#define VAL_REQUEST_UCHR     'V'
#define SETTING_REQUEST_CHR  'd'
#define SETTING_REQUEST_UCHR 'D'
#define EXIT_CMD_CHR         'x'
#define EXIT_CMD_UCHR        'X'

#define CAL_MOTOR_CMD       1
#define CAL_PID_LEFT_CMD    2
#define CAL_PID_RIGHT_CMD   3
#define CAL_LINEAR_CMD      4
#define CAL_ANGULAR_CMD     5
#define CAL_FIRST_CMD       CAL_MOTOR_CMD
#define CAL_LAST_CMD        CAL_ANGULAR_CMD

#define VAL_MOTOR_CMD           1
#define VAL_PID_CMD             2
#define VAL_PID_LEFT_FWD_CMD    3
#define VAL_PID_LEFT_BWD_CMD    4
#define VAL_PID_RIGHT_FWD_CMD   5
#define VAL_PID_RIGHT_BWD_CMD   6
#define VAL_LINEAR_FWD_CMD      7
#define VAL_LINEAR_BWD_CMD      8
#define VAL_ANGULAR_CW_CMD      9
#define VAL_ANGULAR_CCW_CMD     10
#define VAL_FIRST_CMD           VAL_MOTOR_CMD
#define VAL_LAST_CMD            VAL_ANGULAR_CCW_CMD

#define DISP_MOTOR_LEFT_CMD  1
#define DISP_MOTOR_RIGHT_CMD 2
#define DISP_PID_CMD         3
#define DISP_BIAS_CMD        4
#define DISP_ALL_CMD         5
#define DISP_FIRST_CMD       DISP_MOTOR_LEFT_CMD
#define DISP_LAST_CMD        DISP_ALL_CMD

#define CMD_TOP_LEVEL (0)
#define CMD_CALIBRATION (1)
#define CMD_VALIDATION (2)
#define CMD_SETTINGS (3)

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

static CALVAL_INTERFACE_TYPE *active_calval;

static float left_cmd_velocity;
static float right_cmd_velocity;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------
 * Calibration Print Routines 
 *  
 * Routines used by other calibration modules for printing values to the serial port 
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintAllMotorParams
 * Description: Prints the left/right, forward/backward count/sec and pwm calibration values
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 void Cal_PrintAllMotorParams()
{
    Cal_PrintSamples("Left-Backward", (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_bwd);
    Cal_PrintSamples("Left-Forward", (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_fwd);
    Cal_PrintSamples("Right-Backward", (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_bwd);
    Cal_PrintSamples("Right-Forward", (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_fwd);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintSamples
 * Description: Prints the count/sec and pwm samples.  Called from the CalMotor module.
 * Parameters: label - string containing the motor identifier, e.g., left or right 
 *             cal_data - pointer to structure containing motor calibration data
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_PrintSamples(char *label, CAL_DATA_TYPE *cal_data)
{
    uint8 ii;
    
    Ser_PutStringFormat("%s - min/max: %d/%d\r\n", label, cal_data->cps_min, cal_data->cps_max);
    
    for (ii = 0; ii < CAL_NUM_SAMPLES - 1; ++ii)
    {
        Ser_PutStringFormat("%ld:%d ", cal_data->cps_data[ii], cal_data->pwm_data[ii]);
    }
    Ser_PutStringFormat("%ld:%d\r\n\r\n", cal_data->cps_data[ii], cal_data->pwm_data[ii]);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintGains
 * Description: Prints the PID gains.  Called from the CalPid and Cal modules.
 * Parameters: label - string containing the pid identifier, e.g., left pid or right pid. 
 *             gains - an array of float values corresponding to Kp, Ki, and Kd. 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_PrintGains(char *label, float *gains)
{
    Ser_PutStringFormat("%s - P: %.3f, I: %.3f, D: %.3f, F: %.3f\r\n", label, gains[0], gains[1], gains[2], gains[3]);
}

void Cal_PrintStatus(char *label, uint16 status)
{
    Ser_PutStringFormat("%s - %02x\r\n", label, status);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintBias
 * Description: Prints the linear/angular bias.
 * Parameters: None 
 * Return: None
 *-------------------------------------------------------------------------------------------------*/
void Cal_PrintBias()
{
    Ser_PutStringFormat("Linear Bias: %.2f, Angular Bias: %.2f\r\n", Cal_GetLinearBias(), Cal_GetAngularBias());
}

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
  * Name: ClearCalibrationStatusBit/SetCalibrationStatusBit/GetCalibrationStatusBit
  * Description: Clears/Sets the specified bit in the calibration status register and the EEPROM 
  *              status field.
  * Parameters: bit - the bit number to be Cleared/Set - 0 .. 15.
  * Return: None
  * 
  *-------------------------------------------------------------------------------------------------*/
 void Cal_ClearCalibrationStatusBit(uint16 bit)
 {
     uint16 status = p_cal_eeprom->status &= ~bit;
     Nvstore_WriteUint16(status, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->status));
     Control_ClearCalibrationStatusBit(bit);
 }
 
 void Cal_SetCalibrationStatusBit(uint16 bit)
 {
     uint16 status = p_cal_eeprom->status | bit;
     Nvstore_WriteUint16(status, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->status));
     Control_SetCalibrationStatusBit(bit);   
 }
 
 uint16 Cal_GetCalibrationStatusBit(uint16 bit)
 {
     return p_cal_eeprom->status & bit;
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
    Ser_PutStringFormat("    %d. Motor Calibration - creates mapping between count/sec and PWM.\r\n",  CAL_MOTOR_CMD);
    Ser_PutStringFormat("    %d. PID Left Calibration - enter gains, execute step input, print velocity response.\r\n", CAL_PID_LEFT_CMD);
    Ser_PutStringFormat("    %d. PID Right Calibration - enter gains, execute step input, print velocity response.\r\n", CAL_PID_RIGHT_CMD);
    Ser_PutStringFormat("    %d. Linear Calibration - move 1 meter forward, measure and enter actual distance.\r\n", CAL_LINEAR_CMD);
    Ser_PutStringFormat("    %d. Angular Calibration - rotate 360 degrees clockwise, measure and enter actual rotation.\r\n", CAL_ANGULAR_CMD);
    Ser_PutString("\r\n");
    Ser_PutStringFormat("\r\nEnter %c/%c to exit calibration\r\n", EXIT_CMD_CHR, EXIT_CMD_UCHR);
    Ser_PutString("\r\n");
    Ser_PutStringFormat("Make an entry [%d-%d,%c/%c]: ", CAL_FIRST_CMD, CAL_LAST_CMD, EXIT_CMD_CHR, EXIT_CMD_UCHR);
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
    Ser_PutStringFormat("    %d. Motor Validation - operates the motors at varying velocities.\r\n", VAL_MOTOR_CMD);
    Ser_PutStringFormat("    %d. PID Validation - operates the motors at varying velocities, moves in a straight line and rotates in place.\r\n",VAL_PID_CMD);
    Ser_PutStringFormat("    %d. Left PID (forward) Validation - operates the left motor in the forward direction at various velocities.\r\n", VAL_PID_LEFT_FWD_CMD);
    Ser_PutStringFormat("    %d. Left PID (backward) Validation - operates the left motor in the backward direction at various velocities.\r\n", VAL_PID_LEFT_BWD_CMD);
    Ser_PutStringFormat("    %d. Right PID (forward) Validation - operates the right motor in the forward direction at various velocities.\r\n", VAL_PID_RIGHT_FWD_CMD);
    Ser_PutStringFormat("    %d. Right PID (backward) Validation - operates the right motor in the backward direction at various velocities.\r\n", VAL_PID_RIGHT_BWD_CMD);
    Ser_PutStringFormat("    %d. Straight Line (forward) Validation - operates the robot in the forward direction at a constant (slow) velocity.\r\n", VAL_LINEAR_FWD_CMD);
    Ser_PutStringFormat("    %d. Straight Line (backward) Validation - operates the robot in the backward direction at a constant (slow) velocity.\r\n", VAL_LINEAR_BWD_CMD);
    Ser_PutStringFormat("    %d. Rotation (cw) Validation - rotates the robot in place in the clockwise direction at a constant (slow) velocity.\r\n", VAL_ANGULAR_CW_CMD);
    Ser_PutStringFormat("    %d. Rotation (ccw) Validation - rotates the robot in place in the counter clockwise direction at a constant (slow) velocity.\r\n", VAL_ANGULAR_CCW_CMD);
    Ser_PutStringFormat("\r\n\r\nEnter %c/%c to exit validation\r\n", EXIT_CMD_CHR, EXIT_CMD_UCHR);
    Ser_PutStringFormat("\r\nMake an entry [%d-%d,%c/%c]: ", VAL_FIRST_CMD, VAL_LAST_CMD, EXIT_CMD_CHR, EXIT_CMD_UCHR);
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
    Ser_PutStringFormat("    %d. Left Motor Calibration\r\n", DISP_MOTOR_LEFT_CMD);
    Ser_PutStringFormat("    %d. Right Motor Calibration\r\n", DISP_MOTOR_RIGHT_CMD);
    Ser_PutStringFormat("    %d. PID Gains: left pid and right pid\r\n", DISP_PID_CMD);
    Ser_PutStringFormat("    %d. Linear/Angular Bias: linear and angular biases\r\n", DISP_BIAS_CMD);
    Ser_PutStringFormat("    %d. Display All\r\n", DISP_ALL_CMD);
    Ser_PutStringFormat("\r\n\r\nEnter %c/%c to exit validation\r\n", EXIT_CMD_CHR, EXIT_CMD_UCHR);
    Ser_PutStringFormat("\r\nMake an entry [%d-%d,%c/%c]: ", DISP_FIRST_CMD, DISP_LAST_CMD, EXIT_CMD_CHR, EXIT_CMD_UCHR);
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
    Ser_PutStringFormat("\r\nType '%c/%c' to enter calibration, '%c/%c' to enter validation, '%c/%c' to display settings\r\n", 
                    CAL_REQUEST_CHR, CAL_REQUEST_UCHR,
                    VAL_REQUEST_CHR, VAL_REQUEST_UCHR,
                    SETTING_REQUEST_CHR, SETTING_REQUEST_UCHR);
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
    Debug_DisableAll();
    
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
        case DISP_MOTOR_LEFT_CMD:
            Ser_PutString("\r\nDisplaying left motor calibration: count/sec, pwm mapping\r\n");
            Cal_PrintSamples("Left-Backward", (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_bwd);
            Cal_PrintSamples("Left-Forward", (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_fwd);
            Ser_PutString("\r\n");
            
            DisplaySettingsMenu();
            break;
            
        case DISP_MOTOR_RIGHT_CMD:
            Ser_PutString("\r\nDisplaying right motor calibration: count/sec, pwm mapping\r\n");
            Cal_PrintSamples("Right-Backward", (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_bwd);
            Cal_PrintSamples("Right-Forward", (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_fwd);
            Ser_PutString("\r\n");
            
            DisplaySettingsMenu();
            break;
            
        case DISP_PID_CMD:
            Ser_PutString("\r\nDisplaying all PID gains: left, right\r\n");
            Cal_PrintGains("Left PID", (float *) &p_cal_eeprom->left_gains);
            Cal_PrintGains("Right PID", (float *) &p_cal_eeprom->right_gains);
            Ser_PutString("\r\n");

            DisplaySettingsMenu();
            break;
            
        case DISP_BIAS_CMD:
            Ser_PutString("\r\nDisplaying linear/angular bias\r\n");
            Cal_PrintBias();
            Ser_PutString("\r\n");

            DisplaySettingsMenu();
            break;
            
        case DISP_ALL_CMD:
            Ser_PutString("\r\nDisplaying all calibration/settings\r\n");

            Cal_PrintAllMotorParams();

            Cal_PrintGains("Left PID", (float *) &p_cal_eeprom->left_gains);
            Cal_PrintGains("Right PID", (float *) &p_cal_eeprom->right_gains);
            Cal_PrintStatus("Status", p_cal_eeprom->status);
            Cal_PrintBias();

            DisplaySettingsMenu();
            break;
            
        default:
            // Do nothing
            break;
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetTopLevelCommand
 * Description: Translates command characters to numeric command values
 * Parameters: cmd - top level command
 * Return: numeric command
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 GetTopLevelCommand(char* cmd)
{
    switch (cmd[0])
    {
        case CAL_REQUEST_CHR:
        case CAL_REQUEST_UCHR:
            return CAL_REQUEST;

        case VAL_REQUEST_CHR:
        case VAL_REQUEST_UCHR:
            return VAL_REQUEST;

        case SETTING_REQUEST_CHR:
        case SETTING_REQUEST_UCHR:
            return SETTING_REQUEST;

        case EXIT_CMD_CHR:
        case EXIT_CMD_UCHR:
            return EXIT_CMD;
    }

    return NULL_CMD;
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetCalibrationCommand
 * Description: Translates command characters to numeric command values
 * Parameters: cmd - calibration command
 * Return: numeric command
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 GetCalibrationCommand(char* cmd)
{
    uint8 num_cmd;

    if (cmd[0] == EXIT_CMD_CHR || cmd[0] == EXIT_CMD_UCHR)
    {
        return EXIT_CMD;
    }

    num_cmd = atoi(cmd);

    if (num_cmd >= CAL_FIRST_CMD && num_cmd <= CAL_LAST_CMD)
    {
        return num_cmd;
    }

    return NULL_CMD;
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetValidationCommand
 * Description: Translates command characters to numeric command values
 * Parameters: cmd - validation command
 * Return: numeric command
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 GetValidationCommand(char* cmd)
{
    uint8 num_cmd;

    if (cmd[0] == EXIT_CMD_CHR || cmd[0] == EXIT_CMD_UCHR)
    {
        return EXIT_CMD;
    }

    num_cmd = atoi(cmd);

    if (num_cmd >= VAL_FIRST_CMD && num_cmd <= VAL_LAST_CMD)
    {
        return num_cmd;
    }

    return NULL_CMD;
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetSettingsCommand
 * Description: Translates command characters to numeric command values
 * Parameters: cmd - settings command
 * Return: numeric command
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 GetSettingsCommand(char* cmd)
{
    uint8 num_cmd;

    if (cmd[0] == EXIT_CMD_CHR || cmd[0] == EXIT_CMD_UCHR)
    {
        return EXIT_CMD;
    }

    num_cmd = atoi(cmd);
    
    if (num_cmd >= DISP_FIRST_CMD && num_cmd <= DISP_LAST_CMD)
    {
        return num_cmd;
    }

    return NULL_CMD;
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetCommand
 * Description: Gets the calibration/validation/display command from the serial port
 * Parameters: cmd_class
 * Return: the command entered on the serial port.
 * 
 *-------------------------------------------------------------------------------------------------*/
static uint8 GetCommand(uint8 cmd_class)
{
    static char value[2] = {NULL_CMD, '\r'};
    int result;
    uint8 cmd = NULL_CMD;

    result = Ser_ReadLine(value, true);
    if (result != 0)
    {

        switch (cmd_class)
        {
            case CMD_TOP_LEVEL:
                cmd = GetTopLevelCommand(value);
                break;

            case CMD_CALIBRATION:
                cmd = GetCalibrationCommand(value);
                break;

            case CMD_VALIDATION:
                cmd = GetValidationCommand(value);
                break;

            case CMD_SETTINGS:
                cmd = GetSettingsCommand(value);
        }
    
        return cmd;
    }
    
    /* The value returned from the function drives the calibration state machine.  The only values allowed are legitimate
       commands or NULL_CMD (where NULL_CMD is ignored).
     */
    
    return NULL_CMD;
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetCalibration
 * Description: Gets the calibration specified by command
 * Parameters: cmd - the requested calibration.
 * Return: the specified calibration type; otherwise, NULL.
 * 
 *-------------------------------------------------------------------------------------------------*/
static CALVAL_INTERFACE_TYPE* GetCalibration(uint8 cmd)
{
    switch (cmd)
    {
        case CAL_MOTOR_CMD:
            return CalMotor_Start();
            
        case CAL_PID_LEFT_CMD:
            return CalPid_Start(WHEEL_LEFT);

        case CAL_PID_RIGHT_CMD:
            return CalPid_Start(WHEEL_RIGHT);
            
        case CAL_LINEAR_CMD:
            return CalLin_Start();
            
        case CAL_ANGULAR_CMD:
            return CalAng_Start();
            
        default:
            // Do nothing
            break;
    }
    
    return (CALVAL_INTERFACE_TYPE *) 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: GetValidation
 * Description: Gets the validation specified by command
 * Parameters: cmd - the requested validation.
 * Return: the specified validation type; otherwise, NULL.
 * 
 *-------------------------------------------------------------------------------------------------*/
static CALVAL_INTERFACE_TYPE* GetValidation(uint8 cmd)
{
    switch (cmd)
    {
        case VAL_MOTOR_CMD:
            Ser_PutString("\r\nPerforming Motor Validation by operating the motors at varying velocities.\r\n");
            return ValMotor_Start();
            
        case VAL_PID_LEFT_FWD_CMD:
            Ser_PutString("\r\nPerforming Left PID validation in the forward direction.\r\n");
            return ValPid_Start(VAL_PID_LEFT_FORWARD);

        case VAL_PID_LEFT_BWD_CMD:
            Ser_PutString("\r\nPerforming Left PID validation in the backward direction.\r\n");
            return ValPid_Start(VAL_PID_LEFT_BACKWARD);

        case VAL_PID_RIGHT_FWD_CMD:
            Ser_PutString("\r\nPerforming Right PID validation in the forward direction.\r\n");
            return ValPid_Start(VAL_PID_RIGHT_FORWARD);

        case VAL_PID_RIGHT_BWD_CMD:
            Ser_PutString("\r\nPerforming Right PID validation in the backward direction.\r\n");
            return ValPid_Start(VAL_PID_RIGHT_BACKWARD);
                       
        case VAL_LINEAR_FWD_CMD:
            Ser_PutString("\r\nPerforming straight line validation in the forward direction.\r\n");
            return ValLin_Start(DIR_FORWARD);
                
        case VAL_LINEAR_BWD_CMD:
            Ser_PutString("\r\nPerforming straight line validation in the backward direction.\r\n");
            return ValLin_Start(DIR_BACKWARD);
            
        case VAL_ANGULAR_CW_CMD:
            Ser_PutString("\r\nPerforming rotation validation in the clockwise direction.\r\n");
            return ValAng_Start(DIR_CW);

        case VAL_ANGULAR_CCW_CMD:
            Ser_PutString("\r\nPerforming rotation validation in the counter clockwise direction.\r\n");
            return ValAng_Start(DIR_CCW);

        default:
            //Ser_PutStringFormat("\r\nUnknown command: %c\r\n", cmd);
            // Do nothing
            break;

    }
    
    return (CALVAL_INTERFACE_TYPE *) 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: HandleError
 * Description: Prints an error message to the serial for the specified calibration/validation
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void HandleError()
{
    /* Maybe more can go here, but for now, just print an error */
    
    Ser_PutStringFormat("Error processing stage %s, state %s\r\n", CAL_STAGE_TO_STRING(active_calval->stage), CAL_STATE_TO_STRING(active_calval->state));
}

/*---------------------------------------------------------------------------------------------------
 * Name: Process
 * Description: The main routine for processing a calibration/validation.  Each calibration/validation 
 *              module provides a common function interface: init, start, update, stop, results. 
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void Process()
{
    uint8 result;
    
    switch (active_calval->state)
    {
        case CAL_INIT_STATE:
            result = active_calval->init(active_calval->stage, active_calval->params);
            if (result == CAL_OK)
            {
                active_calval->state = CAL_START_STATE;
            }
            else
            {
                HandleError();
            }
            break;
            
        case CAL_START_STATE:
            result = active_calval->start(active_calval->stage, active_calval->params);
            if (result == CAL_OK)
            {
                active_calval->state = CAL_RUNNING_STATE;
            }
            else
            {
                HandleError();
            }
            break;
            
        case CAL_RUNNING_STATE:
            result = active_calval->update(active_calval->stage, active_calval->params);
            if (result == CAL_COMPLETE)
            {
                active_calval->state = CAL_STOP_STATE;
            }
            else if (result == CAL_OK)
            {
                /* Figure out how to output a progress indication ... that will work for all calibration/validation */
            }
            else
            {
                HandleError();
            }
            break;
            
        case CAL_STOP_STATE:
            result =active_calval->stop(active_calval->stage, active_calval->params);
            if (result == CAL_OK)
            {
                active_calval->state = CAL_RESULTS_STATE;
            }
            else
            {
                HandleError();
            }
            break;
            
        case CAL_RESULTS_STATE:
            result = active_calval->results(active_calval->stage, active_calval->params);
            if (result == CAL_OK)
            {
                active_calval->state = CAL_DONE_STATE;                
            }
            else
            {
                HandleError();
            }
            break;
            
        case CAL_DONE_STATE:
        default:
            active_calval = (CALVAL_INTERFACE_TYPE *) 0;
            active_calval = (CALVAL_INTERFACE_TYPE *) 0;
            DisplayMenu(active_calval->stage);
            break;
    }
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
    
    /* Initialize the direction to calibration data constant.  The constant is used for motor calibration
       and validation so it is best to initialize it a common location.
    */
    WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_FORWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_fwd;
    WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_BACKWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->left_motor_bwd;
    WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_FORWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_fwd;
    WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_BACKWARD] = (CAL_DATA_TYPE *) &p_cal_eeprom->right_motor_bwd;
    
    Cal_LeftTarget = LeftTarget;
    Cal_RightTarget = RightTarget;
    
    CalMotor_Init();
    ValMotor_Init();
    CalPid_Init();
    ValPid_Init();
    CalLin_Init();
    CalAng_Init();
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
    uint16 status = p_cal_eeprom->status;
    /* Uncomment for debugging
    Cal_Clear();
    */
    Control_SetCalibrationStatus(status); 
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
    uint8 cmd;
    
    switch (ui_state)
    {
        case UI_STATE_INIT:
            {        
                cmd = GetCommand(CMD_TOP_LEVEL);
                
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
        case UI_STATE_VALIDATION:
            {
                uint8 cal_val = (ui_state == UI_STATE_CALIBRATION) ? CMD_CALIBRATION : CMD_VALIDATION;
                cmd = GetCommand(cal_val);

                if (!active_calval)
                {
                    active_calval = (ui_state == UI_STATE_CALIBRATION) ? GetCalibration(cmd) : GetValidation(cmd);
                }            
                    
                if (active_calval)
                {
                    Control_OverrideDebug(TRUE);
                    Process();
                }                        
                
                if (cmd == EXIT_CMD)
                {
                    ui_state = UI_STATE_EXIT;
                }
            }
            break;
        
        case UI_STATE_SETTINGS:
            cmd = GetCommand(CMD_SETTINGS);
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
#ifdef OLD_WAY
    char value;
    uint8 digits[6];
    uint8 index = 0;
    do
    {
        Ser_Update();
        
        value = Ser_ReadByte();
        /* Note: A delay was necessary here to get minicom on the BBB to read in values.  
           Its not clear why that is needed for Linux and not for windows
         */
        CyDelayUs(1000);
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
#else
    static char digits[10] = {'0'};
    int result;
    do
    {
        Ser_Update();
    
        result = Ser_ReadLine(digits, true);
        if (result > 0)
        {
            return atof((char *) digits);
        }
    } while (result == 0);
    
    return 0.0;
#endif
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
 * Name: Cal_LinearGetPidGains/Cal_AngularGetPidGains
 * Description: Returns the linear/angular PID gains from EEPROM. 
 * Parameters: None
 * Return: pointer to CAL_PID_TYPE
 * 
 *-------------------------------------------------------------------------------------------------*/
CAL_PID_TYPE * Cal_LinearGetPidGains()
{
    return (CAL_PID_TYPE *) &p_cal_eeprom->linear_gains;
}

CAL_PID_TYPE * Cal_AngularGetPidGains()
{
    return (CAL_PID_TYPE *) &p_cal_eeprom->angular_gains;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_SetLeftRightVelocity
 * Description: Sets the left/right velocity for calibration/validation.  This routine is called from 
 *              calibration submodules to set wheel speed. 
 * Parameters: left - the left wheel speed in count/second 
 *             right - the right wheel speed in count/second 
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
    
    if (Cal_GetCalibrationStatusBit(CAL_MOTOR_BIT))
    {
    
        CAL_DATA_TYPE *p_cal_data = WHEEL_DIR_TO_CAL_DATA[wheel][cps >= 0 ? 0 : 1];
        
        cps = constrain((int16) cps, p_cal_data->cps_min, p_cal_data->cps_max);
        pwm = CpsToPwm((int16) cps, &p_cal_data->cps_data[0], &p_cal_data->pwm_data[0], CAL_DATA_SIZE);
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
    Cal_ClearCalibrationStatusBit(CAL_MOTOR_BIT);
    Cal_ClearCalibrationStatusBit(CAL_PID_BIT);
}

void Cal_CalcForwardOperatingRange(float low_percent, float high_percent, float *start, float *stop)
{        
    float tmp_start;
    float tmp_stop;
    
    /* Get the min/max forward values for each motor */
    int16 left_forward_cps_max = p_cal_eeprom->left_motor_fwd.cps_max;
    int16 right_forward_cps_max = p_cal_eeprom->right_motor_fwd.cps_max;

    /* Select the max of the max */
    int16 forward_cps_max = min(left_forward_cps_max, right_forward_cps_max);

    tmp_start = low_percent * (float) forward_cps_max;
    tmp_stop = high_percent * (float) forward_cps_max;

    *start = tmp_start;
    *stop = tmp_stop;

}

void Cal_CalcBackwardOperatingRange(float low_percent, float high_percent, float *start, float *stop)
{
    float tmp_start;
    float tmp_stop;
    
    /* Get the min/max forward values for each motor */
    int16 left_backward_cps_max = p_cal_eeprom->left_motor_bwd.cps_max;
    int16 right_backward_cps_max = p_cal_eeprom->right_motor_bwd.cps_max;

    /* Select the min of the max */
    int16 backward_cps_max = max(left_backward_cps_max, right_backward_cps_max);

    tmp_start = low_percent * (float) backward_cps_max;
    tmp_stop = high_percent * (float) backward_cps_max;

    *start = tmp_start;
    *stop = tmp_stop;
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
   calibration data to determine a range of forward and reverse values for each wheel.  The routine is called from
   motor validation to confirm that motor calibration conversion from count/second to pwm is reasonably accurate.
 */
{
    float start;
    float stop;

    memset(forward_profile, 0, num_points * sizeof(float));
    memset(backward_profile, 0, num_points * sizeof(float));

    /* There must be an odd number of points */
    if( num_points % 2 == 0 )
    {
        return;
    }
    
    /* Only calculate the profile if the motors are calibrated */
    
    if (p_cal_eeprom->status & CAL_MOTOR_BIT)
    {    

        Cal_CalcForwardOperatingRange(lower_limit, upper_limit, &start, &stop);
        CalcTriangularProfile(num_points, start, stop, forward_profile);

        Cal_CalcBackwardOperatingRange(lower_limit, upper_limit, &start, &stop);
        CalcTriangularProfile(num_points, start, stop, backward_profile);
        
    }
    else
    {
        Ser_PutString("\r\nMotor calibration not done\r\n");
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_GetLinearBias
 * Description: Returns the linear bias value either default or calibrated.  Includes constaint
 *              checking to ensure the value is in the expected range.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
float Cal_GetLinearBias()
{
    float linear_bias;
    
    /* Set the default to 1.0, in case, linear bias has not been calibrated
     */
    
    linear_bias = CAL_LINEAR_BIAS_DEFAULT;
    if (CAL_LINEAR_BIT & p_cal_eeprom->status)
    {        
        linear_bias = constrain(p_cal_eeprom->linear_bias, CAL_LINEAR_BIAS_MIN, CAL_LINEAR_BIAS_MAX);
    }
    return linear_bias;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_GetAngularBias
 * Description: Returns the angular bias value either default or calibrated.  Includes constaint
 *              checking to ensure the value is in the expected range.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
float Cal_GetAngularBias()
{
    float angular_bias;
    
    angular_bias = CAL_ANGULAR_BIAS_DEFAULT;
    if (CAL_ANGULAR_BIT & p_cal_eeprom->status)
    {
        angular_bias = constrain(p_cal_eeprom->angular_bias, CAL_ANGULAR_BIAS_MIN, CAL_ANGULAR_BIAS_MAX);
    }
    return angular_bias;
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
