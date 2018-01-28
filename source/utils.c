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

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "time.h"
#include "utils.h"
#include "config.h"
#include "serial.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    


/*---------------------------------------------------------------------------------------------------
 * Name: MovingAverage
 * Description: Calculates an integer moving average.
 *  
 * Parameters: ma    - the integer moving average structure
 *             value - the new value to be averaged
 * Return: average
 * 
 *-------------------------------------------------------------------------------------------------*/
INT32 MovingAverage(MOVING_AVERAGE_TYPE* const ma, INT32 value)
/*
MA*[i]= MA*[i-1] +X[i] - MA*[i-1]/N

where MA* is the moving average*N. 

MA[i]= MA*[i]/N
*/
{
    INT32 ma_curr;

    REQUIRE(ma != NULL);
    
    ma_curr = ma->last + value - ma->last/ma->n;
    ma->last = ma_curr;
    
    return ma_curr/ma->n;
}

/*---------------------------------------------------------------------------------------------------
 * Name: MovingAverageFloat
 * Description: Calculates a FLOATing point moving average.
 *  
 * Parameters: ma    - the FLOATing point moving average structure
 *             value - the new value to be averaged
 * Return: average
 * 
 *-------------------------------------------------------------------------------------------------*/
FLOAT MovingAverageFloat(MOVING_AVERAGE_FLOAT_TYPE* const ma, FLOAT value)
/*
MA*[i]= MA*[i-1] +X[i] - MA*[i-1]/N

where MA* is the moving average*N. 

MA[i]= MA*[i]/N
*/
{
    FLOAT ma_curr;
    
    REQUIRE(ma != NULL);
    
    ma_curr = ma->last + value - ma->last/ma->n;
    ma->last = ma_curr;
    
    return ma_curr/ma->n;
}

/*---------------------------------------------------------------------------------------------------
 * Name: UINT16ToTwoBytes
 * Description: Converts UINT16 to four bytes.  Note: endianess is preserved.
 *  
 * Parameters: value  - the UINT16 value
 *             *bytes - the resulting byte array
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Uint16ToTwoBytes(UINT16 value, UINT8* const bytes)
{
    /* Note: We are not changing the endianess, we're just getting a pointer to the first byte */
    
    REQUIRE(bytes != NULL);
    
    UINT8 *p_bytes = (UINT8 *) &value;
    
    bytes[0] = p_bytes[0];
    bytes[1] = p_bytes[1];
}

/*---------------------------------------------------------------------------------------------------
 * Name: Uint32ToFourBytes
 * Description: Converts UINT32 to four bytes.  Note: endianess is preserved.
 *  
 * Parameters: value  - the UINT32 value
 *             *bytes - the resulting byte array
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Uint32ToFourBytes(UINT32 value, UINT8* const bytes)
{
    /* Note: We are not changing the endianess, we're just getting a pointer to the first byte */

    REQUIRE(bytes != NULL);
    
    UINT8 *p_bytes = (UINT8 *) &value;
    
    bytes[0] = p_bytes[0];
    bytes[1] = p_bytes[1];
    bytes[2] = p_bytes[2];
    bytes[3] = p_bytes[3];
}

/*---------------------------------------------------------------------------------------------------
 * Name: Int32ToFourBytes
 * Description: Converts an INT32 to four bytes.  Note: endianess is preserved.
 *  
 * Parameters: value  - the INT32 value
 *             *bytes - the resulting byte array
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Int32ToFourBytes(INT32 value, UINT8* const bytes)
{
    REQUIRE(bytes != NULL);
    Uint32ToFourBytes((UINT32)value, bytes);
}

/*---------------------------------------------------------------------------------------------------
 * Name: FloatToFourBytes
 * Description: Converts a FLOAT 32 to four bytes.  Note: endianess is preserved.
 *  
 * Parameters: value  - the FLOAT 32 value
 *             *bytes - the resulting byte array
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void FloatToFourBytes(FLOAT value, UINT8* const bytes)
{
    /* Note: We are not changing the endianess, we're just getting a pointer to the first byte */

    REQUIRE(bytes != NULL);

    UINT8 *p_bytes = (UINT8 *) &value;
    
    bytes[0] = p_bytes[0];
    bytes[1] = p_bytes[1];
    bytes[2] = p_bytes[2];
    bytes[3] = p_bytes[3];
}

