/*******************************************************************************
* File Name: Rear_ADC_SAR_theACLK.h
* Version 2.10
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_Rear_ADC_SAR_theACLK_H)
#define CY_CLOCK_Rear_ADC_SAR_theACLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_10 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void Rear_ADC_SAR_theACLK_Start(void) ;
void Rear_ADC_SAR_theACLK_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Rear_ADC_SAR_theACLK_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Rear_ADC_SAR_theACLK_StandbyPower(uint8 state) ;
void Rear_ADC_SAR_theACLK_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Rear_ADC_SAR_theACLK_GetDividerRegister(void) ;
void Rear_ADC_SAR_theACLK_SetModeRegister(uint8 modeBitMask) ;
void Rear_ADC_SAR_theACLK_ClearModeRegister(uint8 modeBitMask) ;
uint8 Rear_ADC_SAR_theACLK_GetModeRegister(void) ;
void Rear_ADC_SAR_theACLK_SetSourceRegister(uint8 clkSource) ;
uint8 Rear_ADC_SAR_theACLK_GetSourceRegister(void) ;
#if defined(Rear_ADC_SAR_theACLK__CFG3)
void Rear_ADC_SAR_theACLK_SetPhaseRegister(uint8 clkPhase) ;
uint8 Rear_ADC_SAR_theACLK_GetPhaseRegister(void) ;
#endif /* defined(Rear_ADC_SAR_theACLK__CFG3) */

#define Rear_ADC_SAR_theACLK_Enable()                       Rear_ADC_SAR_theACLK_Start()
#define Rear_ADC_SAR_theACLK_Disable()                      Rear_ADC_SAR_theACLK_Stop()
#define Rear_ADC_SAR_theACLK_SetDivider(clkDivider)         Rear_ADC_SAR_theACLK_SetDividerRegister(clkDivider, 1u)
#define Rear_ADC_SAR_theACLK_SetDividerValue(clkDivider)    Rear_ADC_SAR_theACLK_SetDividerRegister((clkDivider) - 1u, 1u)
#define Rear_ADC_SAR_theACLK_SetMode(clkMode)               Rear_ADC_SAR_theACLK_SetModeRegister(clkMode)
#define Rear_ADC_SAR_theACLK_SetSource(clkSource)           Rear_ADC_SAR_theACLK_SetSourceRegister(clkSource)
#if defined(Rear_ADC_SAR_theACLK__CFG3)
#define Rear_ADC_SAR_theACLK_SetPhase(clkPhase)             Rear_ADC_SAR_theACLK_SetPhaseRegister(clkPhase)
#define Rear_ADC_SAR_theACLK_SetPhaseValue(clkPhase)        Rear_ADC_SAR_theACLK_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(Rear_ADC_SAR_theACLK__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Rear_ADC_SAR_theACLK_CLKEN              (* (reg8 *) Rear_ADC_SAR_theACLK__PM_ACT_CFG)
#define Rear_ADC_SAR_theACLK_CLKEN_PTR          ((reg8 *) Rear_ADC_SAR_theACLK__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Rear_ADC_SAR_theACLK_CLKSTBY            (* (reg8 *) Rear_ADC_SAR_theACLK__PM_STBY_CFG)
#define Rear_ADC_SAR_theACLK_CLKSTBY_PTR        ((reg8 *) Rear_ADC_SAR_theACLK__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Rear_ADC_SAR_theACLK_DIV_LSB            (* (reg8 *) Rear_ADC_SAR_theACLK__CFG0)
#define Rear_ADC_SAR_theACLK_DIV_LSB_PTR        ((reg8 *) Rear_ADC_SAR_theACLK__CFG0)
#define Rear_ADC_SAR_theACLK_DIV_PTR            ((reg16 *) Rear_ADC_SAR_theACLK__CFG0)

/* Clock MSB divider configuration register. */
#define Rear_ADC_SAR_theACLK_DIV_MSB            (* (reg8 *) Rear_ADC_SAR_theACLK__CFG1)
#define Rear_ADC_SAR_theACLK_DIV_MSB_PTR        ((reg8 *) Rear_ADC_SAR_theACLK__CFG1)

/* Mode and source configuration register */
#define Rear_ADC_SAR_theACLK_MOD_SRC            (* (reg8 *) Rear_ADC_SAR_theACLK__CFG2)
#define Rear_ADC_SAR_theACLK_MOD_SRC_PTR        ((reg8 *) Rear_ADC_SAR_theACLK__CFG2)

#if defined(Rear_ADC_SAR_theACLK__CFG3)
/* Analog clock phase configuration register */
#define Rear_ADC_SAR_theACLK_PHASE              (* (reg8 *) Rear_ADC_SAR_theACLK__CFG3)
#define Rear_ADC_SAR_theACLK_PHASE_PTR          ((reg8 *) Rear_ADC_SAR_theACLK__CFG3)
#endif /* defined(Rear_ADC_SAR_theACLK__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Rear_ADC_SAR_theACLK_CLKEN_MASK         Rear_ADC_SAR_theACLK__PM_ACT_MSK
#define Rear_ADC_SAR_theACLK_CLKSTBY_MASK       Rear_ADC_SAR_theACLK__PM_STBY_MSK

/* CFG2 field masks */
#define Rear_ADC_SAR_theACLK_SRC_SEL_MSK        Rear_ADC_SAR_theACLK__CFG2_SRC_SEL_MASK
#define Rear_ADC_SAR_theACLK_MODE_MASK          (~(Rear_ADC_SAR_theACLK_SRC_SEL_MSK))

#if defined(Rear_ADC_SAR_theACLK__CFG3)
/* CFG3 phase mask */
#define Rear_ADC_SAR_theACLK_PHASE_MASK         Rear_ADC_SAR_theACLK__CFG3_PHASE_DLY_MASK
#endif /* defined(Rear_ADC_SAR_theACLK__CFG3) */

#endif /* CY_CLOCK_Rear_ADC_SAR_theACLK_H */


/* [] END OF FILE */
