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

/*---------------------------------------------------------------------------------------------------
 * Description
 *-------------------------------------------------------------------------------------------------*/
// Add a description of the module

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "encoder.h"
#include "utils.h"
#include "time.h"
//#include "i2c.h"
#include "diag.h"
#include "debug.h"
#include "cal.h"

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#ifdef  LEFT_ENC_DUMP_ENABLED     
#define LEFT_DUMP_ENC(enc)  if (DEBUG_LEFT_ENCODER_ENABLED) DumpEncoder(enc)
#else
#define LEFT_DUMP_ENC(enc)
#endif    

#ifdef RIGHT_ENC_DUMP_ENABLED
#define RIGHT_DUMP_ENC(enc)  if (DEBUG_RIGHT_ENCODER_ENABLED) DumpEncoder(enc)
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
    float dist;
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
    /* dist */              0.0,
    /* count ma */          {0, 0},
    /* cps ma */            {0, 0},
    /* get enc count */     Left_QuadDec_GetCounter,
    /* set enc count */     Left_QuadDec_SetCounter,
};

static ENCODER_TYPE right_enc = {
    /* name */              "right",
    /* count/speed */       0, 0, 0, 0, 0, 0,
    /* dist */              0.0,
    /* count ma */          {0, 0},
    /* cps ma */            {0, 0},
    /* get enc count */     Right_QuadDec_GetCounter,
    /* set enc count */     Right_QuadDec_SetCounter,
};

static float linear_bias;

#if defined (LEFT_ENC_DUMP_ENABLED) || defined (RIGHT_ENC_DUMP_ENABLED)
static char avg_cps_str[10];
static char avg_delta_count_str[10];
static char avg_mps_str[10];
static char dist_str[10];
static char linear_bias_str[10];
/*---------------------------------------------------------------------------------------------------
 * Name: DumpEncoder
 * Description: Dumps the current state of the encoder to the serial port
 * Parameters: enc - encoder type (either left or right) for which data will be dumped
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
 static void DumpEncoder(ENCODER_TYPE *enc)
{
    ftoa(enc->avg_cps, avg_cps_str, 3);
    ftoa(enc->avg_delta_count, avg_delta_count_str, 3);
    ftoa(enc->avg_mps, avg_mps_str, 3);
    ftoa(enc->dist, dist_str, 3);
    ftoa(linear_bias, linear_bias_str, 3);

    DEBUG_PRINT_ARG("%s enc: %s %s %s %ld %s %s\r\n", 
                    enc->name, 
                    avg_cps_str, 
                    avg_mps_str, 
                    avg_delta_count_str, 
                    enc->delta_count, 
                    dist_str,
                    linear_bias_str);
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
    
    enc->dist += linear_bias * PI_D * enc->avg_delta_count/COUNT_PER_REVOLUTION;
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
    left_enc.dist = 0.0;
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
    right_enc.dist = 0.0;
    right_enc.delta_count_ma.last = 0;
    right_enc.delta_count_ma.n = 20;
    right_enc.avg_cps_ma.last = 0;
    right_enc.avg_cps_ma.n = 10;
    right_enc.avg_cps = 0;
    right_enc.avg_mps = 0;
    
    linear_bias = Cal_GetLinearBias();
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
    static uint32 last_update_time = 0;
    static uint32 delta_time;
    static uint8 enc_sched_offset_applied = 0;
    
    delta_time = millis() - last_update_time;
    ENC_DEBUG_DELTA(delta_time);
    if (delta_time >= ENC_SAMPLE_TIME_MS)
    {
        last_update_time = millis();
        
        APPLY_SCHED_OFFSET(ENC_SCHED_OFFSET, enc_sched_offset_applied);
        
        Encoder_Sample(&left_enc, delta_time);
        Encoder_Sample(&right_enc, delta_time);
        LEFT_DUMP_ENC(&left_enc);
        RIGHT_DUMP_ENC(&right_enc);
    }                    
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
 * Name: Encoder_LeftGetDist
 * Description: Returns the left wheel distance (in meters).
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_LeftGetDist()
{
    return left_enc.dist;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_RightGetDist
 * Description: Returns the right wheel distance (in meters).
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_RightGetDist()
{
    return right_enc.dist;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Encoder_GetDist
 * Description: Returns the distance (in meters) traveled by the center of the robot.
 * Parameters: None
 * Return: float
 * 
 *-------------------------------------------------------------------------------------------------*/
float Encoder_GetDist()
{
    return (left_enc.dist + right_enc.dist) / 2;
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
    left_enc.dist = 0;
    
    right_enc.write_counter(0);
    right_enc.last_count = 0;
    right_enc.delta_count = 0;
    right_enc.avg_delta_count = 0;
    right_enc.delta_count_ma.last = 0;
    right_enc.avg_cps = 0;
    right_enc.avg_mps = 0;
    right_enc.avg_cps_ma.last = 0;
    right_enc.count = 0;
    right_enc.dist = 0;
    
    /* If linear calibration has not been done then set the scalar to 1.0; otherwise, there will be no distance
       calculation.
     */
    linear_bias = Cal_GetLinearBias();
    
    LEFT_DUMP_ENC(&left_enc);
    RIGHT_DUMP_ENC(&right_enc);    
}

/* [] END OF FILE */
