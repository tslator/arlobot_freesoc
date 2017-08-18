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
#include <project.h>
#include "config.h"
    
/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/        
#define TRUE (1 == 1)
#define FALSE (!TRUE)

/* Clear bit a in value b */
#define CLEAR_BIT(a, b) ((~(1<<a)) & (b))
/* Set bit a in value b */
#define SET_BIT(a, b) ((1<<a) | (b))
/* Get bit a in value b */
#define GET_BIT(a, b) ((b>>a) & (1))

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define constrain(a, b, c) (min(max(a, b), c))
// Scale X -> Y
//    X          Y
//  -----    = -----  => y = x * y range / x range
//  x range    y range
#define scale_integer(x, x_range, y_range) ( (int32) (x * y_range) / (int32) x_range )
#define scale_float(x, x_range, y_range) ( (float) (x * y_range) / (float) x_range )
    
#define scale_to_unity(input, neg_max, pos_max) (input < 0 ? (input/neg_max) : (input/pos_max) )
    
#define normalize(input, lower_range, upper_range)      (input / (upper_range - lower_range))
#define denormalize(input, lower_range, upper_range)    ((input * (upper_range - lower_range)) + lower_range)
    
#define SAMPLE_TIME_MS(rate)  (1000.0 / rate)
#define SAMPLE_TIME_SEC(rate) (SAMPLE_TIME_MS(rate) / 1000.0)

#define abs(x)  ( x < 0 ? -x : x )
    
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

    
/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/    

typedef struct _moving_average_tag
{
    int32 n;
    int32 last;
} MOVING_AVERAGE_TYPE;
    
typedef struct _moving_average_float_tag
{
    float n;
    float last;
} MOVING_AVERAGE_FLOAT_TYPE;
    
/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

int32 MovingAverage(MOVING_AVERAGE_TYPE* ma, int32 value);
float MovingAverageFloat(MOVING_AVERAGE_FLOAT_TYPE* ma, float value);

int16 TwoBytesToInt16(uint8* bytes);
uint16 TwoBytesToUint16(uint8* bytes);
uint32 FourBytesToUint32(uint8* bytes);
int32 FourBytesToInt32(uint8* bytes);
float FourBytesToFloat(uint8 *bytes);
void Int32ToFourBytes(int32 value, uint8* bytes);
void Uint16ToTwoBytes(uint16 value, uint8* bytes);
void FloatToFourBytes(float value, uint8* bytes);

void BinaryRangeSearch(int32 search, int32 *data_points, uint8 num_points, uint8 *lower_index, uint8 *upper_index);
int16 Interpolate(int16 x, int16 x1, int16 x2, uint16 y1, uint16 y2);

void UniToDiff(float linear, float angular, float *left, float *right);
void DiffToUni(float left, float right, float *linear, float *angular);
float CalcHeading(float left_count, float right_count, float radius, float width, float count_per_rev, float bias);
uint16 CpsToPwm(int32 cps, int32 *cps_data, uint16 *pwm_data, uint8 data_size);

float NormalizeHeading(float heading, DIR_TYPE direction);

void CalcTriangularProfile(uint8 num_points, float lower_limit, float upper_limit, float *profile);


#endif

/* [] END OF FILE */