/*---------------------------------------------------------------------------------------------------
 * Name: TwoBytesUint16
 * Description: Converts four bytes to UINT16.  Note: endianess is preserved.
 *  
 * Parameters: *bytes - bytes to be converted
 * Return: UINT16
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT16 TwoBytesToUint16(UINT8* const bytes)
{
    REQUIRE(bytes != NULL);
    
    UINT16 value = *((UINT16 *) bytes);
    return value;
}

/*---------------------------------------------------------------------------------------------------
 * Name: TwoBytesInt16
 * Description: Converts four bytes to INT16.  Note: endianess is preserved.
 *  
 * Parameters: *bytes - bytes to be converted
 * Return: INT16
 * 
 *-------------------------------------------------------------------------------------------------*/
INT16 TwoBytesToInt16(UINT8* const bytes)
{
    REQUIRE(bytes != NULL);
    return (INT16) TwoBytesToUint16(bytes);
}

/*---------------------------------------------------------------------------------------------------
 * Name: FourBytesToUint32
 * Description: Converts four bytes to UINT32.  Note: endianess is preserved.
 *  
 * Parameters: *bytes - bytes to be converted
 * Return: UINT32
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT32 FourBytesToUint32(UINT8* const bytes)
{
    REQUIRE(bytes != NULL);
    
    UINT32 value = *((UINT32 *) bytes);
    return value;
}

/*---------------------------------------------------------------------------------------------------
 * Name: FourBytesToInt32
 * Description: Converts four bytes to INT32.  Note: endianess is preserved.
 *  
 * Parameters: *bytes - bytes to be converted
 * Return: INT32
 * 
 *-------------------------------------------------------------------------------------------------*/
INT32 FourBytesToInt32(UINT8* const bytes)
{
    REQUIRE(bytes != NULL);
    return (INT32) FourBytesToUint32(bytes);
}

/*---------------------------------------------------------------------------------------------------
 * Name: FourBytesToFloat
 * Description: Converts four bytes to FLOAT 32.  Note: endianess is preserved.
 *  
 * Parameters: *bytes - bytes to be converted
 * Return: FLOATing point value
 * 
 *-------------------------------------------------------------------------------------------------*/
FLOAT FourBytesToFloat(UINT8* const bytes)
{   
    REQUIRE(bytes != NULL);
    
    FLOAT value = *((FLOAT *) bytes);    
    return value;
}

/*---------------------------------------------------------------------------------------------------
 * Name: BinaryRangeSearch
 * Description: Performs a binary search to find a range, i.e., lower and upper values, within which
 *              the search value falls.
 *  
 * Parameters: search       - the value for which a range is sought
 *             *data_points - the data points to be searched
 *             num_points   - the number of data points
 *             *lower_index - the resulting lower value index into the data points
 *             *upper_index - the resulting upper value index into the data points
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void BinaryRangeSearch(INT16 search, INT16* const data_points, UINT8 num_points, UINT8* const lower_index, UINT8* const upper_index)
/*
    This is a binary search modified to return a range, e.g., lower and upper, when searching for an element.

    Performance:
    Array Size      log2N
        10            4
        50            6
       100            7
       500            9

*/
{
    UINT8 first = 0;
    UINT8 last = num_points - 1;
    UINT8 middle = (first+last)/2;
    
    REQUIRE(data_points != NULL);
    REQUIRE(lower_index != NULL);
    REQUIRE(upper_index != NULL);
    REQUIRE(num_points == CAL_NUM_SAMPLES);
    
    *lower_index = 0;
    *upper_index = 0;

    while (first <= last) {
        if (data_points[middle] < search)
        {
            first = middle + 1;
            *lower_index = middle;
        }
        else if (data_points[middle] == search) 
        {
            *lower_index = middle;
            *upper_index = middle;
            break;
        }
        else if (data_points[middle] > search)
        {            
            *upper_index = middle;
            last = middle;
            
            if (*upper_index - *lower_index == 1)
            {
                break;
            }
        }
        else
        {
            // There are no other possibilities :-)
            REQUIRE(FALSE);
        }
 
        middle = (first + last)/2;
   }

   ENSURE(in_range(*lower_index, 0, num_points-1));
   ENSURE(in_range(*upper_index, 0, num_points-1));
}

