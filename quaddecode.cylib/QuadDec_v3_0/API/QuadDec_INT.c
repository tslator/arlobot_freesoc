/*******************************************************************************
* File Name: `$INSTANCE_NAME`_INT.c
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
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

#include "`$INSTANCE_NAME`.h"
#include "`$INSTANCE_NAME`_PVT.h"
`$CY_API_CALLBACK_HEADER_INCLUDE`

volatile int32 `$INSTANCE_NAME`_count32SoftPart = 0;


/*******************************************************************************
* FUNCTION NAME: void `$INSTANCE_NAME`_ISR
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
*  `$INSTANCE_NAME`_count32SoftPart - modified to update hi 16 bit for current
*  value of the 32-bit counter, when Counter size equal 32-bit.
*  `$INSTANCE_NAME`_swStatus - modified with the updated values of STATUS
*  register.
*
*******************************************************************************/
CY_ISR( `$INSTANCE_NAME`_ISR )
{
   uint8 `$INSTANCE_NAME`_swStatus;

   `$INSTANCE_NAME`_swStatus = `$INSTANCE_NAME`_STATUS_REG;

    #ifdef `$INSTANCE_NAME`_ISR_ENTRY_CALLBACK
        `$INSTANCE_NAME`_ISR_EntryCallback();
    #endif /* `$INSTANCE_NAME`_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START `$INSTANCE_NAME`_ISR_START` */

    /* `#END` */

    if (0u != (`$INSTANCE_NAME`_swStatus & `$INSTANCE_NAME`_COUNTER_OVERFLOW))
    {
        `$INSTANCE_NAME`_count32SoftPart += (int32) `$INSTANCE_NAME`_COUNTER_MAX_VALUE;
    }
    else if (0u != (`$INSTANCE_NAME`_swStatus & `$INSTANCE_NAME`_COUNTER_UNDERFLOW))
    {
        `$INSTANCE_NAME`_count32SoftPart -= (int32) `$INSTANCE_NAME`_COUNTER_INIT_VALUE;
    }
    else
    {
        /* Nothing to do here */
    }
    
    if (0u != (`$INSTANCE_NAME`_swStatus & `$INSTANCE_NAME`_COUNTER_RESET))
    {
        `$INSTANCE_NAME`_count32SoftPart = 0;
    }
    
    /* User code required at end of ISR */
    /* `#START `$INSTANCE_NAME`_ISR_END` */

    /* `#END` */

    #ifdef `$INSTANCE_NAME`_ISR_EXIT_CALLBACK
        `$INSTANCE_NAME`_ISR_ExitCallback();
    #endif /* `$INSTANCE_NAME`_ISR_EXIT_CALLBACK */
}


/* [] END OF FILE */
