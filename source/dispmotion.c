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
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <stdarg.h>
#include "dispmotion.h"
#include "cal.h"
#include "control.h"
#include "odom.h"
#include "encoder.h"
#include "conserial.h"
#include "utils.h"
#include "debug.h"
#include "time.h"
#include "concmdif.h"
#include "motion.h"
#include "motor.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/

typedef struct _tag_motion_cal_umbmark
{
} MOTION_CAL_UMBMARK_TYPE;

typedef struct _tag_motion_cal_angular
{
    FLOAT angle;
} MOTION_CAL_ANGULAR_TYPE;

typedef struct _tag_motion_cal_linear
{
    FLOAT distance;
}MOTION_CAL_LINEAR_TYPE;


/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/

static BOOL is_running;

static MOTION_CAL_LINEAR_TYPE motion_cal_linear;
static MOTION_CAL_ANGULAR_TYPE motion_cal_angular;
static MOTION_CAL_UMBMARK_TYPE motion_cal_umbmark;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static void CalLinearResults(void)
{

    Motion_Results();

    /* It is necessary to stop the motors explicitly because "Results" will be called before the 
       main loop can be executed.
    */
    Motor_SetPwm(PWM_STOP, PWM_STOP);

    ConSer_WriteLine(FALSE, "Enter the actual distance traveled: ");
    FLOAT result = Cal_ReadResponseFloat();
    ConSer_WriteLine(TRUE, "");
    FLOAT bias = 1.0 / result;
    ConSer_WriteLine(TRUE, "Linear Bias: %f", bias);
}

static void CalAngularResults(void)
{
    Motion_Results();

    /* It is necessary to stop the motors explicitly because "Results" will be called before the 
       main loop can be executed.
    */
    Motor_SetPwm(PWM_STOP, PWM_STOP);

    ConSer_WriteLine(FALSE, "Enter the actual rotation traveled: ");
    FLOAT result = Cal_ReadResponseFloat();
    ConSer_WriteLine(TRUE, "");
    FLOAT bias = 360.0 / result;
    ConSer_WriteLine(TRUE, "Angular Bias: %f", bias);
}

/*------------------------------------------------------------------------------------------
    Module Interface
*/
void DispMotion_Init(void)
{
    Motion_Init();
}

void DispMotion_InitCalLinear(FLOAT distance)
{
    //motion_cal_linear_init(distance);

    /* Give instructions:
        1. Place a 1 meter stick along side the robot starting at the axel and extending in the direction the robot will move
        2. Hit <enter> to start
        3. When robot stops, measure the actual distance traveled and enter it
    */

    LINEAR_MOVE_DATA_TYPE linear_move_data = {distance, 0.2};

    ConSer_WriteLine(TRUE, "Place a 1 meter stick along side the robot starting\r\nat the axel and extending in the forward direction");
    ConSer_WriteLine(FALSE, "Hit <enter> to start.");
    (void) Cal_ReadResponseReturn();
    ConSer_WriteLine(TRUE, "");
    Motion_AddMove(MOVE_LINEAR, (void *) &linear_move_data);
    ConCmdIf_SetIfaceFuncs(Motion_Update, Motion_Status, CalLinearResults);
    Motion_Start();
}

void DispMotion_InitMotionCalAngular(FLOAT angle)
{
    //motion_cal_angular_init(angle);

    /* Give instructions:
        1. Place a marked on the ground in front of the robot to mark the 0 degree position
        2. Hit <enter> to start
        3. When robot stops, measure the actual rotational distance and enter it (in degrees)
    */
    ANGULAR_MOVE_DATA_TYPE angular_move_data = {angle, 0.5};

    ConSer_WriteLine(TRUE, "Place a marker on the ground in front\r\nof the robot to mark the 0-degree position");
    ConSer_WriteLine(FALSE, "Hit <enter> to start.");
    (void) Cal_ReadResponseReturn();
    ConSer_WriteLine(TRUE, "");
    Motion_AddMove(MOVE_ROTATE, (void *) &angular_move_data);
    ConCmdIf_SetIfaceFuncs(Motion_Update, Motion_Status, CalAngularResults);
    Motion_Start();
}

void DispMotion_InitMotionCalUmbmark(void)
{
    // No implementation
}

void DispMotion_InitMotionValLinear(FLOAT distance)
{
    LINEAR_MOVE_DATA_TYPE linear_move_data = {distance, 0.2};

    Motion_AddMove(MOVE_LINEAR, (void *) &linear_move_data);
    ConCmdIf_SetIfaceFuncs(Motion_Update, Motion_Status, Motion_Results);
    Motion_Start();
}

void DispMotion_InitMotionValAngular(FLOAT angle)
{
    ANGULAR_MOVE_DATA_TYPE angular_move_data = {angle, 0.5};

    Motion_AddMove(MOVE_ROTATE, (void *) &angular_move_data);
    ConCmdIf_SetIfaceFuncs(Motion_Update, Motion_Status, Motion_Results);
    Motion_Start();
}

void DispMotion_InitMotionValSquare(BOOL left, FLOAT side)
{
    LINEAR_MOVE_DATA_TYPE linear_move_data = {side, 0.2};
    ANGULAR_MOVE_DATA_TYPE angular_move_data = {DEGREES_TO_RADIANS(90), 0.5};
    PAUSE_MOVE_DATA_TYPE pause_move_data = {2.0};

    /* Side 1 - move linear distance, pause, rotate 90, pause */
    Motion_AddMove(MOVE_LINEAR, (void *) &linear_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);
    Motion_AddMove(MOVE_ROTATE, (void *) &angular_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);

    /* Side 2 - move linear distance, pause, rotate 90, pause */
    Motion_AddMove(MOVE_LINEAR, (void *) &linear_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);
    Motion_AddMove(MOVE_ROTATE, (void *) &angular_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);

    /* Side 3 - move linear distance, pause, rotate 90, pause */
    Motion_AddMove(MOVE_LINEAR, (void *) &linear_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);
    Motion_AddMove(MOVE_ROTATE, (void *) &angular_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);

    /* Side 4 - move linear distance, pause, rotate 90, pause */
    Motion_AddMove(MOVE_LINEAR, (void *) &linear_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);
    Motion_AddMove(MOVE_ROTATE, (void *) &angular_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);

    ConCmdIf_SetIfaceFuncs(Motion_Update, Motion_Status, Motion_Results);
    Motion_Start();
}

void DispMotion_InitMotionValCircle(BOOL cw, FLOAT radius)
{
    //motion_val_circle_init(cw, radius);
}

void DispMotion_InitMotionValOutAndBack(FLOAT distance)
{
    LINEAR_MOVE_DATA_TYPE linear_move_data = {distance, 0.2};
    ANGULAR_MOVE_DATA_TYPE angular_move_data = {DEGREES_TO_RADIANS(180), 0.5};
    PAUSE_MOVE_DATA_TYPE pause_move_data = {2.0};

    /* Move linear distance, pause, rotate 180, pause */
    Motion_AddMove(MOVE_LINEAR, (void *) &linear_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);
    Motion_AddMove(MOVE_ROTATE, (void *) &angular_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);

    /* Move linear distance, pause, rotate 180, pause */
    Motion_AddMove(MOVE_LINEAR, (void *) &linear_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);
    Motion_AddMove(MOVE_ROTATE, (void *) &angular_move_data);
    Motion_AddMove(MOVE_PAUSE, (void *) &pause_move_data);

    ConCmdIf_SetIfaceFuncs(Motion_Update, Motion_Status, Motion_Results);
    Motion_Start();
}

void DispMotion_Start(void)
{
    // No Implementation Required
}

// EOF