/*---------------------------------------------------------------------------------------------------
 * Name: UniToDiff
 * Description: Converts unicycle linear/angular velocity to differential left/right velocity
 *  
 * Parameters: left     - the differential left velocity (rad/s)
 *             right    - the differential right velocity (rad/s)
 *             *linear  - the linear velocity (m/s)
 *             *angular - the angular velocity (rad/s)
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void UniToDiff(FLOAT linear, FLOAT angular, FLOAT* const left, FLOAT* const right)
/*
    Vr = (2*V + W*L)/(2*R)
    Vl = (2*V - W*L)/(2*R)
    where 
        Vr - the velocity of the right wheel (rad/s)
        Vl - the velocity of the left wheel (rad/s)
        V - the linear velocity (m/s)
        W - the angular velocity (rad/s)
        L - the separation of the left and right wheels
        R - the radius of the wheel
*/
{
    REQUIRE(left != NULL);
    REQUIRE(right != NULL);
    REQUIRE(in_range_float(linear, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY));
    REQUIRE(in_range_float(angular, MAX_WHEEL_CCW_ANGULAR_VELOCITY, MAX_WHEEL_CW_ANGULAR_VELOCITY));
    
    *left = (2*linear - angular*TRACK_WIDTH)/WHEEL_DIAMETER;
    *right = (2*linear + angular*TRACK_WIDTH)/WHEEL_DIAMETER;

    ENSURE(in_range_float(*left, -MAX_WHEEL_RADIAN_PER_SECOND, MAX_WHEEL_RADIAN_PER_SECOND));
    ENSURE(in_range_float(*right, -MAX_WHEEL_RADIAN_PER_SECOND, MAX_WHEEL_RADIAN_PER_SECOND));
}

/*---------------------------------------------------------------------------------------------------
 * Name: DiffToUni
 * Description: Converts differential left/right velocity to unicycle linear/angular 
 *              velocity
 *  
 * Parameters: left     - the differential left velocity (rad/s)
 *             right    - the differential right velocity (rad/s)
 *             *linear  - the linear velocity (m/s)
 *             *angular - the angular velocity (rad/s)
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void DiffToUni(FLOAT left, FLOAT right, FLOAT* const linear, FLOAT* const angular)
/*
    V = R/2 * (Vr + Vl)
    W = R/L * (Vr - Vl)
    where
        V - the linear velocity (m/s)
        W - the angular velocity (rad/s)
        L - the separation of the left and right wheels
        R - the radius of the wheel
        Vr - the velocity of the right wheel (rad/s)
        Vl - the velocity of the left wheel (rad/s)

*/
{
    REQUIRE(linear != NULL);
    REQUIRE(angular != NULL);
    REQUIRE(in_range_float(left, -MAX_WHEEL_RADIAN_PER_SECOND, MAX_WHEEL_RADIAN_PER_SECOND));
    REQUIRE(in_range_float(right, -MAX_WHEEL_RADIAN_PER_SECOND, MAX_WHEEL_RADIAN_PER_SECOND));
    
    *linear = WHEEL_RADIUS * (right + left) / 2;
    *angular = WHEEL_RADIUS * (right - left) / TRACK_WIDTH;

    ENSURE(in_range_float(*linear, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY));
    ENSURE(in_range_float(*angular, MAX_WHEEL_CCW_ANGULAR_VELOCITY, MAX_WHEEL_CW_ANGULAR_VELOCITY));
}

/*---------------------------------------------------------------------------------------------------
 * Name: Interpolate
 * Description: Performs linear interpolation between two points
 *  
 * Parameters: x  - input value
 *             x1 - point 1 x value
 *             x2 - point 2 x value
 *             y1 - point 1 y value
 *             y2 - point 2 y value
 * Return: interpolated output
 * 
 *-------------------------------------------------------------------------------------------------*/
INT32 Interpolate(INT32 x, INT32 x1, INT32 x2, INT32 y1, INT32 y2)
{
    /* Goal:
            Y = ( ( X - X1 )( Y2 - Y1) / ( X2 - X1) ) + Y1 
    */

    INT32 result;

    result = 0;

    /* We are not guaranteed to not have duplicates in the array.
       
       Handle the cases where the values may be equal so we don't divide by zero and so we return a 
       reasonable value.
     */
    if (x1 == x2)
    {
        if (y2 == y1)
        {
            result = y1;
        }
        else
        {
            result = ((y2 - y1)/2 + y1);
        }
    }
    else
    {
        result = ((x - x1)*(y2 - y1))/(x2 - x1) + y1;
    }
    
    return result;
}

