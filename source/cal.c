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
#include "cal.h"
#include "motor.h"
#include "i2c.h"
#include "pwm.h"
#include "encoder.h"
#include "utils.h"
#include "serial.h"
#include "pid.h"
#include "nvstore.h"
#include "calcps2pwm.h"
#include "calpid.h"
#include "callin.h"
#include "calang.h"

//#define DEFAULT_CALIBRATION
#ifdef DEFAULT_CALIBRATION
static CAL_DATA_TYPE left_fwd_cal_data = {
    0,
    28006,
    CAL_SCALE_FACTOR,
    { /*  1     2     3     4     5     6      7    8     9     10  */
            0,    0,    0,   95,  605, 1270, 1922, 2580, 3251, 3917,
         4504, 5040, 5629, 6395, 7064, 7591, 8220, 8890, 9404,10080,
        10736,11326,11969,12539,13161,13767,14377,14961,15590,16126,
        16711,17317,17854,18400,18915,19465,19998,20534,21071,21623,
        22157,22660,23174,23684,24175,24667,25173,26212,27496,27940,
        28006
    },
    {1500, 1510, 1520, 1530, 1540, 1550, 1560, 1570, 1580, 1590, 
     1600, 1610, 1620, 1630, 1640, 1650, 1660, 1670, 1680, 1690, 
     1700, 1710, 1720, 1730, 1740, 1750, 1760, 1770, 1780, 1790,  
     1800, 1810, 1820, 1830, 1840, 1850, 1860, 1870, 1880, 1890, 
     1900, 1910, 1920, 1930, 1940, 1950, 1960, 1970, 1980, 1990, 2000
    },
    0
};

static CAL_DATA_TYPE left_bwd_cal_data = {
    -28447,
    0,
    CAL_SCALE_FACTOR,
    { /*   1      2      3      4      5      6      7      8      9     10  */
        -28447,-28021,-26732,-25679,-25219,-24723,-24232,-23750,-23327,-22774,
        -22266,-21779,-21238,-20692,-20130,-19573,-19003,-18434,-17850,-17264,
        -16676,-16057,-15457,-14850,-14243,-13631,-12968,-12352,-11705,-11079,
        -10446, -9808, -9189, -8516, -7862, -7279, -6610, -5966, -5317, -4671,
         -4036, -3417, -2779, -2161, -1558,  -893,  -236,     0,     0,     0,
        0
    },
    {1000,1010,1020,1030,1040,1050,1060,1070,1080,1090,
     1100,1110,1120,1130,1140,1150,1160,1170,1180,1190,
     1200,1210,1220,1230,1240,1250,1260,1270,1280,1290,
     1300,1310,1320,1330,1340,1350,1360,1370,1380,1390,
     1400,1410,1420,1430,1440,1450,1460,1470,1480,1490,1500
    },
    0
};


static CAL_DATA_TYPE right_fwd_cal_data = {
    0,
    26523,
    CAL_SCALE_FACTOR,
    { /*  1     2     3     4     5     6     7     8     9     10  */
            0,    0,    0,    0,    0,  132,  679, 1344, 1990, 2558,
         3107, 3663, 4263, 4926, 5625, 6195, 6708, 7346, 8051, 8567,
        9202,  9828,10375,11009,11539,12171,12715,13365,13903,14546,
        15075,15691,16247,16783,17358,17884,18379,18873,19401,19936,
        20404,20881,21362,21848,22327,22795,23270,23744,24264,25274,
        26523
    },
    {            
        1500,1490,1480,1470,1460,1450,1440,1430,1420,1410,
        1400,1390,1380,1370,1360,1350,1340,1330,1320,1310,
        1300,1290,1280,1270,1260,1250,1240,1230,1220,1210,
        1200,1190,1180,1170,1160,1150,1140,1130,1120,1110,
        1100,1090,1080,1070,1060,1050,1040,1030,1020,1010,
        1000    
    },
    0
};

    
static CAL_DATA_TYPE right_bwd_cal_data = {
    -27555,
    0,
    CAL_SCALE_FACTOR,
    { /*   1      2      3      4      5      6      7      8      9     10  */
        -27555,-27538,-27503,-27368,-26971,-25685,-24599,-24093,-23610,-23161,
        -22669,-22213,-21733,-21267,-20755,-20240,-19714,-19212,-18693,-18131,
        -17532,-16966,-16408,-15839,-15274,-14694,-14105,-13469,-12874,-12271,
        -11665,-11039,-10445, -9817, -9231, -8592, -7984, -7384, -6771, -6106,
         -5472, -4875, -4233, -3585, -2977, -2337, -1691, -1025,  -333,   -27,
        0
    },
    {
        2000,1990,1980,1970,1960,1950,1940,1930,1920,1910,
        1900,1890,1880,1870,1860,1850,1840,1830,1820,1810,
        1800,1790,1780,1770,1760,1750,1740,1730,1720,1710,
        1700,1690,1680,1670,1660,1650,1640,1630,1620,1610,
        1600,1590,1580,1570,1560,1550,1540,1530,1520,1510,
        1500
    },
    0
};    
#endif

