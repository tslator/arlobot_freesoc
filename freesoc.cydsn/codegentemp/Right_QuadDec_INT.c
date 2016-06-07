/*******************************************************************************
* File Name: Right_QuadDec_INT.c
* Version 3.0
*
* Description:
*  This file contains the Interrupt Service Routine (ISR) for the Quadrature
*  Decoder component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Right_QuadDec.h"
#include "Right_QuadDec_PVT.h"
#include "cyapicallbacks.h"

volatile int32 Right_QuadDec_count32SoftPart = 0;


/*******************************************************************************
* FUNCTION NAME: void Right_QuadDec_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for Quadrature Decoder Component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  Right_QuadDec_count32SoftPart - modified to update hi 16 bit for current
*  value of the 32-bit counter, when Counter size equal 32-bit.
*  Right_QuadDec_swStatus - modified with the updated values of STATUS
*  register.
*
*******************************************************************************/
CY_ISR( Right_QuadDec_ISR )
{
   uint8 Right_QuadDec_swStatus;

   Right_QuadDec_swStatus = Right_QuadDec_STATUS_REG;

    #ifdef Right_QuadDec_ISR_ENTRY_CALLBACK
        Right_QuadDec_ISR_EntryCallback();
    #endif /* Right_QuadDec_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START Right_QuadDec_ISR_START` */

    /* `#END` */

    if (0u != (Right_QuadDec_swStatus & Right_QuadDec_COUNTER_OVERFLOW))
    {
        Right_QuadDec_count32SoftPart += (int32) Right_QuadDec_COUNTER_MAX_VALUE;
    }
    else if (0u != (Right_QuadDec_swStatus & Right_QuadDec_COUNTER_UNDERFLOW))
    {
        Right_QuadDec_count32SoftPart -= (int32) Right_QuadDec_COUNTER_INIT_VALUE;
    }
    else
    {
        /* Nothing to do here */
    }
    
    if (0u != (Right_QuadDec_swStatus & Right_QuadDec_COUNTER_RESET))
    {
        Right_QuadDec_count32SoftPart = 0;
    }
    
    /* User code required at end of ISR */
    /* `#START Right_QuadDec_ISR_END` */

    /* `#END` */

    #ifdef Right_QuadDec_ISR_EXIT_CALLBACK
        Right_QuadDec_ISR_ExitCallback();
    #endif /* Right_QuadDec_ISR_EXIT_CALLBACK */
}


/* [] END OF FILE */
