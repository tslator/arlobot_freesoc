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

#ifdef  LEFT_ENC_DUMP_ENABLED
#define LEFT_DUMP_ENC(enc)  DumpEncoder(enc)
#else
#define LEFT_DUMP_ENC(enc)
#endif    

#ifdef RIGHT_ENC_DUMP_ENABLED
#define RIGHT_DUMP_ENC(enc)  DumpEncoder(enc)
#else
#define RIGHT_DUMP_ENC(enc)
#endif


#define ENC_SAMPLE_TIME_MS  SAMPLE_TIME_MS(ENC_SAMPLE_RATE)
#define ENC_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(ENC_SAMPLE_RATE)


typedef int32 (*READ_ENCODER_COUNTER_TYPE)();
typedef void (*WRITE_ENCODER_COUNTER_TYPE)(int32);
typedef void (*WRITE_OUTPUT_TYPE)(float cps, int16 mmps);

typedef struct _encoder_tag
{
    char name[6];
    int32 count;
    int32 last_count;
    int32 delta_count;
    float avg_delta_count;
    float avg_cps;
    float avg_mmps;
    MOVING_AVERAGE_FLOAT_TYPE delta_count_ma;
    MOVING_AVERAGE_FLOAT_TYPE avg_cps_ma;
    READ_ENCODER_COUNTER_TYPE read_counter;
    WRITE_ENCODER_COUNTER_TYPE write_counter;
    WRITE_OUTPUT_TYPE write_output;
} ENCODER_TYPE;

static ENCODER_TYPE left_enc = {
    /* name */              "left",
    /* count/speed */       0, 0, 0, 0, 0, 0,
    /* count ma */          {0, 0},
    /* cps ma */            {0, 0},
    /* get enc count */     Left_QuadDec_GetCounter,
    /* set enc count */     Left_QuadDec_SetCounter,
    /* set wheel speed */   I2c_LeftWriteOutput
};

static ENCODER_TYPE right_enc = {
    /* name */              "right",
    /* count/speed */       0, 0, 0, 0, 0, 0,
    /* count ma */          {0, 0},
    /* cps ma */            {0, 0},
    /* get enc count */     Right_QuadDec_GetCounter,
    /* set enc count */     Right_QuadDec_SetCounter,
    /* set wheel speed */   I2c_RightWriteOutput
};

#if defined (LEFT_ENC_DUMP_ENABLED) || defined (RIGHT_ENC_DUMP_ENABLED)
static void DumpEncoder(ENCODER_TYPE *enc)
{
    char avg_cps_str[10];
    char avg_delta_count_str[10];
    char avg_mmps_str[10];
    char output[64];
    
    ftoa(enc->avg_cps, avg_cps_str, 3);
    ftoa(enc->avg_delta_count, avg_delta_count_str, 3);
    ftoa(enc->avg_mmps, avg_mmps_str, 3);
    
    sprintf(output, "%s enc: %s %s %s %ld %ld %ld \r\n", enc->name, avg_cps_str, avg_delta_count_str, avg_mmps_str, enc->count, enc->last_count, enc->delta_count);
    UART_Debug_PutString(output);

    enc->write_output(enc->avg_cps, enc->avg_mmps);
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
    
    enc->avg_mmps = enc->avg_cps * MILLIMETER_PER_COUNT;
}

void Encoder_Init()
{
    left_enc.write_counter(0);
    left_enc.count = 0;
    left_enc.last_count = 0;
    left_enc.delta_count = 0;
    left_enc.avg_delta_count = 0;
    left_enc.delta_count_ma.last = 0;
    left_enc.delta_count_ma.n = 3;
    left_enc.avg_cps_ma.last = 0;
    left_enc.avg_cps_ma.n = 10;
    left_enc.avg_cps = 0;
    left_enc.avg_mmps = 0;
    
    right_enc.write_counter(0);
    right_enc.count = 0;
    right_enc.last_count = 0;
    right_enc.delta_count = 0;
    right_enc.avg_delta_count = 0;
    right_enc.delta_count_ma.last = 0;
    right_enc.delta_count_ma.n = 3;
    right_enc.avg_cps_ma.last = 0;
    right_enc.avg_cps_ma.n = 10;
    right_enc.avg_cps = 0;
    right_enc.avg_mmps = 0;
    
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

float Encoder_LeftGetMmPerSec()
{
    return left_enc.avg_mmps;
}

float Encoder_RightGetMmPerSec()
{
    return right_enc.avg_mmps;
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

void Encoder_Reset()
{
    left_enc.write_counter(0);
    left_enc.last_count = 0;
    left_enc.delta_count = 0;
    left_enc.avg_delta_count = 0;
    left_enc.delta_count_ma.last = 0;
    left_enc.avg_cps = 0;
    left_enc.avg_mmps = 0;
    left_enc.avg_cps_ma.last = 0;
    left_enc.count = 0;
    
    right_enc.write_counter(0);
    right_enc.last_count = 0;
    right_enc.delta_count = 0;
    right_enc.avg_delta_count = 0;
    right_enc.delta_count_ma.last = 0;
    right_enc.avg_cps = 0;
    right_enc.avg_mmps = 0;
    right_enc.avg_cps_ma.last = 0;
    right_enc.count = 0;
    
    LEFT_DUMP_ENC(&left_enc);
    RIGHT_DUMP_ENC(&right_enc);
    
}

/* [] END OF FILE */
