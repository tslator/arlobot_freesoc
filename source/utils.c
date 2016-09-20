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
#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include "config.h"

int32 MovingAverage(MOVING_AVERAGE_TYPE* ma, int32 value)
/*
MA*[i]= MA*[i-1] +X[i] - MA*[i-1]/N

where MA* is the moving average*N. 

MA[i]= MA*[i]/N
*/
{
    int32 ma_curr;
    
    ma_curr = ma->last + value - ma->last/ma->n;
    ma->last = ma_curr;
    
    return ma_curr/ma->n;
}

float MovingAverageFloat(MOVING_AVERAGE_FLOAT_TYPE* ma, float value)
/*
MA*[i]= MA*[i-1] +X[i] - MA*[i-1]/N

where MA* is the moving average*N. 

MA[i]= MA*[i]/N
*/
{
    float ma_curr;
    
    ma_curr = ma->last + value - ma->last/ma->n;
    ma->last = ma_curr;
    
    return ma_curr/ma->n;
}

void Uint16ToTwoBytes(uint16 value, uint8* bytes)
{
    bytes[0] = (uint8) ((value & 0xFF00) >> 8);
    bytes[1] = (uint8) (value & 0x00FF);
}

void Uint32ToFourBytes(uint32 value, uint8* bytes)
{
    bytes[0] = (uint8) ((value & 0xFF000000) >> 24);
    bytes[1] = (uint8) ((value & 0x00FF0000) >> 16);
    bytes[2] = (uint8) ((value & 0x0000FF00) >> 8);
    bytes[3] = (uint8) (value & 0x000000FF);
}

void Int32ToFourBytes(int32 value, uint8* bytes)
{
    Uint32ToFourBytes((uint32)value, bytes);
}

void FloatToFourBytes(float value, uint8* bytes)
{
    uint8 *p_bytes = (uint8 *) &value;
    
    bytes[0] = p_bytes[0];
    bytes[1] = p_bytes[1];
    bytes[2] = p_bytes[2];
    bytes[3] = p_bytes[3];
}

uint16 TwoBytesToUint16(uint8* bytes)
{
    uint16 upper = bytes[0] << 8;
    uint16 lower = bytes[1];
    return upper + lower;
}

int16 TwoBytesInt16(uint8* bytes)
{
    return (int16) TwoBytesToUint16(bytes);
}

uint32 FourBytesToUint32(uint8* bytes)
{
    return (uint32) ( (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3] );
}

int32 FourBytesToInt32(uint8* bytes)
{
    return (int32) FourBytesToUint32(bytes);
}

float FourBytesToFloat(uint8 *bytes)
{   
    float value = *((float *) bytes);    
    return value;
}

#ifdef USE_FTOA
static int insert_zeros(int value, char *str, int offset, int precision)
{
    int num_digits = 0;
    
    // Count digits in number
    while (value)
    {
        value /= 10;
        num_digits++;
    }
    
    // If num_digits is less than precision then insert zeros
    while (num_digits < precision)
    {
        str[offset] = '0';
        offset++;
        num_digits++;
    }
        
    return offset;
}

// Converts a floating point number to string.
void ftoa(float n, char *str, int precision)
{   
    int i = 0;
    
    if (n < 0)
    {
        str[i] = '-';
        i++;
        n = abs(n);
    }
    
    // Extract integer part
    int ipart = (int)n;

    // convert integer part to string
    i += strlen(itoa(ipart, &str[i], 10));
    
    // check for display option after point
    if (precision > 0)
    {
        str[i] = '.';  // add dot
        i++;
 
        int fpart = (int) ( (n - (float)ipart) * pow(10, precision) );
        
        i = insert_zeros(fpart, str, i, precision);
        
        itoa( (int)fpart, &str[i], 10);
    }
}
#endif

void BinaryRangeSearch(int32 search, int32 *data_points, uint8 num_points, uint8 *lower_index, uint8 *upper_index)
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
    uint8 first = 0;
    uint8 last = num_points - 1;
    uint8 middle = (first+last)/2;
    
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
        }
 
        middle = (first + last)/2;
   }
}

void UniToDiff(float linear, float angular, float *left, float *right)
/*
    Vr = (2*V + W*L)/(2*R)
    Vl = (2*V + W*L)/(2*R)
    where 
        Vr - the velocity of the right wheel
        Vl - the velocity of the left wheel
        V - the linear velocity
        W - the angular velocity
        L - the separation of the left and right wheels
        R - the radius of the wheel
*/
{
    /*
    *left_cmd_velocity = linear - (angular * TRACK_WIDTH)/2;
    *right_cmd_velocity = linear + (angular * TRACK_WIDTH)/2;
    */
    *left = (2*linear - angular*TRACK_WIDTH)/WHEEL_DIAMETER;
    *right = (2*linear + angular*TRACK_WIDTH)/WHEEL_DIAMETER;
}

void DiffToUni(float left, float right, float *linear, float *angular)
/*
    V = R/2 * (Vr + Vl)
    W = R/L * (Vr - Vl)
    where
        V - the linear velocity
        W - the angular velocity
        L - the separation of the left and right wheels
        R - the radius of the wheel
        Vr - the velocity of the right wheel
        Vl - the velocity of the left wheel
*/
{
    *linear = WHEEL_RADIUS * (right + left) / 2;
    *angular = WHEEL_RADIUS * (right - left) / TRACK_WIDTH;
    
}

int16 Interpolate(int16 x, int16 x1, int16 x2, uint16 y1, uint16 y2)
{
    /* Y = ( ( X - X1 )( Y2 - Y1) / ( X2 - X1) ) + Y1 */

    /* We are not guaranteed to not have duplicates in the array.
       Handle the cases where the values may be equal so we don't divide by zero and so we return a reasonble pwm value.
     */
    if (x1 == x2)
    {
        if (y2 == y1)
        {
            return y1;
        }
        else
        {
            return (y2 - y1)/2 + y1;
        }
    }
    
    return ((x - x1)*((int16) y2 - (int16) y1))/(x2 - x1) + (int16) y1;
}

#define MPS_TO_CPS(mps) (mps / METER_PER_COUNT)
#define CPS_TO_MPS(cps) (cps * METER_PER_COUNT)

/* [] END OF FILE */
