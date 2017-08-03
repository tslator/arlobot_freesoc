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

#ifndef CALPID_H
#define CALPID_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <project.h>
#include "config.h"
#include "cal.h"

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef enum {PID_TYPE_LEFT, PID_TYPE_RIGHT} PID_ENUM_TYPE;

typedef struct cal_pid_params_tag
{
    char name[6];
    PID_ENUM_TYPE pid_type;
    DIR_TYPE direction;
    uint32 run_time;
} CAL_PID_PARAMS;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
CALIBRATION_TYPE *CalPid_LeftCalibration;
CALIBRATION_TYPE *CalPid_RightCalibration;
CALIBRATION_TYPE *CalPid_LeftValidation;
CALIBRATION_TYPE *CalPid_RightValidation;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void CalPid_Init();

#endif