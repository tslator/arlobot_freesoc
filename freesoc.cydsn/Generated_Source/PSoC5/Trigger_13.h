/*******************************************************************************
* File Name: Trigger_13.h  
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

#if !defined(CY_PINS_Trigger_13_H) /* Pins Trigger_13_H */
#define CY_PINS_Trigger_13_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Trigger_13_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Trigger_13__PORT == 15 && ((Trigger_13__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Trigger_13_Write(uint8 value);
void    Trigger_13_SetDriveMode(uint8 mode);
uint8   Trigger_13_ReadDataReg(void);
uint8   Trigger_13_Read(void);
void    Trigger_13_SetInterruptMode(uint16 position, uint16 mode);
uint8   Trigger_13_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Trigger_13_SetDriveMode() function.
     *  @{
     */
        #define Trigger_13_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Trigger_13_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Trigger_13_DM_RES_UP          PIN_DM_RES_UP
        #define Trigger_13_DM_RES_DWN         PIN_DM_RES_DWN
        #define Trigger_13_DM_OD_LO           PIN_DM_OD_LO
        #define Trigger_13_DM_OD_HI           PIN_DM_OD_HI
        #define Trigger_13_DM_STRONG          PIN_DM_STRONG
        #define Trigger_13_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Trigger_13_MASK               Trigger_13__MASK
#define Trigger_13_SHIFT              Trigger_13__SHIFT
#define Trigger_13_WIDTH              1u

/* Interrupt constants */
#if defined(Trigger_13__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Trigger_13_SetInterruptMode() function.
     *  @{
     */
        #define Trigger_13_INTR_NONE      (uint16)(0x0000u)
        #define Trigger_13_INTR_RISING    (uint16)(0x0001u)
        #define Trigger_13_INTR_FALLING   (uint16)(0x0002u)
        #define Trigger_13_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Trigger_13_INTR_MASK      (0x01u) 
#endif /* (Trigger_13__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Trigger_13_PS                     (* (reg8 *) Trigger_13__PS)
/* Data Register */
#define Trigger_13_DR                     (* (reg8 *) Trigger_13__DR)
/* Port Number */
#define Trigger_13_PRT_NUM                (* (reg8 *) Trigger_13__PRT) 
/* Connect to Analog Globals */                                                  
#define Trigger_13_AG                     (* (reg8 *) Trigger_13__AG)                       
/* Analog MUX bux enable */
#define Trigger_13_AMUX                   (* (reg8 *) Trigger_13__AMUX) 
/* Bidirectional Enable */                                                        
#define Trigger_13_BIE                    (* (reg8 *) Trigger_13__BIE)
/* Bit-mask for Aliased Register Access */
#define Trigger_13_BIT_MASK               (* (reg8 *) Trigger_13__BIT_MASK)
/* Bypass Enable */
#define Trigger_13_BYP                    (* (reg8 *) Trigger_13__BYP)
/* Port wide control signals */                                                   
#define Trigger_13_CTL                    (* (reg8 *) Trigger_13__CTL)
/* Drive Modes */
#define Trigger_13_DM0                    (* (reg8 *) Trigger_13__DM0) 
#define Trigger_13_DM1                    (* (reg8 *) Trigger_13__DM1)
#define Trigger_13_DM2                    (* (reg8 *) Trigger_13__DM2) 
/* Input Buffer Disable Override */
#define Trigger_13_INP_DIS                (* (reg8 *) Trigger_13__INP_DIS)
/* LCD Common or Segment Drive */
#define Trigger_13_LCD_COM_SEG            (* (reg8 *) Trigger_13__LCD_COM_SEG)
/* Enable Segment LCD */
#define Trigger_13_LCD_EN                 (* (reg8 *) Trigger_13__LCD_EN)
/* Slew Rate Control */
#define Trigger_13_SLW                    (* (reg8 *) Trigger_13__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Trigger_13_PRTDSI__CAPS_SEL       (* (reg8 *) Trigger_13__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Trigger_13_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Trigger_13__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Trigger_13_PRTDSI__OE_SEL0        (* (reg8 *) Trigger_13__PRTDSI__OE_SEL0) 
#define Trigger_13_PRTDSI__OE_SEL1        (* (reg8 *) Trigger_13__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Trigger_13_PRTDSI__OUT_SEL0       (* (reg8 *) Trigger_13__PRTDSI__OUT_SEL0) 
#define Trigger_13_PRTDSI__OUT_SEL1       (* (reg8 *) Trigger_13__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Trigger_13_PRTDSI__SYNC_OUT       (* (reg8 *) Trigger_13__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Trigger_13__SIO_CFG)
    #define Trigger_13_SIO_HYST_EN        (* (reg8 *) Trigger_13__SIO_HYST_EN)
    #define Trigger_13_SIO_REG_HIFREQ     (* (reg8 *) Trigger_13__SIO_REG_HIFREQ)
    #define Trigger_13_SIO_CFG            (* (reg8 *) Trigger_13__SIO_CFG)
    #define Trigger_13_SIO_DIFF           (* (reg8 *) Trigger_13__SIO_DIFF)
#endif /* (Trigger_13__SIO_CFG) */

/* Interrupt Registers */
#if defined(Trigger_13__INTSTAT)
    #define Trigger_13_INTSTAT            (* (reg8 *) Trigger_13__INTSTAT)
    #define Trigger_13_SNAP               (* (reg8 *) Trigger_13__SNAP)
    
	#define Trigger_13_0_INTTYPE_REG 		(* (reg8 *) Trigger_13__0__INTTYPE)
#endif /* (Trigger_13__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Trigger_13_H */


/* [] END OF FILE */
