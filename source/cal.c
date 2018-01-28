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
#include <stdlib.h>
#include "cal.h"
#include "motor.h"
#include "pwm.h"
#include "encoder.h"
#include "control.h"
#include "utils.h"
#include "conserial.h"
#include "pid.h"
#include "pidleft.h"
#include "pidright.h"
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


/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/
#define LEFT_PID_KP_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_gains.kp)
#define LEFT_PID_KI_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_gains.ki)
#define LEFT_PID_KD_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_gains.kd)
#define LEFT_PID_KF_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_gains.kf)

#define RIGHT_PID_KP_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_gains.kp)
#define RIGHT_PID_KI_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_gains.ki)
#define RIGHT_PID_KD_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_gains.kd)
#define RIGHT_PID_KF_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_gains.kf)

#define STATUS_OFFSET NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->status)

#define ANGULAR_BIAS_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->angular_bias)
#define LINEAR_BIAS_OFFSET (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->linear_bias)

#define MOTOR_DATA_OFFSET(wheel, dir) (UINT16) NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(WHEEL_DIR_TO_CAL_DATA[wheel][dir])


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/

static FLOAT left_cmd_velocity;
static FLOAT right_cmd_velocity;

static volatile CAL_EEPROM_TYPE * p_cal_eeprom;

static volatile CAL_DATA_TYPE * WHEEL_DIR_TO_CAL_DATA[2][2];

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
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
static PWM_TYPE CpsToPwm(INT16 cps, INT16 *cps_data, UINT16 *pwm_data, UINT8 data_size)
{   
    INT16 pwm = (INT16) PWM_STOP;
    UINT8 lower = 0;
    UINT8 upper = 0;

    REQUIRE(in_range(cps, -MAX_WHEEL_COUNT_PER_SECOND, MAX_WHEEL_COUNT_PER_SECOND));
    REQUIRE(cps_data != NULL);
    REQUIRE(pwm_data != NULL);
    REQUIRE(data_size == CAL_DATA_SIZE);

    if (cps > 0 || cps < 0)
    {
        BinaryRangeSearch(cps, cps_data, data_size, &lower, &upper);
        
        pwm = Interpolate( (INT32) cps, 
                           (INT32) cps_data[lower], 
                           (INT32) cps_data[upper], 
                           (INT32) pwm_data[lower], 
                           (INT32) pwm_data[upper]);

    }

    ConSer_WriteLine(TRUE, "CpsToPwm: %d -> %u", cps, pwm);

    ENSURE(in_range(pwm, MIN_PWM_VALUE, MAX_PWM_VALUE));

    return (PWM_TYPE) pwm;
}

