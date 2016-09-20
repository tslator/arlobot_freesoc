/*******************************************************************************
* File Name: `$INSTANCE_NAME`.c  
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides the source code to the API for the Quadrature Decoder
*  component.
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

#if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
    #include "`$INSTANCE_NAME`_PVT.h"
#endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT */

uint8 `$INSTANCE_NAME`_initVar = 0u;


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default QuadDec configuration provided with customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Init(void) `=ReentrantKeil($INSTANCE_NAME . "_Init")`
{
    #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
        /* Disable Interrupt. */
        CyIntDisable(`$INSTANCE_NAME`_ISR_NUMBER);
        /* Set the ISR to point to the `$INSTANCE_NAME`_isr Interrupt. */
        (void) CyIntSetVector(`$INSTANCE_NAME`_ISR_NUMBER, & `$INSTANCE_NAME`_ISR);
        /* Set the priority. */
        CyIntSetPriority(`$INSTANCE_NAME`_ISR_NUMBER, `$INSTANCE_NAME`_ISR_PRIORITY);
    #endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Enable
********************************************************************************
*
* Summary:
*  This function enable interrupts from Component and also enable Component's
*  ISR in case of 32-bit counter.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Enable(void) `=ReentrantKeil($INSTANCE_NAME . "_Enable")`
{
    uint8 enableInterrupts;

    `$INSTANCE_NAME`_SetInterruptMask(`$INSTANCE_NAME`_INIT_INT_MASK);

    /* Clear pending interrupts. */
    (void) `$INSTANCE_NAME`_GetEvents();
    
    enableInterrupts = CyEnterCriticalSection();

    /* Enable interrupts from Statusi register */
    `$INSTANCE_NAME`_SR_AUX_CONTROL |= `$INSTANCE_NAME`_INTERRUPTS_ENABLE;

    CyExitCriticalSection(enableInterrupts);        

    #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
        /* Enable Component interrupts */
        CyIntEnable(`$INSTANCE_NAME`_ISR_NUMBER);
    #endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Start
********************************************************************************
*
* Summary:
*  Initializes UDBs and other relevant hardware.
*  Resets counter, enables or disables all relevant interrupts.
*  Starts monitoring the inputs and counting.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  `$INSTANCE_NAME`_initVar - used to check initial configuration, modified on
*  first function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Start(void) `=ReentrantKeil($INSTANCE_NAME . "_Start")`
{
    #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT)
        `$INSTANCE_NAME`_Cnt8_Start();
        `$INSTANCE_NAME`_Cnt8_WriteCounter(`$INSTANCE_NAME`_COUNTER_INIT_VALUE);
    #else
        /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT) || 
        *  (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT) 
        */
        `$INSTANCE_NAME`_Cnt16_Start();
        `$INSTANCE_NAME`_Cnt16_WriteCounter(`$INSTANCE_NAME`_COUNTER_INIT_VALUE);
    #endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT */
    
    #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)        
       `$INSTANCE_NAME`_count32SoftPart = 0;
    #endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT */

    if (`$INSTANCE_NAME`_initVar == 0u)
    {
        `$INSTANCE_NAME`_Init();
        `$INSTANCE_NAME`_initVar = 1u;
    }

    `$INSTANCE_NAME`_Enable();
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Stop
********************************************************************************
*
* Summary:
*  Turns off UDBs and other relevant hardware.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Stop(void) `=ReentrantKeil($INSTANCE_NAME . "_Stop")`
{
    uint8 enableInterrupts;

    #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT)
        `$INSTANCE_NAME`_Cnt8_Stop();
    #else 
        /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT) ||
        *  (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
        */
        `$INSTANCE_NAME`_Cnt16_Stop();    /* counter disable */
    #endif /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT) */
 
    enableInterrupts = CyEnterCriticalSection();

    /* Disable interrupts interrupts from Statusi register */
    `$INSTANCE_NAME`_SR_AUX_CONTROL &= (uint8) (~`$INSTANCE_NAME`_INTERRUPTS_ENABLE);

    CyExitCriticalSection(enableInterrupts);

    #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
        CyIntDisable(`$INSTANCE_NAME`_ISR_NUMBER);    /* interrupt disable */
    #endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_GetCounter
********************************************************************************
*
* Summary:
*  Reports the current value of the counter.
*
* Parameters:
*  None.
*
* Return:
*  The counter value. Return type is signed and per the counter size setting.
*  A positive value indicates clockwise movement (B before A).
*
* Global variables:
*  `$INSTANCE_NAME`_count32SoftPart - used to get hi 16 bit for current value
*  of the 32-bit counter, when Counter size equal 32-bit.
*
*******************************************************************************/
`$CounterSizeReplacementString` `$INSTANCE_NAME`_GetCounter(void) `=ReentrantKeil($INSTANCE_NAME . "_GetCounter")`
{
    `$CounterSizeReplacementString` count;
    `$CounterSizeReplacementStringUnsigned` tmpCnt;

    #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
        int16 hwCount;

        CyIntDisable(`$INSTANCE_NAME`_ISR_NUMBER);

        tmpCnt = `$INSTANCE_NAME`_Cnt16_ReadCounter();
        hwCount = (int16) ((int32) tmpCnt - (int32) `$INSTANCE_NAME`_COUNTER_INIT_VALUE);
        count = `$INSTANCE_NAME`_count32SoftPart + hwCount;

        CyIntEnable(`$INSTANCE_NAME`_ISR_NUMBER);
    #else 
        /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT) || 
        *  (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT)
        */
        #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT)
            tmpCnt = `$INSTANCE_NAME`_Cnt8_ReadCounter();
        #else /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT) */
            tmpCnt = `$INSTANCE_NAME`_Cnt16_ReadCounter();
        #endif  /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT */

        count = (`$CounterSizeReplacementString`) ((`$TypeCastReplacementString`) tmpCnt -
                (`$TypeCastReplacementString`) `$INSTANCE_NAME`_COUNTER_INIT_VALUE);

    #endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT */ 

    return (count);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SetCounter
********************************************************************************
*
* Summary:
*  Sets the current value of the counter.
*
* Parameters:
*  value:  The new value. Parameter type is signed and per the counter size
*  setting.
*
* Return:
*  None.
*
* Global variables:
*  `$INSTANCE_NAME`_count32SoftPart - modified to set hi 16 bit for current
*  value of the 32-bit counter, when Counter size equal 32-bit.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SetCounter(`$CounterSizeReplacementString` value) `=ReentrantKeil($INSTANCE_NAME . "_SetCounter")`
{
    #if ((`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT) || \
         (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT))
        `$CounterSizeReplacementStringUnsigned` count;
        
        if (value >= 0)
        {
            count = (`$CounterSizeReplacementStringUnsigned`) value + `$INSTANCE_NAME`_COUNTER_INIT_VALUE;
        }
        else
        {
            count = `$INSTANCE_NAME`_COUNTER_INIT_VALUE - (`$CounterSizeReplacementStringUnsigned`)(-value);
        }
        #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT)
            `$INSTANCE_NAME`_Cnt8_WriteCounter(count);
        #else /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT) */
            `$INSTANCE_NAME`_Cnt16_WriteCounter(count);
        #endif  /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT */
    #else /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT) */
        CyIntDisable(`$INSTANCE_NAME`_ISR_NUMBER);

        `$INSTANCE_NAME`_Cnt16_WriteCounter(`$INSTANCE_NAME`_COUNTER_INIT_VALUE);
        `$INSTANCE_NAME`_count32SoftPart = value;

        CyIntEnable(`$INSTANCE_NAME`_ISR_NUMBER);
    #endif  /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT) ||
             * (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT)
             */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_GetEvents
********************************************************************************
* 
* Summary:
*   Reports the current status of events. This function clears the bits of the 
*   status register.
*
* Parameters:
*  None.
*
* Return:
*  The events, as bits in an unsigned 8-bit value:
*    Bit      Description
*     0        Counter overflow.
*     1        Counter underflow.
*     2        Counter reset due to index, if index input is used.
*     3        Invalid A, B inputs state transition.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_GetEvents(void) `=ReentrantKeil($INSTANCE_NAME . "_GetEvents")`
{
    return (`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_INIT_INT_MASK);
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SetInterruptMask
********************************************************************************
*
* Summary:
*  Enables / disables interrupts due to the events.
*  For the 32-bit counter, the overflow, underflow and reset interrupts cannot
*  be disabled, these bits are ignored.
*
* Parameters:
*  mask: Enable / disable bits in an 8-bit value, where 1 enables the interrupt.
*
* Return:
*  None.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SetInterruptMask(uint8 mask) `=ReentrantKeil($INSTANCE_NAME . "_SetInterruptMask")`
{
    #if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
        /* Underflow, Overflow and Reset interrupts for 32-bit Counter are always enable */
        mask |= (`$INSTANCE_NAME`_COUNTER_OVERFLOW | `$INSTANCE_NAME`_COUNTER_UNDERFLOW |
                 `$INSTANCE_NAME`_COUNTER_RESET);
    #endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT */

    `$INSTANCE_NAME`_STATUS_MASK = mask;
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_GetInterruptMask
********************************************************************************
*
* Summary:
*  Reports the current interrupt mask settings.
*
* Parameters:
*  None.
*
* Return:
*  Enable / disable bits in an 8-bit value, where 1 enables the interrupt.
*  For the 32-bit counter, the overflow, underflow and reset enable bits are
*  always set.
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_GetInterruptMask(void) `=ReentrantKeil($INSTANCE_NAME . "_GetInterruptMask")`
{
    return (`$INSTANCE_NAME`_STATUS_MASK & `$INSTANCE_NAME`_INIT_INT_MASK);
}


/* [] END OF FILE */
