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
    bytes[0] = (uint8) (value & 0xFF00) >> 8;
    bytes[1] = (uint8) (value & 0x00FF);
}

void Uint32ToFourBytes(uint32 value, uint8* bytes)
{
    bytes[0] = (value & 0xFF000000) >> 24;
    bytes[1] = (value & 0x00FF0000) >> 16;
    bytes[2] = (value & 0x0000FF00) >> 8;
    bytes[3] = (value & 0x000000FF);
}

void Int32ToFourBytes(int32 value, uint8* bytes)
{
    Uint32ToFourBytes((uint32)value, bytes);
}

void FloatToFourBytes(float value, uint8* bytes)
{
    Uint32ToFourBytes(*((uint32*)&value), bytes);
}

uint16 TwoBytesToUint16(uint8* bytes)
{
    return (uint16) ( (bytes[0] << 8) + bytes[1] );
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

/* [] END OF FILE */
