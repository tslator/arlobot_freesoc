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
   Description: This module provides the implementation for reading the encoders and calculating
   motor speed and distance.
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "encoder.h"
#include "utils.h"
#include "time.h"
#include "diag.h"
#include "debug.h"
#include "cal.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef  LEFT_ENC_DUMP_ENABLED     
#define LEFT_DUMP_ENC(enc)  if (Debug_IsEnabled(DEBUG_LEFT_ENCODER_ENABLE_BIT)) DumpEncoder(enc)
#else
#define LEFT_DUMP_ENC(enc)
#endif    

#ifdef RIGHT_ENC_DUMP_ENABLED
#define RIGHT_DUMP_ENC(enc)  if (Debug_IsEnabled(DEBUG_RIGHT_ENCODER_ENABLE_BIT)) DumpEncoder(enc)
#else
#define RIGHT_DUMP_ENC(enc)
#endif

#ifdef ENC_UPDATE_DELTA_ENABLED
#define ENC_DEBUG_DELTA(delta) DEBUG_DELTA_TIME("enc", delta)
#else
#define ENC_DEBUG_DELTA(delta)
#endif    

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
#define ENC_SAMPLE_TIME_MS  SAMPLE_TIME_MS(ENC_SAMPLE_RATE)
#define ENC_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(ENC_SAMPLE_RATE)


/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/    
typedef int32 (*READ_ENCODER_COUNTER_TYPE)();
typedef void (*WRITE_ENCODER_COUNTER_TYPE)(int32);
typedef void (*WRITE_OUTPUT_TYPE)(int16 mmps);

typedef struct _encoder_tag
{
    char name[6];
    int32 count;
    int32 last_count;
    int32 delta_count;
    float avg_delta_count;
    float avg_cps;
    float avg_mps;
    float delta_dist;
    MOVING_AVERAGE_FLOAT_TYPE delta_count_ma;
    MOVING_AVERAGE_FLOAT_TYPE avg_cps_ma;
    READ_ENCODER_COUNTER_TYPE read_counter;
    WRITE_ENCODER_COUNTER_TYPE write_counter;
} ENCODER_TYPE;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static ENCODER_TYPE left_enc = {
    /* name */              "left",
    /* count/speed */       0, 0, 0, 0, 0, 0,
    /* delta_dist */        0.0,
    /* count ma */          {0, 0},
    /* cps ma */            {0, 0},
    /* get enc count */     Left_QuadDec_GetCounter,
    /* set enc count */     Left_QuadDec_SetCounter,
};

static ENCODER_TYPE right_enc = {
    /* name */              "right",
    /* count/speed */       0, 0, 0, 0, 0, 0,
    /* delta dist */        0.0,
    /* count ma */          {0, 0},
    /* cps ma */            {0, 0},
    /* get enc count */     Right_QuadDec_GetCounter,
    /* set enc count */     Right_QuadDec_SetCounter,
};

