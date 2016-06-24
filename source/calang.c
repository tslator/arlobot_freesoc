#include <math.h>
#include "calang.h"
#include "odom.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "time.h"
#include "utils.h"
#include "serial.h"
#include "nvstore.h"

#define TEST_ANGULAR_VEL (0.7)
#define ANGULAR_TEST_HEADING (2*PI)

static float test_left_vel;
static float test_right_vel;

void PerformAngularBiasCalibration(uint8 verbose)
/*
    Set angular bias to 1.0
    Rotate 360 degrees
        Get the current heading
        Stop motors
        Start turn angle = 0
        Test angle = 360
        while turn angle < test angle

            Set angular velocity
            Get heading
            Calculate delta angle
            Update turn and last angles

        Stop motors

 */
{
    float heading;
    float turn_heading;
    float delta_heading;
    float last_heading;
    
    // Reset angular bias
    Nvstore_WriteFloat(1.0, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->angular_bias));    
    ConvertLinearAngularToDifferential(0, TEST_ANGULAR_VEL, &test_left_vel, &test_right_vel);            
    
    turn_heading = 0;
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    last_heading = Odom_GetHeading();
    
    do
    {
        Encoder_Update();
        Pid_Update();
        Odom_Update();
        
        heading = Odom_GetHeading();
        delta_heading = heading - last_heading;
        if (delta_heading > PI) 
        { 
            delta_heading -= 2*PI; 
        } 
        else if (delta_heading <= -PI) 
        { 
            delta_heading += 2*PI; 
        }

        turn_heading += delta_heading;
        last_heading = heading;
    
        Motor_LeftSetMeterPerSec(test_left_vel);
        Motor_RightSetMeterPerSec(test_right_vel);
        
    } while (abs(turn_heading) < ANGULAR_TEST_HEADING);
    
    Motor_LeftSetCntsPerSec(0);
    Motor_RightSetCntsPerSec(0);
    
    /* Pend on reading the linear bias (or timeout 60 seconds) */
    float angular_bias;
    uint32 wait_time = millis();
    while (millis() - wait_time < 60000)
    {
        if (Ser_IsDataReady())
        {
            Ser_ReadFloat(&angular_bias);
            
            /* Convert angular bias from degrees (because it is easier to estimate) to radian. */
            angular_bias = DEGREES_TO_RADIANS(angular_bias);
            
            /* Store the linear bias into EEPROM */
            Nvstore_WriteFloat(angular_bias, NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->angular_bias));
        }
    }
}
