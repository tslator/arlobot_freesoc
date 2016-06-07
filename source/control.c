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

#include "control.h"
#include "i2c.h"
#include "encoder.h"
#include "motor.h"
#include "cal.h"

/* The purpose of this module is to handle control changes to the system.
 */

void Control_Init()
{
}

void Control_Start()
{    
}

void Control_Update()
{
    uint16 control;
    
    /* Clear Encoder Counts */
    control = I2c_ReadControl();
    
    if (control & CONTROL_CLEAR_ENCODER_COUNT_BIT)
    {
        Encoder_Reset();
    }    

    /* Perform calibration */
    if (control & CONTROL_ENABLE_CALIBRATION_BIT)
    {
        Cal_Update();
    }
    
    /* Upload calibration values */
    if (control & CONTROL_UPLOAD_CALIBRATION_BIT)
    {
        Cal_Upload();
    }

    if (control & CONTROL_DOWNLOAD_CALIBRATION_BIT)
    {
        Cal_Download();
    }
    
    if (control & CONTROL_VALIDATE_CALIBRATION_BIT)
    {
        Cal_Validate();
    }
    
}

/* [] END OF FILE */