/*---------------------------------------------------------------------------------------------------
 * Calibration Print Routines 
 *  
 * Routines used by other calibration modules for printing values to the serial port 
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintAllMotorParams
 * Description: Prints the left/right, forward/backward count/sec and pwm calibration values
 * Parameters: as_json - if TRUE, output in json format; otherwise, plain text.
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/

void Cal_PrintLeftMotorParams(BOOL as_json)
{
    Cal_PrintSamples(WHEEL_LEFT, DIR_BACKWARD, (CONST_CAL_DATA_CONST_PTR_TYPE) WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_BACKWARD], as_json);
    Cal_PrintSamples(WHEEL_LEFT, DIR_FORWARD, (CONST_CAL_DATA_CONST_PTR_TYPE) WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_FORWARD], as_json);
}

void Cal_PrintRightMotorParams(BOOL as_json)
{
    Cal_PrintSamples(WHEEL_RIGHT, DIR_BACKWARD, (CONST_CAL_DATA_CONST_PTR_TYPE) WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_BACKWARD], as_json);
    Cal_PrintSamples(WHEEL_RIGHT, DIR_FORWARD, (CONST_CAL_DATA_CONST_PTR_TYPE) WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_FORWARD], as_json);
}


 void Cal_PrintAllMotorParams(BOOL as_json)
{
    Cal_PrintLeftMotorParams(as_json);
    Cal_PrintRightMotorParams(as_json);
}

void Cal_PrintMotorParams(WHEEL_TYPE wheel, BOOL as_json)
{        
    if (wheel == WHEEL_LEFT)
    {
        Cal_PrintLeftMotorParams(as_json);
    }
    else if (wheel == WHEEL_RIGHT)
    {
        Cal_PrintRightMotorParams(as_json);
    }
    else // WHEEL_BOTH
    {
        Cal_PrintAllMotorParams(as_json);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintSamples
 * Description: Prints the count/sec and pwm samples.  Called from the CalMotor module.
 * Parameters: wheel - either left or right
 *             dir - either forward or backward 
 *             cal_data - pointer to structure containing motor calibration data
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_PrintSamples(WHEEL_TYPE wheel, DIR_TYPE dir, CONST_CAL_DATA_CONST_PTR_TYPE cal_data, UINT8 as_json)
{
    UINT16 ii;

    REQUIRE(cal_data != NULL);
    REQUIRE(wheel == WHEEL_LEFT || wheel == WHEEL_RIGHT);
    REQUIRE(dir == DIR_FORWARD || dir == DIR_BACKWARD);

    if (as_json)
    {
        /* Example json
            {"wheel":"left", 
            "direction":"backward", 
            "min":-4328, 
            "max":0, 
            "values":[
                {"cps":-4328,"pwm":1000},
                {"cps":-4328,"pwm":1000},
                {"cps":-4318,"pwm":1010},
                ...
                {"cps":-50,"pwm":1470},
                {"cps":0,"pwm":1480},
                {"cps":0,"pwm":1490},
                {"cps":0,"pwm":1500}
            ]}
        */

        ConSer_WriteLine(FALSE, "{\"wheel\":\"%s\",", wheel == WHEEL_LEFT ? "left" : "right");
        ConSer_WriteLine(FALSE, "\"direction\":\"%s\",", dir == DIR_FORWARD ? "forward" : "backward");
        ConSer_WriteLine(FALSE, "\"min\":%d,", cal_data->cps_min);
        ConSer_WriteLine(FALSE, "\"max\":%d,", cal_data->cps_max);
        ConSer_WriteLine(FALSE, "\"values\":[");
        for (ii = 0; ii < CAL_NUM_SAMPLES - 1; ++ii)
        {
            ConSer_WriteLine(FALSE, "{\"cps\":%d,\"pwm\":%d},", cal_data->cps_data[ii], cal_data->pwm_data[ii]);
        }
        ConSer_WriteLine(TRUE, "{\"cps\":%d,\"pwm\":%d}]}\r\n", cal_data->cps_data[ii], cal_data->pwm_data[ii]);
    }
    else
    {
        ConSer_WriteLine(TRUE, "%s-%s - min/max: %d/%d", 
                            wheel == WHEEL_LEFT ? "Left" : "Right", 
                            dir == DIR_FORWARD ? "Forward" : "Backward", 
                            cal_data->cps_min, 
                            cal_data->cps_max);
        
        for (ii = 0; ii < CAL_NUM_SAMPLES - 1; ++ii)
        {
            ConSer_WriteLine(TRUE, "%ld:%d ", cal_data->cps_data[ii], cal_data->pwm_data[ii]);
        }
        ConSer_WriteLine(TRUE, "%ld:%d", cal_data->cps_data[ii], cal_data->pwm_data[ii]);
        ConSer_WriteLine(TRUE, "");
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintPidGains
 * Description: Prints the PID gains.  Called from the CalPid and Cal modules.
 * Parameters: wheel - string containing the pid identifier, e.g., left pid or right pid. 
 *             gains - an array of FLOAT values corresponding to Kp, Ki, and Kd. 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_PrintPidGains(WHEEL_TYPE wheel, FLOAT const * const gains, UINT8 as_json)
{
    REQUIRE(wheel == WHEEL_LEFT || wheel == WHEEL_RIGHT);
    REQUIRE(gains != NULL);

    if (as_json)
    {
        /*
            {"wheel":"left", "p":%.3f,"i":%.3f,"d":%.3f,"f":%.3f}
        */
        ConSer_WriteLine(TRUE, "{\"wheel\":\"%s\",\"p\":%.3f,\"i\":%.3f,\"d\":%.3f,\"f\":%.3f}",
                            wheel == WHEEL_LEFT ? "left" : "right", 
                            gains[0], gains[1], gains[2], gains[3]);
    }
    else
    {
        ConSer_WriteLine(TRUE, "%s PID - P: %.3f, I: %.3f, D: %.3f, F: %.3f", 
                            wheel == WHEEL_LEFT ? "Left" : "Right", 
                            gains[0], gains[1], gains[2], gains[3]);
    }
}

void Cal_PrintLeftPidGains(BOOL as_json)
{
    CAL_PID_PTR_TYPE p_pid;
    FLOAT gains[4];
    
    p_pid = Cal_GetPidGains(PID_TYPE_LEFT);
    gains[0] = p_pid->kp;
    gains[1] = p_pid->ki;
    gains[2] = p_pid->kd;
    gains[3] = p_pid->kf;
    Cal_PrintPidGains(WHEEL_LEFT, gains, as_json);
}

void Cal_PrintRightPidGains(BOOL as_json)
{
    CAL_PID_PTR_TYPE p_pid;
    FLOAT gains[4];

    p_pid = Cal_GetPidGains(PID_TYPE_RIGHT);
    gains[0] = p_pid->kp;
    gains[1] = p_pid->ki;
    gains[2] = p_pid->kd;
    gains[3] = p_pid->kf;
    Cal_PrintPidGains(WHEEL_RIGHT, gains, as_json);            
}

void Cal_PrintAllPidGains(BOOL as_json)
{
    Cal_PrintLeftPidGains(as_json);
    Cal_PrintRightPidGains(as_json);
}

void Cal_PrintStatus(UINT8 as_json)
{
    if (as_json)
    {
        /* TODO: Consider parsing out the bits of status into fields in the json */
        ConSer_WriteLine(TRUE, "{\"status\":%02x}", p_cal_eeprom->status);
    }
    else
    {
        ConSer_WriteLine(TRUE, "Status - %02x", p_cal_eeprom->status);
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_PrintBias
 * Description: Prints the linear/angular bias.
 * Parameters: None 
 * Return: None
 *-------------------------------------------------------------------------------------------------*/
void Cal_PrintBias(UINT8 as_json)
{
    if (as_json)
    {
        ConSer_WriteLine(TRUE, "{\"linear\":%.2f,\"angular\":%.2f}", Cal_GetLinearBias(), Cal_GetAngularBias());
    }
    else
    {
        ConSer_WriteLine(TRUE, "Linear Bias: %.2f, Angular Bias: %.2f", Cal_GetLinearBias(), Cal_GetAngularBias());
    }
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
static FLOAT LeftTarget()
{
    return left_cmd_velocity;
}
 
static FLOAT RightTarget()
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
void Cal_ClearCalibrationStatusBit(UINT16 bit)
{
    REQUIRE(is_power_of_two(bit));

    UINT16 status = p_cal_eeprom->status &= ~bit;
    Nvstore_WriteUint16(status, STATUS_OFFSET);
    Control_ClearCalibrationStatusBit(bit);
}
 
void Cal_SetCalibrationStatusBit(UINT16 bit)
{
    REQUIRE(is_power_of_two(bit));

    UINT16 status = p_cal_eeprom->status | bit;
    Nvstore_WriteUint16(status, STATUS_OFFSET);
    Control_SetCalibrationStatusBit(bit);   
}
 
UINT16 Cal_GetCalibrationStatusBit(UINT16 bit)
{
    REQUIRE(is_power_of_two(bit));
    return p_cal_eeprom->status & bit;
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
    UINT16 status = p_cal_eeprom->status;
    /* Uncomment for debugging
    Cal_Clear();
    */
    Control_SetCalibrationStatus(status);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_ReadResponse
 * Description: Reads a line of text from the the serial port.
 *              NOTE: This a blocking call and should only be used it is known not to impact other
 *              module updates.   The serial module update is kicked to keep the connection alive.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
INT16 Cal_ReadResponse(CHAR * const digits, BOOL echo, UINT8 max_length)
{
    INT16 result;

    REQUIRE(digits != NULL);

    do
    {
        ConSer_Update();
    
        result = ConSer_ReadLine(digits, echo, max_length);
        if (result >= 0)
        {
            break;
        }
    } while (result < 1);

    ENSURE(in_range(result, -1, MAX_STRING_LENGTH));
    
    return result;
}

FLOAT Cal_ReadResponseFloat()
{
    CHAR digits[10] = {0};
    INT16 result;

    result = Cal_ReadResponse(digits, TRUE, 10);
    if (result > 0)
    {
        return atof((char *) digits);
    }

    return 0.0;
}

UINT8 Cal_ReadResponseReturn()
{
    CHAR digits[10] = {0};
    INT16 result;

    result = Cal_ReadResponse(digits, TRUE, 10);
    if (result == 0)
    {
        return 1;
    }

    return 0;
}

CHAR Cal_ReadResponseChar()
{
    CHAR chars[2] = {'0'};
    INT16 result;

    result = Cal_ReadResponse(chars, TRUE, 10);
    if (result > 0)
    {
        return chars[0];
    }
    
    return '\n';
}

FLOAT Cal_ReadResponseWithDefault(FLOAT dflt)
{
    CHAR digits[10] = {0};
    INT16 result;

    result = Cal_ReadResponse(digits, TRUE, 10);
    if (result > 0)
    {
        return atof((char *) digits);
    }
    
    return dflt;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_GetPidGains
 * Description: Returns the specified PID gains from EEPROM. 
 * Parameters: None
 * Return: pointer to CAL_PID_TYPE
 * 
 *-------------------------------------------------------------------------------------------------*/
CAL_PID_PTR_TYPE Cal_GetPidGains(PID_ENUM_TYPE pid)
{
    switch (pid)
    {
        case PID_TYPE_LEFT:
            return (CAL_PID_PTR_TYPE) &p_cal_eeprom->left_gains;

        case PID_TYPE_RIGHT:
            return (CAL_PID_PTR_TYPE) &p_cal_eeprom->right_gains;        

        case PID_TYPE_LINEAR:
            return (CAL_PID_PTR_TYPE) &p_cal_eeprom->linear_gains;        

        case PID_TYPE_ANGULAR:
            return (CAL_PID_PTR_TYPE) &p_cal_eeprom->angular_gains;
            
        default:
            return (CAL_PID_PTR_TYPE) NULL;
    }
    
    return (CAL_PID_PTR_TYPE) NULL;
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
void Cal_SetLeftRightVelocity(FLOAT left, FLOAT right)
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
PWM_TYPE Cal_CpsToPwm(WHEEL_TYPE wheel, FLOAT cps)
{
    static UINT8 send_once = 0;
    PWM_TYPE pwm;
    CONST_CAL_DATA_PTR_TYPE p_cal_data;

    REQUIRE(wheel == WHEEL_LEFT || wheel == WHEEL_RIGHT);
    REQUIRE(in_range_float(cps, -MAX_WHEEL_COUNT_PER_SECOND, MAX_WHEEL_COUNT_PER_SECOND));

    pwm = PWM_STOP;
    
    /* The conversion from CPS to PWM is valid only when calibration has been performed */
    
    if (Cal_GetCalibrationStatusBit(CAL_MOTOR_BIT))
    {    
        p_cal_data = (CONST_CAL_DATA_PTR_TYPE) WHEEL_DIR_TO_CAL_DATA[wheel][cps >= 0 ? DIR_FORWARD : DIR_BACKWARD];
        //REQUIRE(in_range_float(cps, p_cal_data->cps_min, p_cal_data->cps_max));    
        cps = constrain((INT16) cps, p_cal_data->cps_min, p_cal_data->cps_max);
        pwm = CpsToPwm((INT16) cps, (INT16 *) &p_cal_data->cps_data[0], (UINT16 *) &p_cal_data->pwm_data[0], CAL_DATA_SIZE);
    }
    else
    {
        if (!send_once)
        {
            ConSer_WriteLine(FALSE, "Motor calibration status not set");
            send_once = 1;
        }
    }
    
    ENSURE(in_range(pwm, MIN_PWM_VALUE, MAX_PWM_VALUE));

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

void Cal_CalcForwardOperatingRange(FLOAT low_percent, FLOAT high_percent, FLOAT* const start, FLOAT* const stop)
{        
    FLOAT tmp_start;
    FLOAT tmp_stop;
    INT16 left_forward_cps_max;
    INT16 right_forward_cps_max;
    INT16 forward_cps_max;

    REQUIRE(start != NULL);
    REQUIRE(stop != NULL);
    REQUIRE(in_range_float(low_percent, 0.0, 1.0));
    REQUIRE(in_range_float(high_percent, 0.0, 1.0));
    REQUIRE(low_percent < high_percent);

    /* Get the min/max forward values for each motor */
    left_forward_cps_max = WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_FORWARD]->cps_max;
    right_forward_cps_max = WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_FORWARD]->cps_max;

    /* Select the max of the max */
    forward_cps_max = min(left_forward_cps_max, right_forward_cps_max);

    tmp_start = low_percent * (FLOAT) forward_cps_max;
    tmp_stop = high_percent * (FLOAT) forward_cps_max;

    *start = tmp_start;
    *stop = tmp_stop;

    ENSURE(in_range_float(*start, 0, forward_cps_max));
    ENSURE(in_range_float(*stop, 0, forward_cps_max));
}

void Cal_CalcBackwardOperatingRange(FLOAT low_percent, FLOAT high_percent, FLOAT* const start, FLOAT* const stop)
{
    FLOAT tmp_start;
    FLOAT tmp_stop;
    INT16 left_backward_cps_max;
    INT16 right_backward_cps_max;
    INT16 backward_cps_max;

    REQUIRE(start != NULL);
    REQUIRE(stop != NULL);
    REQUIRE(in_range_float(low_percent, 0.0, 1.0));
    REQUIRE(in_range_float(high_percent, 0.0, 1.0));
    REQUIRE(low_percent < high_percent);

    /* Get the min/max forward values for each motor */
    left_backward_cps_max = WHEEL_DIR_TO_CAL_DATA[WHEEL_LEFT][DIR_BACKWARD]->cps_min;
    right_backward_cps_max = WHEEL_DIR_TO_CAL_DATA[WHEEL_RIGHT][DIR_BACKWARD]->cps_min;

    /* Select the min of the max */
    backward_cps_max = min(left_backward_cps_max, right_backward_cps_max);

    tmp_start = low_percent * (FLOAT) backward_cps_max;
    tmp_stop = high_percent * (FLOAT) backward_cps_max;

    *start = tmp_start;
    *stop = tmp_stop;

    ENSURE(in_range_float(*start, backward_cps_max, 0));
    ENSURE(in_range_float(*stop, backward_cps_max, 0));
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
void Cal_CalcTriangularProfile(UINT8 num_points, FLOAT lower_limit, FLOAT upper_limit, FLOAT* const forward_profile, FLOAT* const backward_profile)
/* This routine calculates a series of count/second values in a triangle profile (slow, fast, slow).  It uses the motor
   calibration data to determine a range of forward and reverse values for each wheel.  The routine is called from
   motor validation to confirm that motor calibration conversion from count/second to pwm is reasonably accurate.
 */
{
    FLOAT start;
    FLOAT stop;
    int ii;

    REQUIRE(num_points % 2 != 0);
    REQUIRE(in_range(lower_limit, 0.0, 1.0));
    REQUIRE(in_range(upper_limit, 0.0, 1.0));
    REQUIRE(forward_profile != NULL);
    REQUIRE(backward_profile != NULL);

    for (ii = 0; ii < num_points; ++ii)
    {
        forward_profile[ii] = 0.0;
        backward_profile[ii] = 0.0;
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
        ConSer_WriteLine(TRUE, "");
        ConSer_WriteLine(TRUE, "Motor calibration not done");
    }
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_GetLinearBias/Cal_GetAngularBias
 * Description: Returns the linear/angular bias value either default or calibrated.  Includes constaint
 *              checking to ensure the value is in the expected range.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
FLOAT Cal_GetLinearBias()
{
    FLOAT linear_bias;
    
    /* Set the default to 1.0, in case, linear bias has not been calibrated
     */
    
    linear_bias = CAL_LINEAR_BIAS_DEFAULT;
    if (CAL_LINEAR_BIT & p_cal_eeprom->status)
    {        
        linear_bias = constrain(p_cal_eeprom->linear_bias, CAL_LINEAR_BIAS_MIN, CAL_LINEAR_BIAS_MAX);
    }
    return linear_bias;
}

FLOAT Cal_GetAngularBias()
{
    FLOAT angular_bias;
    
    angular_bias = CAL_ANGULAR_BIAS_DEFAULT;
    if (CAL_ANGULAR_BIT & p_cal_eeprom->status)
    {
        angular_bias = constrain(p_cal_eeprom->angular_bias, CAL_ANGULAR_BIAS_MIN, CAL_ANGULAR_BIAS_MAX);
    }
    return angular_bias;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_SetAngularBias/Cal_SetLinearBias
 * Description: Sets the linear/angular bias value either default or calibrated.  Includes constaint
 *              checking to ensure the value is in the expected range.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Cal_SetLinearBias(FLOAT bias)
{
    REQUIRE(in_range_float(bias, CAL_LINEAR_BIAS_MIN, CAL_LINEAR_BIAS_MAX));
    bias = constrain(bias, CAL_LINEAR_BIAS_MIN, CAL_LINEAR_BIAS_MAX);
    Nvstore_WriteFloat(bias, LINEAR_BIAS_OFFSET);
}

void Cal_SetAngularBias(FLOAT bias)
{
    REQUIRE(in_range_float(bias, CAL_ANGULAR_BIAS_MIN, CAL_ANGULAR_BIAS_MAX));
    bias = constrain(bias, CAL_ANGULAR_BIAS_MIN, CAL_ANGULAR_BIAS_MAX);
    Nvstore_WriteFloat(bias, ANGULAR_BIAS_OFFSET);
}


UINT16 Cal_GetStatus()
{
    return p_cal_eeprom->status;
}

void Cal_SetGains(PID_ENUM_TYPE pid, FLOAT* const gains)
{
    REQUIRE(pid == PID_TYPE_LEFT || pid == PID_TYPE_RIGHT);
    REQUIRE(gains != NULL);

    switch (pid)
    {
        case PID_TYPE_LEFT:
            Nvstore_WriteFloat(gains[0], LEFT_PID_KP_OFFSET);
            Nvstore_WriteFloat(gains[1], LEFT_PID_KI_OFFSET);
            Nvstore_WriteFloat(gains[2], LEFT_PID_KD_OFFSET);
            Nvstore_WriteFloat(gains[3], LEFT_PID_KF_OFFSET);
            break;

        case PID_TYPE_RIGHT:
            Nvstore_WriteFloat(gains[0], RIGHT_PID_KP_OFFSET);
            Nvstore_WriteFloat(gains[1], RIGHT_PID_KI_OFFSET);
            Nvstore_WriteFloat(gains[2], RIGHT_PID_KD_OFFSET);
            Nvstore_WriteFloat(gains[3], RIGHT_PID_KF_OFFSET);
            break;
            
        case PID_TYPE_LINEAR:
        case PID_TYPE_ANGULAR:
        default:
            break;
    }
}

void Cal_SetMotorData(WHEEL_TYPE wheel, DIR_TYPE dir, CAL_DATA_TYPE *data)
{
    REQUIRE(wheel == WHEEL_LEFT || wheel == WHEEL_RIGHT);
    REQUIRE(data != NULL);

    /* Write the calibration to non-volatile storage */
    Nvstore_WriteBytes((UINT8 *) data, sizeof(*data), MOTOR_DATA_OFFSET(wheel, dir));
}

CONST_CAL_DATA_CONST_PTR_TYPE Cal_GetMotorData(WHEEL_TYPE wheel, DIR_TYPE dir)
{
    REQUIRE(wheel == WHEEL_LEFT || wheel == WHEEL_RIGHT);
    REQUIRE(dir == DIR_FORWARD || dir == DIR_BACKWARD);
    return (CONST_CAL_DATA_CONST_PTR_TYPE) WHEEL_DIR_TO_CAL_DATA[wheel][dir];
}

/*---------------------------------------------------------------------------------------------------
 * Name: Cal_CalMaxCps
 * Description: Calculates the minimum of the maximum CPS values between the left/right motors. 
 * Parameters: None
 * Return: FLOAT - cps (count/second)
 * 
 *-------------------------------------------------------------------------------------------------*/
FLOAT Cal_CalcMaxCps()
/* The step input velocity is 80% of maximum wheel velocity.
   Maximum wheel velocity is determined considering two factors:
        1. the maximum left/right PID value (derived from the theoretical maximum)
        2. the maximum calibrated velocity of each wheel
   The minimum value of the above is the basis for determining the step input velocity.
 */
{
    INT16 left_fwd_max;
    INT16 left_bwd_max;
    INT16 right_fwd_max;
    INT16 right_bwd_max;
    INT16 left_max;
    INT16 right_max;
    INT16 max_leftright_cps;
    INT16 max_leftright_pid;

    left_fwd_max = abs(Cal_GetMotorData(WHEEL_LEFT, DIR_FORWARD)->cps_max);
    left_bwd_max = abs(Cal_GetMotorData(WHEEL_LEFT, DIR_BACKWARD)->cps_min);
    right_fwd_max = abs(Cal_GetMotorData(WHEEL_RIGHT, DIR_FORWARD)->cps_max);
    right_bwd_max = abs(Cal_GetMotorData(WHEEL_RIGHT, DIR_BACKWARD)->cps_min);
    
    left_max = min(left_fwd_max, left_bwd_max);
    right_max = min(right_fwd_max, right_bwd_max);
    
    max_leftright_cps = min(left_max, right_max);
    max_leftright_pid = min(LEFTPID_MAX, RIGHTPID_MAX);

    return min(max_leftright_cps, max_leftright_pid);
}

void Cal_PrintParams(BOOL as_json)
{
    REQUIRE(as_json == FALSE);

    ConSer_WriteLine(TRUE, "----------- Physical Characteristics -----------");
    ConSer_WriteLine(TRUE, "Track Width         : %.4f meter", TRACK_WIDTH);
    ConSer_WriteLine(TRUE, "Wheel Radius        : %.4f meter", WHEEL_RADIUS);
    ConSer_WriteLine(TRUE, "Wheel Diameter      : %.4f meter", WHEEL_DIAMETER);
    ConSer_WriteLine(TRUE, "Wheel Circumference : %.4f meter", WHEEL_CIRCUMFERENCE);
    ConSer_WriteLine(TRUE, "Wheel Max Rotation  : %2.0f RPM", MAX_WHEEL_RPM);
    ConSer_WriteLine(TRUE, "Wheel Encoder Tick  : %d tick/rev", WHEEL_ENCODER_TICK_PER_REV);
    ConSer_WriteLine(TRUE, "Wheel Encoder Count : %d count/rev", WHEEL_COUNT_PER_REV);
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "------------ Wheel Rates -----------");
    ConSer_WriteLine(TRUE, "Wheel (meter/count)   : %.4f", WHEEL_METER_PER_COUNT);
    ConSer_WriteLine(TRUE, "Wheel (radian/count)  : %.4f", WHEEL_RADIAN_PER_COUNT);            
    ConSer_WriteLine(TRUE, "Wheel (radian/second) : %.4f", MAX_WHEEL_RADIAN_PER_SECOND);
    ConSer_WriteLine(TRUE, "Wheel (count/meter)   : %.4f", WHEEL_COUNT_PER_METER);
    ConSer_WriteLine(TRUE, "Wheel (count/radian)  : %.4f", WHEEL_COUNT_PER_RADIAN);
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "---------------------------- Wheel Maxes ---------------------------");
    ConSer_WriteLine(TRUE, "Wheel Forward Max  : %.4f meter/second", MAX_WHEEL_FORWARD_LINEAR_VELOCITY);
    ConSer_WriteLine(TRUE, "Wheel Forward Max  : %.4f count/second", MAX_WHEEL_FORWARD_COUNT_PER_SEC);
    ConSer_WriteLine(TRUE, "Wheel Backward Max : %.4f meter/second", MAX_WHEEL_BACKWARD_LINEAR_VELOCITY);
    ConSer_WriteLine(TRUE, "Wheel Backward Max : %.4f count/second", MAX_WHEEL_BACKWARD_COUNT_PER_SEC);
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "Wheel CW Max       : %.4f radian/second", MAX_WHEEL_CW_ANGULAR_VELOCITY);
    ConSer_WriteLine(TRUE, "Wheel CW Max       : %.4f count/second", MAX_WHEEL_CW_COUNT_PER_SEC);
    ConSer_WriteLine(TRUE, "Wheel CCW Max      : %.4f radian/second", MAX_WHEEL_CCW_ANGULAR_VELOCITY);
    ConSer_WriteLine(TRUE, "Wheel CCW Max      : %.4f count/second", MAX_WHEEL_CCW_COUNT_PER_SEC);
    ConSer_WriteLine(TRUE, "");
    ConSer_WriteLine(TRUE, "---------------------------- Robot Max ---------------------------");
    ConSer_WriteLine(TRUE, "Robot Max Rotation : %.4f RPM", MAX_ROBOT_RPM);
    ConSer_WriteLine(TRUE, "Robot Rotation     : %.4f meter/rev", ROBOT_METER_PER_REV);
    ConSer_WriteLine(TRUE, "Robot Rotation     : %.4f count/rev", ROBOT_COUNT_PER_REV);
    ConSer_WriteLine(TRUE, "Robot Forward Max  : %.4f meter/second", MAX_WHEEL_FORWARD_LINEAR_VELOCITY);
    ConSer_WriteLine(TRUE, "Robot Backward Max : %.4f meter/second", MAX_WHEEL_BACKWARD_LINEAR_VELOCITY);
    ConSer_WriteLine(TRUE, "Robot CW Max       : %.4f radian/second", MAX_ROBOT_CW_RADIAN_PER_SECOND);
    ConSer_WriteLine(TRUE, "Robot CCW Max      : %.4f radian/second", MAX_ROBOT_CCW_RADIAN_PER_SECOND);
}

/*-------------------------------------------------------------------------------*/
/* [] END OF FILE */