static uint8 cal_verbose;

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

void Cal_Init()
{
    p_cal_eeprom = NVSTORE_CAL_EEPROM_BASE;
    cal_verbose = 0;
}

void Cal_Start()
{
    //Nvstore_WriteBytes((uint8 *) &left_fwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_motor_fwd));
    //Nvstore_WriteBytes((uint8 *) &left_bwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->left_motor_bwd));
    //Nvstore_WriteBytes((uint8 *) &right_fwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_motor_fwd));
    //Nvstore_WriteBytes((uint8 *) &right_bwd_cal_data, sizeof(CAL_DATA_TYPE), NVSTORE_CAL_EEPROM_ADDR_TO_OFFSET(&p_cal_eeprom->right_motor_bwd));
    //Nvstore_WriteUint16(0, 0);
    
    I2c_SetCalibrationStatusBit(p_cal_eeprom->status);
}

void Cal_Update(uint16 status)
{   
    cal_verbose = status & CAL_VERBOSE_BIT;
    
    if (status & CAL_COUNT_PER_SEC_TO_PWM_BIT)
    {
        ClearCalibrationStatusBit(CAL_COUNT_PER_SEC_TO_PWM_BIT);
        PerformCountPerSecToPwmCalibration(cal_verbose);
        SetCalibrationStatusBit(CAL_COUNT_PER_SEC_TO_PWM_BIT);
    }
    else if (status & CAL_PID_BIT)
    {
        ClearCalibrationStatusBit(CAL_PID_BIT);
        PerformPidCalibration(cal_verbose);
        SetCalibrationStatusBit(CAL_PID_BIT);
    }
    else if (status & CAL_LINEAR_BIAS_BIT)
    {
        ClearCalibrationStatusBit(CAL_LINEAR_BIAS_BIT);
        PerformLinearBiasCalibration(cal_verbose);
        SetCalibrationStatusBit(CAL_LINEAR_BIAS_BIT);
    }
    else if (status & CAL_ANGULAR_BIAS_BIT)
    {
        ClearCalibrationStatusBit(CAL_ANGULAR_BIAS_BIT);
        PerformAngularBiasCalibration(cal_verbose);
        SetCalibrationStatusBit(CAL_ANGULAR_BIAS_BIT);
    }
    
    cal_verbose = 0;
}

void Cal_LeftGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data)
{
#ifdef DEFAULT_CALIBRATION    
    *fwd_cal_data = &left_fwd_cal_data;
    *bwd_cal_data = &left_bwd_cal_data;
#else
    *fwd_cal_data = (CAL_DATA_TYPE *) &(p_cal_eeprom->left_motor_fwd);
    *bwd_cal_data = (CAL_DATA_TYPE *) &(p_cal_eeprom->left_motor_bwd);
#endif    
}

void Cal_RightGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data)
{
#ifdef DEFAULT_CALIBRATION    
    *fwd_cal_data = &right_fwd_cal_data;
    *bwd_cal_data = &right_bwd_cal_data;
#else
    *fwd_cal_data = (CAL_DATA_TYPE *) &(p_cal_eeprom->right_motor_fwd);
    *bwd_cal_data = (CAL_DATA_TYPE *) &(p_cal_eeprom->right_motor_bwd);
#endif    
}

CAL_PID_TYPE * Cal_LeftGetPidGains()
{
    return (CAL_PID_TYPE *) &p_cal_eeprom->left_gains;
}

CAL_PID_TYPE * Cal_RightGetPidGains()
{
    return (CAL_PID_TYPE *) &p_cal_eeprom->right_gains;
}

void Cal_Validate()
{
}

/* [] END OF FILE */
