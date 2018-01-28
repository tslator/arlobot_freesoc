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
 * Description: 
 *-------------------------------------------------------------------------------------------------*/

#ifndef CONMOTOR_H
#define CONMOTOR_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include "freesoc.h"
#include "concmd.h"

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
void DispMotor_Init(void);
void DispMotor_Start(void);
void DispMotor_InitMotorShow(WHEEL_TYPE wheel, BOOL plain_text);
void DispMotor_InitMotorRepeat(
            WHEEL_TYPE wheel,
            FLOAT first,
            FLOAT second,
            FLOAT intvl,
            INT8 iters,
            BOOL no_pid,
            BOOL no_accel);
void DispMotor_InitMotorCal(
            WHEEL_TYPE wheel,
            INT8 iters);
void DispMotor_InitMotorVal(
            WHEEL_TYPE wheel,
            DIR_TYPE direction,
            FLOAT min_percent,
            FLOAT max_percent,
            INT8 num_points);
void DispMotor_InitMotorMove(
            FLOAT left_speed,
            FLOAT right_speed,
            FLOAT duration,
            BOOL no_pid,
            BOOL no_accel);

#endif
    