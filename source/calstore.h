#ifndef CALSTORE_H
#define CALSTORE_H

#include <project.h>
    
#define CAL_NUM_SAMPLES (51)
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
    // Note: Total size is 320 bytes, at 16 bytes per row, 20 rows
} __attribute__ ((packed)) CAL_DATA_TYPE;

typedef struct _cal_pid_tag
{
    float kp;
    float ki;
    float kd;
    // Added to force row alignment
    uint8 reversed[4];
    // Note: Total size is 16 bytes, 1 row
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
    float  linear_bias;
    float  angular_bias;
    // Added to force row alignment
    uint8 reserved[4];
    CAL_PID_TYPE left_gains;
    CAL_PID_TYPE right_gains;
    CAL_PID_TYPE linear_gains;
    CAL_PID_TYPE angular_gains;
    CAL_DATA_TYPE left_motor_fwd;
    CAL_DATA_TYPE left_motor_bwd;
    CAL_DATA_TYPE right_motor_fwd;
    CAL_DATA_TYPE right_motor_bwd;
} __attribute__ ((packed)) CAL_EEPROM_TYPE;
  
    
#endif