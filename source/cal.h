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
void Cal_Init();
void Cal_Start();
void Cal_Update();
void Cal_Upload();
void Cal_Download();
void Cal_Validate();
    
#endif

/* [] END OF FILE */
