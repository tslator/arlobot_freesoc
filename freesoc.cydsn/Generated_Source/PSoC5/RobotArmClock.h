/*******************************************************************************
* File Name: RobotArmClock.h
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

#if !defined(CY_CLOCK_RobotArmClock_H)
#define CY_CLOCK_RobotArmClock_H

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

void RobotArmClock_Start(void) ;
void RobotArmClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void RobotArmClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void RobotArmClock_StandbyPower(uint8 state) ;
void RobotArmClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 RobotArmClock_GetDividerRegister(void) ;
void RobotArmClock_SetModeRegister(uint8 modeBitMask) ;
void RobotArmClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 RobotArmClock_GetModeRegister(void) ;
void RobotArmClock_SetSourceRegister(uint8 clkSource) ;
uint8 RobotArmClock_GetSourceRegister(void) ;
#if defined(RobotArmClock__CFG3)
void RobotArmClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 RobotArmClock_GetPhaseRegister(void) ;
#endif /* defined(RobotArmClock__CFG3) */

#define RobotArmClock_Enable()                       RobotArmClock_Start()
#define RobotArmClock_Disable()                      RobotArmClock_Stop()
#define RobotArmClock_SetDivider(clkDivider)         RobotArmClock_SetDividerRegister(clkDivider, 1u)
#define RobotArmClock_SetDividerValue(clkDivider)    RobotArmClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define RobotArmClock_SetMode(clkMode)               RobotArmClock_SetModeRegister(clkMode)
#define RobotArmClock_SetSource(clkSource)           RobotArmClock_SetSourceRegister(clkSource)
#if defined(RobotArmClock__CFG3)
#define RobotArmClock_SetPhase(clkPhase)             RobotArmClock_SetPhaseRegister(clkPhase)
#define RobotArmClock_SetPhaseValue(clkPhase)        RobotArmClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(RobotArmClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define RobotArmClock_CLKEN              (* (reg8 *) RobotArmClock__PM_ACT_CFG)
#define RobotArmClock_CLKEN_PTR          ((reg8 *) RobotArmClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define RobotArmClock_CLKSTBY            (* (reg8 *) RobotArmClock__PM_STBY_CFG)
#define RobotArmClock_CLKSTBY_PTR        ((reg8 *) RobotArmClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define RobotArmClock_DIV_LSB            (* (reg8 *) RobotArmClock__CFG0)
#define RobotArmClock_DIV_LSB_PTR        ((reg8 *) RobotArmClock__CFG0)
#define RobotArmClock_DIV_PTR            ((reg16 *) RobotArmClock__CFG0)

/* Clock MSB divider configuration register. */
#define RobotArmClock_DIV_MSB            (* (reg8 *) RobotArmClock__CFG1)
#define RobotArmClock_DIV_MSB_PTR        ((reg8 *) RobotArmClock__CFG1)

/* Mode and source configuration register */
#define RobotArmClock_MOD_SRC            (* (reg8 *) RobotArmClock__CFG2)
#define RobotArmClock_MOD_SRC_PTR        ((reg8 *) RobotArmClock__CFG2)

#if defined(RobotArmClock__CFG3)
/* Analog clock phase configuration register */
#define RobotArmClock_PHASE              (* (reg8 *) RobotArmClock__CFG3)
#define RobotArmClock_PHASE_PTR          ((reg8 *) RobotArmClock__CFG3)
#endif /* defined(RobotArmClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define RobotArmClock_CLKEN_MASK         RobotArmClock__PM_ACT_MSK
#define RobotArmClock_CLKSTBY_MASK       RobotArmClock__PM_STBY_MSK

/* CFG2 field masks */
#define RobotArmClock_SRC_SEL_MSK        RobotArmClock__CFG2_SRC_SEL_MASK
#define RobotArmClock_MODE_MASK          (~(RobotArmClock_SRC_SEL_MSK))

#if defined(RobotArmClock__CFG3)
/* CFG3 phase mask */
#define RobotArmClock_PHASE_MASK         RobotArmClock__CFG3_PHASE_DLY_MASK
#endif /* defined(RobotArmClock__CFG3) */

#endif /* CY_CLOCK_RobotArmClock_H */


/* [] END OF FILE */