/*---------------------------------------------------------------------------------------------------
 * Name: CalcHeading
 * Description: Calculates the heading given left/right count, radius, width (distance between left/right
 *              wheels) and count/revolution.
 *  
 * Parameters: left_count    - the encoder count from the left wheel
 *             right_count   - the encoder count from the right wheel
 *             radius        - the radius of the wheel
 *             width         - the distance between left and right wheels
 *             count_per_rev - the encoder count per wheel revolution
 *             bias          - the bias adjustment
 * Return: heading
 * 
 *-------------------------------------------------------------------------------------------------*/
FLOAT CalcHeading(FLOAT left_count, FLOAT right_count, FLOAT radius, FLOAT width, FLOAT count_per_rev, FLOAT bias)
{
    return (bias * (right_count - left_count) * PI * radius)/(count_per_rev * width);
}

/*---------------------------------------------------------------------------------------------------
 * Name: NormalizeHeading
 * Description: Returns a heading ranging from -PI to Pi relative to the direction 
 *  
 * Parameters: heading - raw non-normalized heading as calculated in CalcHeading 
 *             direction - the direction of the rotation, i.e., CW or CCW 
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
FLOAT NormalizeHeading(FLOAT heading)
{
    FLOAT result = 0.0;

    result = heading;
    
    result -= (FLOAT)( (int)(result/TWOPI) ) * TWOPI;
    if (result < -PI) 
    { 
        result += TWOPI;
    }
    else if (result > PI)
    {
        result -= TWOPI; 
    }

    ENSURE(in_range_float(result, -PI, PI));

    return result;
}


/*---------------------------------------------------------------------------------------------------
 * Name: CalcTriangularProfile
 * Description: Returns a triangular profile of num_point values in the inclusive range of lower/upper
 *              limit.
 *  
 * Parameters: num_points - the number of points in the profile
 *             lower_limit - the starting value
 *             upper_limit - the ending value
 *             profile - pointer to array
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void CalcTriangularProfile(UINT8 num_points, FLOAT lower_limit, FLOAT upper_limit, FLOAT* const profile)
{
    UINT8 mid_sample_offset;
    FLOAT delta;
    FLOAT value;
    UINT8 ii;

    REQUIRE(num_points % 2 != 0);
    REQUIRE((lower_limit < 0 && upper_limit < 0 && lower_limit > upper_limit) || 
            (lower_limit > 0 && upper_limit > 0 && lower_limit < upper_limit));

    /* Calculate the mid point */
    mid_sample_offset = num_points / 2;
    
    /* Calculate the delta step */
    delta = (upper_limit - lower_limit)/mid_sample_offset;
    
    /* Set the mid point */
    profile[mid_sample_offset] = upper_limit;
    
    /* Set the 'up' slope values */
    value = lower_limit;
    for (ii = 0; ii < mid_sample_offset; ++ii)
    {
        profile[ii] = value;
        value += delta;
    }
    
    /* Set the 'down' slope values */ 
    value = upper_limit;
    for (ii = mid_sample_offset; ii < num_points; ++ii)
    {
        profile[ii] = value;
        value -= delta;
    }
    
}

void EnsureAngularVelocity(FLOAT* const v, FLOAT* const w)
{
    /*
    Ensure specified angular velocity can be met by adjusting linear velocity
    :param v: Linear velocity
    :param w: Angular velocity
    :param track_width: Wheel base width
    :param wheel_radius: Wheel radius
    :param ang_wheel_max: Maximum allowed angular velocity
    :return: Linear/Angular velocity
    */

    FLOAT l_v_d;
    FLOAT r_v_d;
    FLOAT max_rl_v;
    FLOAT min_rl_v;
    FLOAT temp_v;
    FLOAT temp_w;
    FLOAT l_v;
    FLOAT r_v;

    REQUIRE(v != NULL);
    REQUIRE(w != NULL);

    temp_v = *v;
    temp_w = *w;
    
    UniToDiff(temp_v, temp_w, &l_v_d, &r_v_d);

    max_rl_v = max(l_v_d, r_v_d);
    min_rl_v = min(l_v_d, r_v_d);

    l_v = 0;
    r_v = 0;

    //print("{:6.3f} {:6.3f} {:6.3f} {:6.3f} {:6.3f}".format(l_v_d, r_v_d, track_width, wheel_radius, ang_wheel_max))

    /* Only adjust if v and w are non-zero
    */
    if (temp_v != 0.0 && temp_w != 0.0)
    {
        if (max_rl_v > MAX_WHEEL_RADIAN_PER_SECOND)
        {
            r_v = r_v_d - (max_rl_v - MAX_WHEEL_RADIAN_PER_SECOND);
            l_v = l_v_d - (max_rl_v - MAX_WHEEL_RADIAN_PER_SECOND);
        }
        else if (min_rl_v < -MAX_WHEEL_RADIAN_PER_SECOND)
        {
            r_v = r_v_d - (min_rl_v + MAX_WHEEL_RADIAN_PER_SECOND);
            l_v = l_v_d - (min_rl_v + MAX_WHEEL_RADIAN_PER_SECOND);
        }
        else
        {
            l_v = l_v_d;
            r_v = r_v_d;
        }

        DiffToUni(l_v, r_v, &temp_v, &temp_w);

        *v = temp_v;
        *w = temp_w;
    }

    ENSURE(in_range_float(*v, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY));
    ENSURE(in_range_float(*w, MAX_WHEEL_CCW_ANGULAR_VELOCITY, MAX_WHEEL_CW_ANGULAR_VELOCITY));
}

