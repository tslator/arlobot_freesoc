/*******************************************************************************
* File Name: Front_LeftCliffDetect.h  
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

#if !defined(CY_PINS_Front_LeftCliffDetect_H) /* Pins Front_LeftCliffDetect_H */
#define CY_PINS_Front_LeftCliffDetect_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Front_LeftCliffDetect_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Front_LeftCliffDetect__PORT == 15 && ((Front_LeftCliffDetect__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Front_LeftCliffDetect_Write(uint8 value);
void    Front_LeftCliffDetect_SetDriveMode(uint8 mode);
uint8   Front_LeftCliffDetect_ReadDataReg(void);
uint8   Front_LeftCliffDetect_Read(void);
void    Front_LeftCliffDetect_SetInterruptMode(uint16 position, uint16 mode);
uint8   Front_LeftCliffDetect_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Front_LeftCliffDetect_SetDriveMode() function.
     *  @{
     */
        #define Front_LeftCliffDetect_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Front_LeftCliffDetect_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Front_LeftCliffDetect_DM_RES_UP          PIN_DM_RES_UP
        #define Front_LeftCliffDetect_DM_RES_DWN         PIN_DM_RES_DWN
        #define Front_LeftCliffDetect_DM_OD_LO           PIN_DM_OD_LO
        #define Front_LeftCliffDetect_DM_OD_HI           PIN_DM_OD_HI
        #define Front_LeftCliffDetect_DM_STRONG          PIN_DM_STRONG
        #define Front_LeftCliffDetect_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Front_LeftCliffDetect_MASK               Front_LeftCliffDetect__MASK
#define Front_LeftCliffDetect_SHIFT              Front_LeftCliffDetect__SHIFT
#define Front_LeftCliffDetect_WIDTH              1u

/* Interrupt constants */
#if defined(Front_LeftCliffDetect__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Front_LeftCliffDetect_SetInterruptMode() function.
     *  @{
     */
        #define Front_LeftCliffDetect_INTR_NONE      (uint16)(0x0000u)
        #define Front_LeftCliffDetect_INTR_RISING    (uint16)(0x0001u)
        #define Front_LeftCliffDetect_INTR_FALLING   (uint16)(0x0002u)
        #define Front_LeftCliffDetect_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Front_LeftCliffDetect_INTR_MASK      (0x01u) 
#endif /* (Front_LeftCliffDetect__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Front_LeftCliffDetect_PS                     (* (reg8 *) Front_LeftCliffDetect__PS)
/* Data Register */
#define Front_LeftCliffDetect_DR                     (* (reg8 *) Front_LeftCliffDetect__DR)
/* Port Number */
#define Front_LeftCliffDetect_PRT_NUM                (* (reg8 *) Front_LeftCliffDetect__PRT) 
/* Connect to Analog Globals */                                                  
#define Front_LeftCliffDetect_AG                     (* (reg8 *) Front_LeftCliffDetect__AG)                       
/* Analog MUX bux enable */
#define Front_LeftCliffDetect_AMUX                   (* (reg8 *) Front_LeftCliffDetect__AMUX) 
/* Bidirectional Enable */                                                        
#define Front_LeftCliffDetect_BIE                    (* (reg8 *) Front_LeftCliffDetect__BIE)
/* Bit-mask for Aliased Register Access */
#define Front_LeftCliffDetect_BIT_MASK               (* (reg8 *) Front_LeftCliffDetect__BIT_MASK)
/* Bypass Enable */
#define Front_LeftCliffDetect_BYP                    (* (reg8 *) Front_LeftCliffDetect__BYP)
/* Port wide control signals */                                                   
#define Front_LeftCliffDetect_CTL                    (* (reg8 *) Front_LeftCliffDetect__CTL)
/* Drive Modes */
#define Front_LeftCliffDetect_DM0                    (* (reg8 *) Front_LeftCliffDetect__DM0) 
#define Front_LeftCliffDetect_DM1                    (* (reg8 *) Front_LeftCliffDetect__DM1)
#define Front_LeftCliffDetect_DM2                    (* (reg8 *) Front_LeftCliffDetect__DM2) 
/* Input Buffer Disable Override */
#define Front_LeftCliffDetect_INP_DIS                (* (reg8 *) Front_LeftCliffDetect__INP_DIS)
/* LCD Common or Segment Drive */
#define Front_LeftCliffDetect_LCD_COM_SEG            (* (reg8 *) Front_LeftCliffDetect__LCD_COM_SEG)
/* Enable Segment LCD */
#define Front_LeftCliffDetect_LCD_EN                 (* (reg8 *) Front_LeftCliffDetect__LCD_EN)
/* Slew Rate Control */
#define Front_LeftCliffDetect_SLW                    (* (reg8 *) Front_LeftCliffDetect__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Front_LeftCliffDetect_PRTDSI__CAPS_SEL       (* (reg8 *) Front_LeftCliffDetect__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Front_LeftCliffDetect_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Front_LeftCliffDetect__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Front_LeftCliffDetect_PRTDSI__OE_SEL0        (* (reg8 *) Front_LeftCliffDetect__PRTDSI__OE_SEL0) 
#define Front_LeftCliffDetect_PRTDSI__OE_SEL1        (* (reg8 *) Front_LeftCliffDetect__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Front_LeftCliffDetect_PRTDSI__OUT_SEL0       (* (reg8 *) Front_LeftCliffDetect__PRTDSI__OUT_SEL0) 
#define Front_LeftCliffDetect_PRTDSI__OUT_SEL1       (* (reg8 *) Front_LeftCliffDetect__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Front_LeftCliffDetect_PRTDSI__SYNC_OUT       (* (reg8 *) Front_LeftCliffDetect__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Front_LeftCliffDetect__SIO_CFG)
    #define Front_LeftCliffDetect_SIO_HYST_EN        (* (reg8 *) Front_LeftCliffDetect__SIO_HYST_EN)
    #define Front_LeftCliffDetect_SIO_REG_HIFREQ     (* (reg8 *) Front_LeftCliffDetect__SIO_REG_HIFREQ)
    #define Front_LeftCliffDetect_SIO_CFG            (* (reg8 *) Front_LeftCliffDetect__SIO_CFG)
    #define Front_LeftCliffDetect_SIO_DIFF           (* (reg8 *) Front_LeftCliffDetect__SIO_DIFF)
#endif /* (Front_LeftCliffDetect__SIO_CFG) */

/* Interrupt Registers */
#if defined(Front_LeftCliffDetect__INTSTAT)
    #define Front_LeftCliffDetect_INTSTAT            (* (reg8 *) Front_LeftCliffDetect__INTSTAT)
    #define Front_LeftCliffDetect_SNAP               (* (reg8 *) Front_LeftCliffDetect__SNAP)
    
	#define Front_LeftCliffDetect_0_INTTYPE_REG 		(* (reg8 *) Front_LeftCliffDetect__0__INTTYPE)
#endif /* (Front_LeftCliffDetect__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Front_LeftCliffDetect_H */


/* [] END OF FILE */
