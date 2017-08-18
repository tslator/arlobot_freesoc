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
   Description: This module provides the implementation of the PWM for the motors.
 *-------------------------------------------------------------------------------------------------*/

#ifndef PWM_H
#define PWM_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/
#include <project.h>

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/

/* Note: While technically the motors are reverse relative to each other, it is clearer to hide this difference from
   the code.  Therefore, while the pwm values will be different, for a code perspective the functionality will be the 
   same between the boards driving the motors.  For example, the pwm constants refer to forward and reverse even though
   the actual values are different and the motors are moving oppositely.

                            Left    Right
        PWM_FULL_FORWARD |  2000  | 1000  |
        PWM_STOP         |  1500  | 1500  |
        PWM_FULL_REVERSE |  1000  | 2000  |
        PWM_CAL_STEP     |   10   |  10   |
        PWM_MIN          |  2000  | 1500  |
        PWM_MAX          |  1500  | 1000  |
 */

/* Per the HB-25 data sheet: 
    1.0 ms Full Reverse 
    1.5 ms Neutral (Off) 
    2.0 ms Full Forward
 */

#define MAX_PWM_VALUE  (2000)
#define MID_PWM_VALUE  (1500)
#define MIN_PWM_VALUE  (1000)

#define PWM_STOP        (MID_PWM_VALUE)

#define LEFT_PWM_STOP               (PWM_STOP)
#define LEFT_PWM_FULL_FORWARD       (MAX_PWM_VALUE)
#define LEFT_PWM_FORWARD_DOMAIN     (LEFT_PWM_FULL_FORWARD - LEFT_PWM_STOP)
#define LEFT_PWM_FULL_BACKWARD      (MIN_PWM_VALUE)
#define LEFT_PWM_BACKWARD_DOMAIN    (LEFT_PWM_STOP - LEFT_PWM_FULL_BACKWARD)
#define RIGHT_PWM_FULL_FORWARD      (MIN_PWM_VALUE)
#define RIGHT_PWM_STOP              (PWM_STOP)
#define RIGHT_PWM_FORWARD_DOMAIN    (RIGHT_PWM_STOP - RIGHT_PWM_FULL_FORWARD)
#define RIGHT_PWM_FULL_BACKWARD     (MAX_PWM_VALUE)
#define RIGHT_PWM_BACKWARD_DOMAIN   (RIGHT_PWM_FULL_BACKWARD - RIGHT_PWM_STOP)
    
 
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef uint16 PWM_TYPE;

typedef struct _pwm_params
{
    uint16 start;
    uint16 end;
    int16 step;
} PWM_PARAMS_TYPE;


#endif

/* [] END OF FILE */
