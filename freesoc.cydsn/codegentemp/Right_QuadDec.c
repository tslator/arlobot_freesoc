/*******************************************************************************
* File Name: Right_QuadDec.c  
* Version 3.0
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

#include "Right_QuadDec.h"

#if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT)
    #include "Right_QuadDec_PVT.h"
#endif /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT */

uint8 Right_QuadDec_initVar = 0u;


/*******************************************************************************
* Function Name: Right_QuadDec_Init
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
void Right_QuadDec_Init(void) 
{
    #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT)
        /* Disable Interrupt. */
        CyIntDisable(Right_QuadDec_ISR_NUMBER);
        /* Set the ISR to point to the Right_QuadDec_isr Interrupt. */
        (void) CyIntSetVector(Right_QuadDec_ISR_NUMBER, & Right_QuadDec_ISR);
        /* Set the priority. */
        CyIntSetPriority(Right_QuadDec_ISR_NUMBER, Right_QuadDec_ISR_PRIORITY);
    #endif /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: Right_QuadDec_Enable
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
void Right_QuadDec_Enable(void) 
{
    uint8 enableInterrupts;

    Right_QuadDec_SetInterruptMask(Right_QuadDec_INIT_INT_MASK);

    /* Clear pending interrupts. */
    (void) Right_QuadDec_GetEvents();
    
    enableInterrupts = CyEnterCriticalSection();

    /* Enable interrupts from Statusi register */
    Right_QuadDec_SR_AUX_CONTROL |= Right_QuadDec_INTERRUPTS_ENABLE;

    CyExitCriticalSection(enableInterrupts);        

    #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT)
        /* Enable Component interrupts */
        CyIntEnable(Right_QuadDec_ISR_NUMBER);
    #endif /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: Right_QuadDec_Start
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
*  Right_QuadDec_initVar - used to check initial configuration, modified on
*  first function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Right_QuadDec_Start(void) 
{
    #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT)
        Right_QuadDec_Cnt8_Start();
        Right_QuadDec_Cnt8_WriteCounter(Right_QuadDec_COUNTER_INIT_VALUE);
    #else
        /* (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_16_BIT) || 
        *  (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT) 
        */
        Right_QuadDec_Cnt16_Start();
        Right_QuadDec_Cnt16_WriteCounter(Right_QuadDec_COUNTER_INIT_VALUE);
    #endif /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT */
    
    #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT)        
       Right_QuadDec_count32SoftPart = 0;
    #endif /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT */

    if (Right_QuadDec_initVar == 0u)
    {
        Right_QuadDec_Init();
        Right_QuadDec_initVar = 1u;
    }

    Right_QuadDec_Enable();
}


/*******************************************************************************
* Function Name: Right_QuadDec_Stop
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
void Right_QuadDec_Stop(void) 
{
    uint8 enableInterrupts;

    #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT)
        Right_QuadDec_Cnt8_Stop();
    #else 
        /* (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_16_BIT) ||
        *  (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT)
        */
        Right_QuadDec_Cnt16_Stop();    /* counter disable */
    #endif /* (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT) */
 
    enableInterrupts = CyEnterCriticalSection();

    /* Disable interrupts interrupts from Statusi register */
    Right_QuadDec_SR_AUX_CONTROL &= (uint8) (~Right_QuadDec_INTERRUPTS_ENABLE);

    CyExitCriticalSection(enableInterrupts);

    #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT)
        CyIntDisable(Right_QuadDec_ISR_NUMBER);    /* interrupt disable */
    #endif /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: Right_QuadDec_GetCounter
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
*  Right_QuadDec_count32SoftPart - used to get hi 16 bit for current value
*  of the 32-bit counter, when Counter size equal 32-bit.
*
*******************************************************************************/
int32 Right_QuadDec_GetCounter(void) 
{
    int32 count;
    uint16 tmpCnt;

    #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT)
        int16 hwCount;

        CyIntDisable(Right_QuadDec_ISR_NUMBER);

        tmpCnt = Right_QuadDec_Cnt16_ReadCounter();
        hwCount = (int16) ((int32) tmpCnt - (int32) Right_QuadDec_COUNTER_INIT_VALUE);
        count = Right_QuadDec_count32SoftPart + hwCount;

        CyIntEnable(Right_QuadDec_ISR_NUMBER);
    #else 
        /* (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT) || 
        *  (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_16_BIT)
        */
        #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT)
            tmpCnt = Right_QuadDec_Cnt8_ReadCounter();
        #else /* (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_16_BIT) */
            tmpCnt = Right_QuadDec_Cnt16_ReadCounter();
        #endif  /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT */

        count = (int32) ((int32) tmpCnt -
                (int32) Right_QuadDec_COUNTER_INIT_VALUE);

    #endif /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT */ 

    return (count);
}


/*******************************************************************************
* Function Name: Right_QuadDec_SetCounter
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
*  Right_QuadDec_count32SoftPart - modified to set hi 16 bit for current
*  value of the 32-bit counter, when Counter size equal 32-bit.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Right_QuadDec_SetCounter(int32 value) 
{
    #if ((Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT) || \
         (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_16_BIT))
        uint16 count;
        
        if (value >= 0)
        {
            count = (uint16) value + Right_QuadDec_COUNTER_INIT_VALUE;
        }
        else
        {
            count = Right_QuadDec_COUNTER_INIT_VALUE - (uint16)(-value);
        }
        #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT)
            Right_QuadDec_Cnt8_WriteCounter(count);
        #else /* (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_16_BIT) */
            Right_QuadDec_Cnt16_WriteCounter(count);
        #endif  /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT */
    #else /* (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT) */
        CyIntDisable(Right_QuadDec_ISR_NUMBER);

        Right_QuadDec_Cnt16_WriteCounter(Right_QuadDec_COUNTER_INIT_VALUE);
        Right_QuadDec_count32SoftPart = value;

        CyIntEnable(Right_QuadDec_ISR_NUMBER);
    #endif  /* (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_8_BIT) ||
             * (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_16_BIT)
             */
}


/*******************************************************************************
* Function Name: Right_QuadDec_GetEvents
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
uint8 Right_QuadDec_GetEvents(void) 
{
    return (Right_QuadDec_STATUS_REG & Right_QuadDec_INIT_INT_MASK);
}


/*******************************************************************************
* Function Name: Right_QuadDec_SetInterruptMask
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
void Right_QuadDec_SetInterruptMask(uint8 mask) 
{
    #if (Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT)
        /* Underflow, Overflow and Reset interrupts for 32-bit Counter are always enable */
        mask |= (Right_QuadDec_COUNTER_OVERFLOW | Right_QuadDec_COUNTER_UNDERFLOW |
                 Right_QuadDec_COUNTER_RESET);
    #endif /* Right_QuadDec_COUNTER_SIZE == Right_QuadDec_COUNTER_SIZE_32_BIT */

    Right_QuadDec_STATUS_MASK = mask;
}


/*******************************************************************************
* Function Name: Right_QuadDec_GetInterruptMask
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
uint8 Right_QuadDec_GetInterruptMask(void) 
{
    return (Right_QuadDec_STATUS_MASK & Right_QuadDec_INIT_INT_MASK);
}


/* [] END OF FILE */
