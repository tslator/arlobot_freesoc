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

#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "encoder.h"
#include "utils.h"
#include "time.h"
#include "i2c.h"
#include "diag.h"
#include "debug.h"

#ifdef  LEFT_ENC_DUMP_ENABLED     
#define LEFT_DUMP_ENC(enc)  if (debug_control_enabled & DEBUG_LEFT_ENCODER_ENABLE_BIT) DumpEncoder(enc)
#else
#define LEFT_DUMP_ENC(enc)
#endif    

#ifdef RIGHT_ENC_DUMP_ENABLED
#define RIGHT_DUMP_ENC(enc)  if (debug_control_enabled & DEBUG_RIGHT_ENCODER_ENABLE_BIT) DumpEncoder(enc)
#else
#define RIGHT_DUMP_ENC(enc)
#endif

#ifdef ENC_UPDATE_DELTA_ENABLED
#define ENC_DEBUG_DELTA(delta) DEBUG_DELTA_TIME("enc", delta)
#else
#define ENC_DEBUG_DELTA(delta)
#endif    

#define ENC_SAMPLE_TIME_MS  SAMPLE_TIME_MS(ENC_SAMPLE_RATE)
#define ENC_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(ENC_SAMPLE_RATE)


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

#if defined (LEFT_ENC_DUMP_ENABLED) || defined (RIGHT_ENC_DUMP_ENABLED)
static void DumpEncoder(ENCODER_TYPE *enc)
{
    char avg_cps_str[10];
    char avg_delta_count_str[10];
    char avg_mps_str[10];
    char dist_str[10];
    
    ftoa(enc->avg_cps, avg_cps_str, 3);
    ftoa(enc->avg_delta_count, avg_delta_count_str, 3);
    ftoa(enc->avg_mps, avg_mps_str, 3);
    ftoa(enc->dist, dist_str, 3);
    
    if (ENCODER_DEBUG_CONTROL_ENABLED)
    {
        DEBUG_PRINT_ARG("%s enc: %s %s %s %ld %s\r\n", enc->name, avg_cps_str, avg_mps_str, avg_delta_count_str, enc->delta_count, dist_str);
    }
}
#endif

static void Encoder_Sample(ENCODER_TYPE *enc, uint32 delta_time)
{
    enc->count = (int32) enc->read_counter();
    enc->delta_count = enc->count - enc->last_count;
    enc->last_count = enc->count;
    
    enc->avg_delta_count = MovingAverageFloat(&enc->delta_count_ma, enc->delta_count);
    
    float cps = ((enc->avg_delta_count * MS_IN_SEC) / (float) delta_time);
    
    enc->avg_cps = MovingAverageFloat(&enc->avg_cps_ma, cps);
    
    enc->avg_mps = enc->avg_cps * METER_PER_COUNT;
    
    enc->dist += PI_D * enc->avg_delta_count/COUNT_PER_REVOLUTION;
}

void Encoder_Init()
{
    left_enc.count = 0;
    left_enc.last_count = 0;
    left_enc.delta_count = 0;
    left_enc.avg_delta_count = 0;
    left_enc.dist = 0.0;
    left_enc.delta_count_ma.last = 0;
    left_enc.delta_count_ma.n = 3;
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
    right_enc.delta_count_ma.n = 3;
    right_enc.avg_cps_ma.last = 0;
    right_enc.avg_cps_ma.n = 10;
    right_enc.avg_cps = 0;
    right_enc.avg_mps = 0;
    
}

void Encoder_Start()
{
    Left_QuadDec_Start();
    Right_QuadDec_Start();
    left_enc.write_counter(0);
    right_enc.write_counter(0);
}

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

float Encoder_LeftGetCntsPerSec()
{
    return left_enc.avg_cps;
}

float Encoder_RightGetCntsPerSec()
{
    return right_enc.avg_cps;
}

float Encoder_GetCntsPerSec()
{
    return (left_enc.avg_cps + right_enc.avg_cps) / 2;
}

float Encoder_LeftGetMeterPerSec()
{
    return left_enc.avg_mps;
}

float Encoder_RightGetMeterPerSec()
{
    return right_enc.avg_mps;
}

float Encoder_GetMeterPerSec()
{
    return (left_enc.avg_mps + right_enc.avg_mps) / 2;
}

int32 Encoder_LeftGetCount()
{
    return left_enc.read_counter();
}

int32 Encoder_RightGetCount()
{
    return right_enc.read_counter();
}

float Encoder_LeftGetDeltaCount()
{
    return left_enc.avg_delta_count;
}

float Encoder_RightGetDeltaCount()
{
    return right_enc.avg_delta_count;
}

float Encoder_LeftGetDist()
{
    return left_enc.dist;
}

float Encoder_RightGetDist()
{
    return right_enc.dist;
}

float Encoder_GetDist()
{
    return (left_enc.dist + right_enc.dist) / 2;
}

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
    
    right_enc.write_counter(0);
    right_enc.last_count = 0;
    right_enc.delta_count = 0;
    right_enc.avg_delta_count = 0;
    right_enc.delta_count_ma.last = 0;
    right_enc.avg_cps = 0;
    right_enc.avg_mps = 0;
    right_enc.avg_cps_ma.last = 0;
    right_enc.count = 0;
    
    LEFT_DUMP_ENC(&left_enc);
    RIGHT_DUMP_ENC(&right_enc);    
}

/* [] END OF FILE */