/*---------------------------------------------------------------------------------------------------
 * Name: AdjustVelocity
 * Description: Calculates a new velocity based on the last velocity, max velocity and specified
 *              response time.
 * Parameters: last_velocity - the last adjusted velocity
 *             curr_velocity - the currently commanded velocity
 *             max_velocity  - the maximum allowed velocity
 *             response_time - the response time, i.e., how long to go from 0 to max velocity
 *             last_time     - the last time (in milliseconds) 
 * Return: limited velocity
 * 
 *-------------------------------------------------------------------------------------------------*/ 
static FLOAT AdjustVelocity(FLOAT last_velocity, FLOAT curr_velocity, FLOAT max_velocity, FLOAT response_time, UINT32* const last_time)
/*
    Explanation: The approach here is to limit acceleration in terms of a response time, e.g., wheel should reach new 
    speed within X seconds.  The worst case is 0 to max speed.  On each call delta time, delta velocity and percent
    change are calculated.  The velocity adjustment is proportional to the worst case velocity change.  The velocity is
    adjusted until the newly command velocity is reached.  The resulting response is an S curve due to the percent
    change getting increasingly smaller as the target velocity is reached.
*/
{
    FLOAT delta_time;
    FLOAT delta_velocity;
    FLOAT percent_change;
    FLOAT adjust;
    FLOAT velocity;
    FLOAT now;
    
    REQUIRE(last_time != NULL);
    
    velocity = 0.0;
    
    now = millis();
    delta_time = (FLOAT) (now - *last_time) / 1000.0;
    *last_time = now;
    delta_velocity = curr_velocity - last_velocity;
    
    percent_change = abs(delta_velocity / max_velocity);
    adjust = IS_NAN_DEFAULT(delta_velocity / (response_time * percent_change / delta_time), 0.0);
    
    if (adjust > 0.0 && adjust < 0.0001)
    {
        adjust = 0.0;
    }
    else if (adjust < 0.0 && adjust > -0.0001)
    {
        adjust = 0.0;
    }
    
    if (last_velocity <= curr_velocity)
    {
        velocity = min(last_velocity + adjust, curr_velocity);
    }
    else if (last_velocity > curr_velocity)
    {
        velocity = max(last_velocity + adjust, curr_velocity);
    }

    ENSURE(in_range_float(velocity, last_velocity, max_velocity));

    return velocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: LimitLinearAccel
 * Description: Limits linear velocity based on the defined linear response time.
 * Parameters: linear_velocity - the linear velocity to be limited.
 *             max_linear      - maximum linear velocity.
 *             response_time   - maximum time (in milliseconds) in which to acheive specified velocity.
 * Return: updated velocity
 * 
 *-------------------------------------------------------------------------------------------------*/ 
FLOAT LimitLinearAccel(FLOAT const linear_velocity, FLOAT const max_linear, FLOAT const response_time)
{
    static FLOAT last_velocity = 0.0;
    static UINT32 last_time = 0;

    ENSURE(in_range_float(linear_velocity, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY));
    ENSURE(in_range_float(max_linear, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY));
    ENSURE(linear_velocity >= 0.0 ? linear_velocity < max_linear : linear_velocity > max_linear);
    
    last_velocity = AdjustVelocity(last_velocity, 
                                   linear_velocity, 
                                   max_linear, 
                                   response_time, 
                                   &last_time);

    ENSURE(in_range_float(last_velocity, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY));

    return last_velocity;
}

/*---------------------------------------------------------------------------------------------------
 * Name: LimitAngularAccel
 * Description: Limits angular velocity based on the defined angular response time.
 * Parameters: angular_velocity - the angular velocity to be limited.
 *             max_angular      - maximum angular velocity.
 *             response_time    - maximum time (in milliseconds) in which to acheive specified velocity.
 * Return: updated velocity
 * 
 *-------------------------------------------------------------------------------------------------*/ 
FLOAT LimitAngularAccel(FLOAT angular_velocity, FLOAT max_angular, FLOAT response_time)
{
    static FLOAT last_velocity = 0.0;
    static UINT32 last_time = 0;

    ENSURE(in_range_float(angular_velocity, MAX_WHEEL_CCW_ANGULAR_VELOCITY, MAX_WHEEL_CW_ANGULAR_VELOCITY));
    ENSURE(in_range_float(max_angular, MAX_WHEEL_CCW_ANGULAR_VELOCITY, MAX_WHEEL_CW_ANGULAR_VELOCITY));
    ENSURE(angular_velocity >= 0.0 ? angular_velocity < max_angular : angular_velocity > max_angular);

    last_velocity = AdjustVelocity(last_velocity, 
                                   angular_velocity, 
                                   max_angular, 
                                   response_time, 
                                   &last_time);

    ENSURE(in_range_float(last_velocity, MAX_WHEEL_CCW_ANGULAR_VELOCITY, MAX_WHEEL_CCW_ANGULAR_VELOCITY));

    return last_velocity;    
}

FLOAT CalcMaxLinearVelocity()
{
    FLOAT max_linear;
    FLOAT dont_care;
    
    DiffToUni(MAX_WHEEL_RADIAN_PER_SECOND, MAX_WHEEL_RADIAN_PER_SECOND, &max_linear, &dont_care);
    
    ENSURE(in_range_float(max_linear, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY));
    
    return max_linear;
}

FLOAT CalcMaxAngularVelocity()
{
    FLOAT max_angular;
    FLOAT dont_care;
    
    DiffToUni(MAX_WHEEL_RADIAN_PER_SECOND, -MAX_WHEEL_RADIAN_PER_SECOND, &dont_care, &max_angular);
    
    ENSURE(in_range_float(max_angular, MAX_WHEEL_CCW_ANGULAR_VELOCITY, MAX_WHEEL_CCW_ANGULAR_VELOCITY));
    
    return max_angular;
}

FLOAT CalcMaxDiffVelocity()
{
    FLOAT max_diff;
    FLOAT dont_care;
    
    UniToDiff(MAX_WHEEL_METER_PER_SECOND, 0, &max_diff, &dont_care);
    
    ENSURE(in_range_float(max_diff, MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, MAX_WHEEL_FORWARD_LINEAR_VELOCITY));
    
    return max_diff;
}

CHAR * format_string(CHAR *str, FORMAT_TYPE format)
{
    ENSURE(str != NULL);
    ENSURE(format == FORMAT_UPPER || format == FORMAT_LOWER || format == FORMAT_TITLE);

    switch (format)
    {
        case FORMAT_UPPER:
            return strupr(str);
        
        case FORMAT_LOWER:
            return strlwr(str);
            
        case FORMAT_TITLE:
            str = strlwr(str);
            toupper(str[0]);
            return str;
    }

    return str;
}

CHAR * const WheelToString(WHEEL_TYPE wheel, FORMAT_TYPE format)
{
    switch (wheel)
    {
        case WHEEL_LEFT:
        {
            switch (format)
            {
                case FORMAT_UPPER:
                    return "LEFT";
                
                case FORMAT_LOWER:
                    return "left";

                case FORMAT_TITLE:
                    return "Left";
            }
        }

        case WHEEL_RIGHT:
        {
            switch (format)
            {
                case FORMAT_UPPER:
                    return "RIGHT";
                
                case FORMAT_LOWER:
                    return "right";

                case FORMAT_TITLE:
                    return "Right";
            }
        }

        case WHEEL_BOTH:
        {
            switch (format)
            {
                case FORMAT_UPPER:
                    return "BOTH";
                
                case FORMAT_LOWER:
                    return "both";

                case FORMAT_TITLE:
                    return "Both";
            }
        }
    }

    return "unknown wheel";
}

UINT32 GetRandomValue(UINT32 start, UINT32 end)
{
    return constrain((systick() % end), start, end);
}

/* [] END OF FILE */