#if defined (LEFT_ENC_DUMP_ENABLED) || defined (RIGHT_ENC_DUMP_ENABLED)
/*---------------------------------------------------------------------------------------------------
 * Name: DumpEncoder
 * Description: Dumps the current state of the encoder to the serial port
 * Parameters: enc - encoder type (either left or right) for which data will be dumped
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 static void DumpEncoder(ENCODER_TYPE *enc)
{
    DEBUG_PRINT_ARG("%s enc: %.3f %.3f %.3f %ld %.3f\r\n", 
                    enc->name, 
                    enc->avg_cps, 
                    enc->avg_mps, 
                    enc->avg_delta_count, 
                    enc->delta_count, 
                    enc->delta_dist);
}
#endif

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_Sample
 * Description: Updates the relevant encoder fields.  The function is called at the sampling
 *              rate defined for the encoder.
 * Parameters: enc - encoder type (either left or right) for which data will be dumped
 *             delta_time - number of milliseconds since the last call
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
static void Encoder_Sample(ENCODER_TYPE *enc, uint32 delta_time)
{
    enc->count = (int32) enc->read_counter();
    enc->delta_count = enc->count - enc->last_count;
    enc->last_count = enc->count;
    
    enc->avg_delta_count = MovingAverageFloat(&enc->delta_count_ma, enc->delta_count);
    
    float cps = ((enc->avg_delta_count * MS_IN_SEC) / (float) delta_time);
    
    enc->avg_cps = MovingAverageFloat(&enc->avg_cps_ma, cps);
    
    enc->avg_mps = enc->avg_cps * METER_PER_COUNT;
    
    enc->delta_dist = PI_D * enc->avg_delta_count/COUNT_PER_REVOLUTION;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_Init
 * Description: Initializes the encoder structures
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Encoder_Init()
{
    left_enc.count = 0;
    left_enc.last_count = 0;
    left_enc.delta_count = 0;
    left_enc.avg_delta_count = 0;
    left_enc.delta_dist = 0.0;
    left_enc.delta_count_ma.last = 0;
    left_enc.delta_count_ma.n = 20;
    left_enc.avg_cps_ma.last = 0;
    left_enc.avg_cps_ma.n = 10;
    left_enc.avg_cps = 0;
    left_enc.avg_mps = 0;
    
    right_enc.count = 0;
    right_enc.last_count = 0;
    right_enc.delta_count = 0;
    right_enc.avg_delta_count = 0;
    right_enc.delta_dist = 0.0;
    right_enc.delta_count_ma.last = 0;
    right_enc.delta_count_ma.n = 20;
    right_enc.avg_cps_ma.last = 0;
    right_enc.avg_cps_ma.n = 10;
    right_enc.avg_cps = 0;
    right_enc.avg_mps = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_Start
 * Description: Starts the left/right quadrature encoder components and initializes the counter.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Encoder_Start()
{
    Left_QuadDec_Start();
    Right_QuadDec_Start();
    left_enc.write_counter(0);
    right_enc.write_counter(0);
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_Update
 * Description: Updates the encoder calculations and fields.  This routine is called from the main
 *              loop.  Internally, it enforces the encoder sampling rate.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Encoder_Update()
{
    static uint32 last_update_time = ENC_SCHED_OFFSET;
    static uint32 delta_time;
    
    ENCODER_UPDATE_START();
    
    delta_time = millis() - last_update_time;
    ENC_DEBUG_DELTA(delta_time);
    if (delta_time >= ENC_SAMPLE_TIME_MS)
    {
        last_update_time = millis();
        
        Encoder_Sample(&left_enc, delta_time);
        Encoder_Sample(&right_enc, delta_time);
        LEFT_DUMP_ENC(&left_enc);
        RIGHT_DUMP_ENC(&right_enc);
    }                    
    
    ENCODER_UPDATE_END();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_LeftGetCntsPerSec
 * Description: Returns the average left encoder count/second
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_LeftGetCntsPerSec()
{
    return left_enc.avg_cps;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_RightGetCntsPerSec
 * Description: Returns the average right encoder count/second
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_RightGetCntsPerSec()
{
    return right_enc.avg_cps;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_GetCntsPerSec
 * Description: Returns the average center count/second.  Note, this is the count/second as viewed
 *              from the center point of the robot between the left and right wheels.
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_GetCntsPerSec()
{
    return (left_enc.avg_cps + right_enc.avg_cps) / 2;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_LeftGetMeterPerSec
 * Description: Returns the average left meter/second.
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_LeftGetMeterPerSec()
{
    return left_enc.avg_mps;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_RightGetMeterPerSec
 * Description: Returns the average right meter/second.
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_RightGetMeterPerSec()
{
    return right_enc.avg_mps;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_GetMeterPerSec
 * Description: Returns the average center meter/second.  Note, this is the meter/second as viewed
 *              from the center point of the robot between the left and right wheels.
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_GetMeterPerSec()
{
    return (left_enc.avg_mps + right_enc.avg_mps) / 2;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_LeftGetCount
 * Description: Returns the the left encoder count.
 * Parameters: None
 * Return: int32
 * 
 *-------------------------------------------------------------------------------------------------*/
int32 Encoder_LeftGetCount()
{
    return left_enc.read_counter();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_RightGetCount
 * Description: Returns the right encoder count.
 * Parameters: None
 * Return: int32
 * 
 *-------------------------------------------------------------------------------------------------*/
int32 Encoder_RightGetCount()
{
    return right_enc.read_counter();
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_LeftGetDeltaCount
 * Description: Returns the left encoder average delta count.
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_LeftGetDeltaCount()
{
    return left_enc.avg_delta_count;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_RightGetDeltaCount
 * Description: Returns the right encoder average delta count.
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_RightGetDeltaCount()
{
    return right_enc.avg_delta_count;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_LeftGetDeltaDist
 * Description: Returns the left wheel distance (in meters).
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_LeftGetDeltaDist()
{
    return left_enc.delta_dist;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_RightGetDeltaDist
 * Description: Returns the right wheel distance (in meters).
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_RightGetDeltaDist()
{
    return right_enc.delta_dist;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_GetCenterDist
 * Description: Returns the distance (in meters) traveled by the center of the robot.
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_GetCenterDist()
{
    return (left_enc.delta_dist + right_enc.delta_dist) / 2;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_Reset
 * Description: Resets the left/right encoder fields.
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Encoder_Reset()
{
    left_enc.write_counter(0);
    left_enc.last_count = 0;
    left_enc.delta_count = 0;
    left_enc.avg_delta_count = 0;
    left_enc.delta_count_ma.last = 0;
    left_enc.avg_cps = 0;
    left_enc.avg_mps = 0;
    left_enc.avg_cps_ma.last = 0;
    left_enc.count = 0;
    left_enc.delta_dist = 0;
    
    right_enc.write_counter(0);
    right_enc.last_count = 0;
    right_enc.delta_count = 0;
    right_enc.avg_delta_count = 0;
    right_enc.delta_count_ma.last = 0;
    right_enc.avg_cps = 0;
    right_enc.avg_mps = 0;
    right_enc.avg_cps_ma.last = 0;
    right_enc.count = 0;
    right_enc.delta_dist = 0;
    
    LEFT_DUMP_ENC(&left_enc);
    RIGHT_DUMP_ENC(&right_enc);    
}

/* [] END OF FILE */
