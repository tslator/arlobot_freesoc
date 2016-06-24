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

#define LINEAR_BIAS_DISTANCE (1.0)
#define LINEAR_BIAS_VELOCITY (0.150)
#define LINEAR_BIAS_TOLERANCE (0.01)

static float dist_error;

void PerformLinearBiasCalibration(uint8 verbose)
/* 
    Set Linear Bias value to 1.0
    Move forward for 1 meter
        Get current position
        Stop motors
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
    float x_start;
    float y_start;
    float x_pos;
    float y_pos;
    dist_error = LINEAR_BIAS_DISTANCE;
    
    // Reset linear bias
    Nvstore_WriteFloat(1.0, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->linear_bias));
    
    Odom_GetPosition(&x_start, &y_start);
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    
    do
    {
        Encoder_Update();
        Pid_Update();
        Odom_Update();
        
        Odom_GetPosition(&x_pos, &y_pos);
        float distance = sqrt(pow((x_pos - x_start), 2) +
                              pow((y_pos - y_start), 2));

        dist_error = distance - LINEAR_BIAS_DISTANCE;
        Motor_LeftSetMeterPerSec(LINEAR_BIAS_VELOCITY);
        Motor_RightSetMeterPerSec(LINEAR_BIAS_VELOCITY);
    } while (abs(dist_error) > LINEAR_BIAS_TOLERANCE);
    
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    
    /* Pend on reading the linear bias (or timeout 60 seconds) */
    float linear_bias;
    uint32 wait_time = millis();
    while (millis() - wait_time < 60000)
    {
        if (Ser_IsDataReady())
        {
            Ser_ReadFloat(&linear_bias);
            
            /* Store the linear bias into EEPROM */
            Nvstore_WriteFloat(linear_bias, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->linear_bias));
        }
    }
    
}
