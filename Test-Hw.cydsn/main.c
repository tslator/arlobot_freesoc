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
#include <project.h>
#include "i2ctest.h"
#include "eepromtest.h"
#include "serialtest.h"



int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    //I2cTest();
    //EepromTest();
    UsbuartTest();
}

/* [] END OF FILE */
