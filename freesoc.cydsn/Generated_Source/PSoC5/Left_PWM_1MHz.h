/*******************************************************************************
* File Name: Left_PWM_1MHz.h
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

#if !defined(CY_CLOCK_Left_PWM_1MHz_H)
#define CY_CLOCK_Left_PWM_1MHz_H

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

void Left_PWM_1MHz_Start(void) ;
void Left_PWM_1MHz_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Left_PWM_1MHz_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Left_PWM_1MHz_StandbyPower(uint8 state) ;
void Left_PWM_1MHz_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Left_PWM_1MHz_GetDividerRegister(void) ;
void Left_PWM_1MHz_SetModeRegister(uint8 modeBitMask) ;
void Left_PWM_1MHz_ClearModeRegister(uint8 modeBitMask) ;
uint8 Left_PWM_1MHz_GetModeRegister(void) ;
void Left_PWM_1MHz_SetSourceRegister(uint8 clkSource) ;
uint8 Left_PWM_1MHz_GetSourceRegister(void) ;
#if defined(Left_PWM_1MHz__CFG3)
void Left_PWM_1MHz_SetPhaseRegister(uint8 clkPhase) ;
uint8 Left_PWM_1MHz_GetPhaseRegister(void) ;
#endif /* defined(Left_PWM_1MHz__CFG3) */

#define Left_PWM_1MHz_Enable()                       Left_PWM_1MHz_Start()
#define Left_PWM_1MHz_Disable()                      Left_PWM_1MHz_Stop()
#define Left_PWM_1MHz_SetDivider(clkDivider)         Left_PWM_1MHz_SetDividerRegister(clkDivider, 1u)
#define Left_PWM_1MHz_SetDividerValue(clkDivider)    Left_PWM_1MHz_SetDividerRegister((clkDivider) - 1u, 1u)
#define Left_PWM_1MHz_SetMode(clkMode)               Left_PWM_1MHz_SetModeRegister(clkMode)
#define Left_PWM_1MHz_SetSource(clkSource)           Left_PWM_1MHz_SetSourceRegister(clkSource)
#if defined(Left_PWM_1MHz__CFG3)
#define Left_PWM_1MHz_SetPhase(clkPhase)             Left_PWM_1MHz_SetPhaseRegister(clkPhase)
#define Left_PWM_1MHz_SetPhaseValue(clkPhase)        Left_PWM_1MHz_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(Left_PWM_1MHz__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Left_PWM_1MHz_CLKEN              (* (reg8 *) Left_PWM_1MHz__PM_ACT_CFG)
#define Left_PWM_1MHz_CLKEN_PTR          ((reg8 *) Left_PWM_1MHz__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Left_PWM_1MHz_CLKSTBY            (* (reg8 *) Left_PWM_1MHz__PM_STBY_CFG)
#define Left_PWM_1MHz_CLKSTBY_PTR        ((reg8 *) Left_PWM_1MHz__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Left_PWM_1MHz_DIV_LSB            (* (reg8 *) Left_PWM_1MHz__CFG0)
#define Left_PWM_1MHz_DIV_LSB_PTR        ((reg8 *) Left_PWM_1MHz__CFG0)
#define Left_PWM_1MHz_DIV_PTR            ((reg16 *) Left_PWM_1MHz__CFG0)

/* Clock MSB divider configuration register. */
#define Left_PWM_1MHz_DIV_MSB            (* (reg8 *) Left_PWM_1MHz__CFG1)
#define Left_PWM_1MHz_DIV_MSB_PTR        ((reg8 *) Left_PWM_1MHz__CFG1)

/* Mode and source configuration register */
#define Left_PWM_1MHz_MOD_SRC            (* (reg8 *) Left_PWM_1MHz__CFG2)
#define Left_PWM_1MHz_MOD_SRC_PTR        ((reg8 *) Left_PWM_1MHz__CFG2)

#if defined(Left_PWM_1MHz__CFG3)
/* Analog clock phase configuration register */
#define Left_PWM_1MHz_PHASE              (* (reg8 *) Left_PWM_1MHz__CFG3)
#define Left_PWM_1MHz_PHASE_PTR          ((reg8 *) Left_PWM_1MHz__CFG3)
#endif /* defined(Left_PWM_1MHz__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Left_PWM_1MHz_CLKEN_MASK         Left_PWM_1MHz__PM_ACT_MSK
#define Left_PWM_1MHz_CLKSTBY_MASK       Left_PWM_1MHz__PM_STBY_MSK

/* CFG2 field masks */
#define Left_PWM_1MHz_SRC_SEL_MSK        Left_PWM_1MHz__CFG2_SRC_SEL_MASK
#define Left_PWM_1MHz_MODE_MASK          (~(Left_PWM_1MHz_SRC_SEL_MSK))

#if defined(Left_PWM_1MHz__CFG3)
/* CFG3 phase mask */
#define Left_PWM_1MHz_PHASE_MASK         Left_PWM_1MHz__CFG3_PHASE_DLY_MASK
#endif /* defined(Left_PWM_1MHz__CFG3) */

#endif /* CY_CLOCK_Left_PWM_1MHz_H */


/* [] END OF FILE */
