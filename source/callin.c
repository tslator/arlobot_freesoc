#include <math.h>
#include "callin.h"
#include "odom.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "time.h"
#include "utils.h"
#include "serial.h"
#include "nvstore.h"

static float dist_error;

void PerformLinearBiasCalibration(uint8 verbose)
/* 
    Set Linear Bias value to 1.0
    Move forward for 1 meter
        Get current position
        Stop motor
        while distance error > theshold
            Get current position
            Calculate distance
            Calculate distance error
            Move motor at linear bias velocity
        Stop motor
    Wait for Linear Bias offset
    Wait for command
        if START the goto START
        if STOP continue
    Update linear bias value in EEPROM
    Set linear bias calibration status bit
    Update linear bias calibration status bit in EEPROM

 */
{
    float linear_bias = 1.0;
    float x_start;
    float y_start;
    float x_pos;
    float y_pos;
    #define LINEAR_BIAS_DISTANCE (1.0)
    #define LINEAR_BIAS_VELOCITY (0.150)
    #define LINEAR_BIAS_TOLERANCE (0.01)
    #define LIN_SAMPLE_TIME_MS  SAMPLE_TIME_MS(40)
    dist_error = LINEAR_BIAS_DISTANCE;
    uint32 start_time;
    
    Odom_GetPosition(&x_start, &y_start);
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    start_time = millis();
    
    do
    {
        Encoder_Update();
        Pid_Update();
        Odom_Update();
        
        if (millis() - start_time > LIN_SAMPLE_TIME_MS)
        {
            Odom_GetPosition(&x_pos, &y_pos);
            float distance = sqrt(pow((x_pos - x_start), 2) +
                                  pow((y_pos - y_start), 2));
            distance *= linear_bias;
            dist_error = distance - LINEAR_BIAS_DISTANCE;
            Motor_LeftSetMeterPerSec(LINEAR_BIAS_VELOCITY);
            Motor_RightSetMeterPerSec(LINEAR_BIAS_VELOCITY);
        }
    } while (abs(dist_error) > LINEAR_BIAS_TOLERANCE);
    
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    
    /* Pend on reading the linear bias (or timeout) */
    Ser_ReadFloat(&linear_bias);
    
    /* Store the linear bias into EEPROM */
    Nvstore_WriteFloat(linear_bias, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->linear_bias));
}
