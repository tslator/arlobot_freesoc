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

#ifndef CAL_H
#define CAL_H

#include <project.h>
    
/* The calibration module supports the following calibrations:
    
    1. Motor - creates a mapping between count/sec velocity and motor pwm value.  The resulting four tables,
       left-forward, left-reverse, right-forward, right-reverse are stored in EEPROM and read into memory on startup.
    2. PID Gains - executes step input motion to determine the PID gains.  Uses the TWIDDLE algorithm, reference:
       https://www.youtube.com/watch?v=2uQ2BSzDvXs, to minimize the steady-state error.  There is a PID for each wheel.
       Calibration is run for each wheel.  The resulting gains are stored in EEPROM and read into memory on startup.
    3. Linear Bias - moves 1 meter forward, stops and waits to receive the actual linear distance (in meters), calculates
       the linear bias and stores in EEPROM.  The linear bias is applied to the delta distance calculation in odometry.
    4. Angular Bias - rotates 360 degrees, stops and waits to receive the actual rotation (in degrees), calculates the 
       angular bias and stores in EEPROM.  The angular bias is applied to the delta heading calculation in odometry.
 */
    
/* Calibration control/status bits */
#define CAL_COUNT_PER_SEC_TO_PWM_BIT    (0x0001)
#define CAL_PID_BIT                     (0x0002)
#define CAL_LINEAR_BIAS_BIT             (0x0004)
#define CAL_ANGULAR_BIAS_BIT            (0x0008)
#define CAL_VERBOSE_BIT                 (0x0080)
    
#define CAL_NUM_SAMPLES (51)
#define CAL_SCALE_FACTOR (100)
    
#define CAL_DATA_SIZE (CAL_NUM_SAMPLES)

typedef struct _CAL_DATA_TYPE
{
    int32  cps_min;
    int32  cps_max;
    int    cps_scale;
    int32  cps_data[CAL_DATA_SIZE];
    uint16 pwm_data[CAL_DATA_SIZE];
    // Added to force row alignment
    uint16 reserved; 
} __attribute__ ((packed)) CAL_DATA_TYPE;

typedef struct _cal_pid_tag
{
    float kp;
    float ki;
    float kd;
} __attribute__ ((packed)) CAL_PID_TYPE;

typedef struct _eeprom_tag
{
    // the following fields are padded to 16 bytes (1 row)
    uint16 status;                  /* bit 0: Motor (Count/Sec to PWM)
                                       bit 1: PID
                                       bit 2: Linear Bias
                                       bit 3: Angular Bias
                                     */
    uint16 checksum;
    uint8 reserved[12];
    // The following fields take up 32 bytes (2 rows) and are aligned with the EEPROM row size
    //   For example:
    //      sizeof(PID_TYPE) = 12, sizeof(float) = 4
    //      12 + 12 + 4 + 4 = 32 bytes (2 16-byte rows)
    CAL_PID_TYPE left_gains;
    CAL_PID_TYPE right_gains;
    float  linear_bias;
    float  angular_bias;
    CAL_DATA_TYPE left_motor_fwd;
    CAL_DATA_TYPE left_motor_bwd;
    CAL_DATA_TYPE right_motor_fwd;
    CAL_DATA_TYPE right_motor_bwd;
} __attribute__ ((packed)) CAL_EEPROM_TYPE;

volatile CAL_EEPROM_TYPE *p_cal_eeprom;

void Cal_Init();
void Cal_Start();

void Cal_LeftGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data);
void Cal_RightGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data);
CAL_PID_TYPE* Cal_LeftGetPidGains();
CAL_PID_TYPE* Cal_RightGetPidGains();

void Cal_CheckRequest();

#endif

/* [] END OF FILE */
