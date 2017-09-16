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
   Description: This module provides the definition of the structure for storing calibration values.
 *-------------------------------------------------------------------------------------------------*/    


#ifndef CALSTORE_H
#define CALSTORE_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <project.h>
    
/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
#define CAL_NUM_SAMPLES (51)
#define CAL_DATA_SIZE (CAL_NUM_SAMPLES)    
    
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
 

 typedef struct _CAL_DATA_TYPE
{
    int16  cps_min;
    int16  cps_max;
    int16  cps_data[CAL_DATA_SIZE];
    uint16 pwm_data[CAL_DATA_SIZE];
    // Note: Total size is 208 bytes, at 16 bytes per row, 13 rows
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
    /* bit 0: Left/Right Motor (Count/Sec to PWM) calibrated
       bit 1: Left/Right PID calibrated
       bit 2: Unicycle PIDs calibrated
       bit 3: Linear Bias calibrated
       bit 4: Angular Bias calibrated
    */    
    uint16 status;                  /*    0 */
    uint16 checksum;                /*    2 */
    uint8 reserved_4[4];            /*    4 */
    CAL_PID_TYPE left_gains;        /*    8 */
    CAL_PID_TYPE right_gains;       /*   24 */
    float linear_bias;              /*   40 */
    float angular_bias;             /*   44 */
    CAL_PID_TYPE linear_gains;      /*   48 */
    CAL_PID_TYPE angular_gains;     /*   64 */
    uint8 reversed[1136];           /*   80 */
    CAL_DATA_TYPE left_motor_fwd;   /* 1216 */
    CAL_DATA_TYPE left_motor_bwd;   /* 1424 */
    CAL_DATA_TYPE right_motor_fwd;  /* 1632 */
    CAL_DATA_TYPE right_motor_bwd;  /* 1840 */
} __attribute__ ((packed)) CAL_EEPROM_TYPE;
  
    
#endif