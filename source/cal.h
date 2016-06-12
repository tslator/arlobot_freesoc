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

#ifndef CAL_H
#define CAL_H

#include <project.h>
    
#define CAL_NUM_SAMPLES (51)
#define CAL_SCALE_FACTOR (100)
    
#define CAL_DATA_SIZE (CAL_NUM_SAMPLES)

typedef struct _CAL_DATA_TYPE
{
    int32  cps_min;
    int32  cps_max;
    int    cps_scale;
    int32  cps_data[CAL_DATA_SIZE];
    uint16 pwm_data[CAL_DATA_SIZE];
    uint16 reserved; // Added to force 16 byte alignment
} __attribute__ ((packed)) CAL_DATA_TYPE;
       
void Cal_Init();
void Cal_Start();
void Cal_Update();
void Cal_Validate();

void Cal_LeftGetCalData(CAL_DATA_TYPE *fwd_cal_data, CAL_DATA_TYPE *bwd_cal_data);
void Cal_RightGetCalData(CAL_DATA_TYPE *fwd_cal_data, CAL_DATA_TYPE *bwd_cal_data);

    
#endif

/* [] END OF FILE */
