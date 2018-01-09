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
   Description: This module provides utility macros and functions used by one more modules.
 *-------------------------------------------------------------------------------------------------*/    

#ifndef UTILS_H
#define UTILS_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "freesoc.h"
#include "consts.h"
    
/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/        

/* Clear bit a in value b */
#define CLEAR_BIT(a, b) ((~(1<<a)) & (b))
/* Set bit a in value b */
#define SET_BIT(a, b) ((1<<a) | (b))
/* Get bit a in value b */
#define GET_BIT(a, b) ((b>>a) & (1))

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define constrain(a, b, c) (min(max(a, b), c))

#define in_range(value, lower, upper) (value >= lower ? (value <= upper ? TRUE : FALSE) : FALSE)

#define equal_float(f1, f2, precision)  (((f1 > (f2 - precision)) && (f1 < (f2 + precision))) ? TRUE : FALSE)
#define lessthan_float(f1, f2, precision) ((f1 < (f2 + precision)) ? TRUE : FALSE)
#define greaterthan_float(f1, f2, precision) ((f1 > (f2 - precision)) ? TRUE : FALSE)
#define compare_float(f1, f2, precision) (equal_float(f1, f2, precision) ? 0 : (lessthan_float(f1, f2, precision) ? -1 : 1))
#define in_range_float(value, lower, upper) (greaterthan_float(value, lower, FLOAT_PRECISION) ? (lessthan_float(value, upper, FLOAT_PRECISION) ? TRUE : FALSE) : FALSE)

// Scale X -> Y
//    X          Y
//  -----    = -----  => y = x * y range / x range
//  x range    y range
#define scale_integer(x, x_range, y_range) ( (INT32) (x * y_range) / (INT32) x_range )
#define scale_FLOAT(x, x_range, y_range) ( (FLOAT) (x * y_range) / (FLOAT) x_range )
    
#define scale_to_unity(input, neg_max, pos_max) (input < 0 ? (input/neg_max) : (input/pos_max) )
    
#define normalize(input, lower_range, upper_range)      (input / (upper_range - lower_range))
#define denormalize(input, lower_range, upper_range)    ((input * (upper_range - lower_range)) + lower_range)
    
#define SAMPLE_TIME_MS(rate)  (1000.0 / rate)
#define SAMPLE_TIME_SEC(rate) (SAMPLE_TIME_MS(rate) / 1000.0)

#define abs(x)  ( x < 0 ? -x : x )
#define is_even(x)  (x % 2 == 0 ? TRUE : FALSE)
#define is_odd(x) (!is_even(x))
    
#define constrain_angle(angle) do                         \
                               {                          \
                                   if (angle > PI)        \
                                       angle -= 2*PI;     \
                                   else if (angle <= -PI) \
                                       angle += 2*PI;     \
                               } while (0);
// Note: An alternate way to constrain an angle is to atan2(sin(theta), cos(theta))                               

    
#define APPLY_SCHED_OFFSET(offset, applied) do {                        \
                                                if (!applied)           \
                                                {                       \
                                                    applied = 1;        \
                                                    CyDelay(offset);    \
                                                }                       \
                                            } while (0);
    

#define DEGREES_TO_RADIANS(d)   (d * PI / 180)
#define RADIANS_TO_DEGREES(r)   (r * 180 / PI)

#define IS_NAN_DEFAULT(x,d)   isnan(x) ? d : x

#define MPS_TO_CPS(mps) (mps / WHEEL_METER_PER_COUNT)
#define CPS_TO_MPS(cps) (cps * WHEEL_METER_PER_COUNT)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/    

typedef struct _moving_average_tag
{
    INT32 n;
    INT32 last;
} MOVING_AVERAGE_TYPE;
    
typedef struct _moving_average_FLOAT_tag
{
    FLOAT n;
    FLOAT last;
} MOVING_AVERAGE_FLOAT_TYPE;


typedef enum {FORMAT_UPPER, FORMAT_LOWER, FORMAT_TITLE} FORMAT_TYPE;
    
/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

INT32 MovingAverage(MOVING_AVERAGE_TYPE* const ma, INT32 value);
FLOAT MovingAverageFloat(MOVING_AVERAGE_FLOAT_TYPE* const ma, FLOAT value);

INT16 TwoBytesToInt16(UINT8* const bytes);
UINT16 TwoBytesToUint16(UINT8* const bytes);
UINT32 FourBytesToUint32(UINT8* const bytes);
INT32 FourBytesToInt32(UINT8* const bytes);
FLOAT FourBytesToFloat(UINT8* const bytes);
void Uint16ToTwoBytes(UINT16 value, UINT8* const bytes);
void Int32ToFourBytes(INT32 value, UINT8* const bytes);
void Uint16ToTwoBytes(UINT16 value, UINT8* const bytes);
void FloatToFourBytes(FLOAT value, UINT8* const bytes);

void BinaryRangeSearch(INT16 search, INT16* const data_points, UINT8 num_points, UINT8* const lower_index, UINT8* const upper_index);
INT16 Interpolate(INT16 x, INT16 x1, INT16 x2, UINT16 y1, UINT16 y2);

void UniToDiff(FLOAT linear, FLOAT angular, FLOAT* const left, FLOAT* const right);
void DiffToUni(FLOAT left, FLOAT right, FLOAT* const linear, FLOAT* const angular);
FLOAT CalcHeading(FLOAT left_count, FLOAT right_count, FLOAT radius, FLOAT width, FLOAT count_per_rev, FLOAT bias);

FLOAT NormalizeHeading(FLOAT heading);

void CalcTriangularProfile(UINT8 num_points, FLOAT lower_limit, FLOAT upper_limit, FLOAT *profile);
void EnsureAngularVelocity(FLOAT* const linear_cmd_velocity, FLOAT* const angular_cmd_velocity);

FLOAT LimitLinearAccel(FLOAT linear_velocity, FLOAT max_linear, FLOAT response_time);
FLOAT LimitAngularAccel(FLOAT angular_velocity, FLOAT max_angular, FLOAT response_time);

FLOAT CalcMaxLinearVelocity();
FLOAT CalcMaxAngularVelocity();
FLOAT CalcMaxDiffVelocity();

CHAR * const WheelToString(WHEEL_TYPE wheel, FORMAT_TYPE format);
CHAR * format_string(CHAR *str, FORMAT_TYPE format);

#endif

/* [] END OF FILE */