/*******************************************************************************
* File Name: Trigger_12.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Trigger_12_H) /* Pins Trigger_12_H */
#define CY_PINS_Trigger_12_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Trigger_12_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Trigger_12__PORT == 15 && ((Trigger_12__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Trigger_12_Write(uint8 value);
void    Trigger_12_SetDriveMode(uint8 mode);
uint8   Trigger_12_ReadDataReg(void);
uint8   Trigger_12_Read(void);
void    Trigger_12_SetInterruptMode(uint16 position, uint16 mode);
uint8   Trigger_12_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Trigger_12_SetDriveMode() function.
     *  @{
     */
        #define Trigger_12_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Trigger_12_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Trigger_12_DM_RES_UP          PIN_DM_RES_UP
        #define Trigger_12_DM_RES_DWN         PIN_DM_RES_DWN
        #define Trigger_12_DM_OD_LO           PIN_DM_OD_LO
        #define Trigger_12_DM_OD_HI           PIN_DM_OD_HI
        #define Trigger_12_DM_STRONG          PIN_DM_STRONG
        #define Trigger_12_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Trigger_12_MASK               Trigger_12__MASK
#define Trigger_12_SHIFT              Trigger_12__SHIFT
#define Trigger_12_WIDTH              1u

/* Interrupt constants */
#if defined(Trigger_12__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Trigger_12_SetInterruptMode() function.
     *  @{
     */
        #define Trigger_12_INTR_NONE      (uint16)(0x0000u)
        #define Trigger_12_INTR_RISING    (uint16)(0x0001u)
        #define Trigger_12_INTR_FALLING   (uint16)(0x0002u)
        #define Trigger_12_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Trigger_12_INTR_MASK      (0x01u) 
#endif /* (Trigger_12__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Trigger_12_PS                     (* (reg8 *) Trigger_12__PS)
/* Data Register */
#define Trigger_12_DR                     (* (reg8 *) Trigger_12__DR)
/* Port Number */
#define Trigger_12_PRT_NUM                (* (reg8 *) Trigger_12__PRT) 
/* Connect to Analog Globals */                                                  
#define Trigger_12_AG                     (* (reg8 *) Trigger_12__AG)                       
/* Analog MUX bux enable */
#define Trigger_12_AMUX                   (* (reg8 *) Trigger_12__AMUX) 
/* Bidirectional Enable */                                                        
#define Trigger_12_BIE                    (* (reg8 *) Trigger_12__BIE)
/* Bit-mask for Aliased Register Access */
#define Trigger_12_BIT_MASK               (* (reg8 *) Trigger_12__BIT_MASK)
/* Bypass Enable */
#define Trigger_12_BYP                    (* (reg8 *) Trigger_12__BYP)
/* Port wide control signals */                                                   
#define Trigger_12_CTL                    (* (reg8 *) Trigger_12__CTL)
/* Drive Modes */
#define Trigger_12_DM0                    (* (reg8 *) Trigger_12__DM0) 
#define Trigger_12_DM1                    (* (reg8 *) Trigger_12__DM1)
#define Trigger_12_DM2                    (* (reg8 *) Trigger_12__DM2) 
/* Input Buffer Disable Override */
#define Trigger_12_INP_DIS                (* (reg8 *) Trigger_12__INP_DIS)
/* LCD Common or Segment Drive */
#define Trigger_12_LCD_COM_SEG            (* (reg8 *) Trigger_12__LCD_COM_SEG)
/* Enable Segment LCD */
#define Trigger_12_LCD_EN                 (* (reg8 *) Trigger_12__LCD_EN)
/* Slew Rate Control */
#define Trigger_12_SLW                    (* (reg8 *) Trigger_12__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Trigger_12_PRTDSI__CAPS_SEL       (* (reg8 *) Trigger_12__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Trigger_12_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Trigger_12__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Trigger_12_PRTDSI__OE_SEL0        (* (reg8 *) Trigger_12__PRTDSI__OE_SEL0) 
#define Trigger_12_PRTDSI__OE_SEL1        (* (reg8 *) Trigger_12__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Trigger_12_PRTDSI__OUT_SEL0       (* (reg8 *) Trigger_12__PRTDSI__OUT_SEL0) 
#define Trigger_12_PRTDSI__OUT_SEL1       (* (reg8 *) Trigger_12__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Trigger_12_PRTDSI__SYNC_OUT       (* (reg8 *) Trigger_12__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Trigger_12__SIO_CFG)
    #define Trigger_12_SIO_HYST_EN        (* (reg8 *) Trigger_12__SIO_HYST_EN)
    #define Trigger_12_SIO_REG_HIFREQ     (* (reg8 *) Trigger_12__SIO_REG_HIFREQ)
    #define Trigger_12_SIO_CFG            (* (reg8 *) Trigger_12__SIO_CFG)
    #define Trigger_12_SIO_DIFF           (* (reg8 *) Trigger_12__SIO_DIFF)
#endif /* (Trigger_12__SIO_CFG) */

/* Interrupt Registers */
#if defined(Trigger_12__INTSTAT)
    #define Trigger_12_INTSTAT            (* (reg8 *) Trigger_12__INTSTAT)
    #define Trigger_12_SNAP               (* (reg8 *) Trigger_12__SNAP)
    
	#define Trigger_12_0_INTTYPE_REG 		(* (reg8 *) Trigger_12__0__INTTYPE)
#endif /* (Trigger_12__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Trigger_12_H */


/* [] END OF FILE */
