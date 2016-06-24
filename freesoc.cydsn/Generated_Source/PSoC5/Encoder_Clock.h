/*******************************************************************************
* File Name: Encoder_Clock.h
* Version 2.20
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

#if !defined(CY_CLOCK_Encoder_Clock_H)
#define CY_CLOCK_Encoder_Clock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void Encoder_Clock_Start(void) ;
void Encoder_Clock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Encoder_Clock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Encoder_Clock_StandbyPower(uint8 state) ;
void Encoder_Clock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Encoder_Clock_GetDividerRegister(void) ;
void Encoder_Clock_SetModeRegister(uint8 modeBitMask) ;
void Encoder_Clock_ClearModeRegister(uint8 modeBitMask) ;
uint8 Encoder_Clock_GetModeRegister(void) ;
void Encoder_Clock_SetSourceRegister(uint8 clkSource) ;
uint8 Encoder_Clock_GetSourceRegister(void) ;
#if defined(Encoder_Clock__CFG3)
void Encoder_Clock_SetPhaseRegister(uint8 clkPhase) ;
uint8 Encoder_Clock_GetPhaseRegister(void) ;
#endif /* defined(Encoder_Clock__CFG3) */

#define Encoder_Clock_Enable()                       Encoder_Clock_Start()
#define Encoder_Clock_Disable()                      Encoder_Clock_Stop()
#define Encoder_Clock_SetDivider(clkDivider)         Encoder_Clock_SetDividerRegister(clkDivider, 1u)
#define Encoder_Clock_SetDividerValue(clkDivider)    Encoder_Clock_SetDividerRegister((clkDivider) - 1u, 1u)
#define Encoder_Clock_SetMode(clkMode)               Encoder_Clock_SetModeRegister(clkMode)
#define Encoder_Clock_SetSource(clkSource)           Encoder_Clock_SetSourceRegister(clkSource)
#if defined(Encoder_Clock__CFG3)
#define Encoder_Clock_SetPhase(clkPhase)             Encoder_Clock_SetPhaseRegister(clkPhase)
#define Encoder_Clock_SetPhaseValue(clkPhase)        Encoder_Clock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(Encoder_Clock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Encoder_Clock_CLKEN              (* (reg8 *) Encoder_Clock__PM_ACT_CFG)
#define Encoder_Clock_CLKEN_PTR          ((reg8 *) Encoder_Clock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Encoder_Clock_CLKSTBY            (* (reg8 *) Encoder_Clock__PM_STBY_CFG)
#define Encoder_Clock_CLKSTBY_PTR        ((reg8 *) Encoder_Clock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Encoder_Clock_DIV_LSB            (* (reg8 *) Encoder_Clock__CFG0)
#define Encoder_Clock_DIV_LSB_PTR        ((reg8 *) Encoder_Clock__CFG0)
#define Encoder_Clock_DIV_PTR            ((reg16 *) Encoder_Clock__CFG0)

/* Clock MSB divider configuration register. */
#define Encoder_Clock_DIV_MSB            (* (reg8 *) Encoder_Clock__CFG1)
#define Encoder_Clock_DIV_MSB_PTR        ((reg8 *) Encoder_Clock__CFG1)

/* Mode and source configuration register */
#define Encoder_Clock_MOD_SRC            (* (reg8 *) Encoder_Clock__CFG2)
#define Encoder_Clock_MOD_SRC_PTR        ((reg8 *) Encoder_Clock__CFG2)

#if defined(Encoder_Clock__CFG3)
/* Analog clock phase configuration register */
#define Encoder_Clock_PHASE              (* (reg8 *) Encoder_Clock__CFG3)
#define Encoder_Clock_PHASE_PTR          ((reg8 *) Encoder_Clock__CFG3)
#endif /* defined(Encoder_Clock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Encoder_Clock_CLKEN_MASK         Encoder_Clock__PM_ACT_MSK
#define Encoder_Clock_CLKSTBY_MASK       Encoder_Clock__PM_STBY_MSK

/* CFG2 field masks */
#define Encoder_Clock_SRC_SEL_MSK        Encoder_Clock__CFG2_SRC_SEL_MASK
#define Encoder_Clock_MODE_MASK          (~(Encoder_Clock_SRC_SEL_MSK))

#if defined(Encoder_Clock__CFG3)
/* CFG3 phase mask */
#define Encoder_Clock_PHASE_MASK         Encoder_Clock__CFG3_PHASE_DLY_MASK
#endif /* defined(Encoder_Clock__CFG3) */

#endif /* CY_CLOCK_Encoder_Clock_H */


/* [] END OF FILE */
