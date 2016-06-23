/*******************************************************************************
* File Name: Rear_RightCliffDetect.h  
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

#if !defined(CY_PINS_Rear_RightCliffDetect_H) /* Pins Rear_RightCliffDetect_H */
#define CY_PINS_Rear_RightCliffDetect_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Rear_RightCliffDetect_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Rear_RightCliffDetect__PORT == 15 && ((Rear_RightCliffDetect__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Rear_RightCliffDetect_Write(uint8 value);
void    Rear_RightCliffDetect_SetDriveMode(uint8 mode);
uint8   Rear_RightCliffDetect_ReadDataReg(void);
uint8   Rear_RightCliffDetect_Read(void);
void    Rear_RightCliffDetect_SetInterruptMode(uint16 position, uint16 mode);
uint8   Rear_RightCliffDetect_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Rear_RightCliffDetect_SetDriveMode() function.
     *  @{
     */
        #define Rear_RightCliffDetect_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Rear_RightCliffDetect_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Rear_RightCliffDetect_DM_RES_UP          PIN_DM_RES_UP
        #define Rear_RightCliffDetect_DM_RES_DWN         PIN_DM_RES_DWN
        #define Rear_RightCliffDetect_DM_OD_LO           PIN_DM_OD_LO
        #define Rear_RightCliffDetect_DM_OD_HI           PIN_DM_OD_HI
        #define Rear_RightCliffDetect_DM_STRONG          PIN_DM_STRONG
        #define Rear_RightCliffDetect_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Rear_RightCliffDetect_MASK               Rear_RightCliffDetect__MASK
#define Rear_RightCliffDetect_SHIFT              Rear_RightCliffDetect__SHIFT
#define Rear_RightCliffDetect_WIDTH              1u

/* Interrupt constants */
#if defined(Rear_RightCliffDetect__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Rear_RightCliffDetect_SetInterruptMode() function.
     *  @{
     */
        #define Rear_RightCliffDetect_INTR_NONE      (uint16)(0x0000u)
        #define Rear_RightCliffDetect_INTR_RISING    (uint16)(0x0001u)
        #define Rear_RightCliffDetect_INTR_FALLING   (uint16)(0x0002u)
        #define Rear_RightCliffDetect_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Rear_RightCliffDetect_INTR_MASK      (0x01u) 
#endif /* (Rear_RightCliffDetect__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Rear_RightCliffDetect_PS                     (* (reg8 *) Rear_RightCliffDetect__PS)
/* Data Register */
#define Rear_RightCliffDetect_DR                     (* (reg8 *) Rear_RightCliffDetect__DR)
/* Port Number */
#define Rear_RightCliffDetect_PRT_NUM                (* (reg8 *) Rear_RightCliffDetect__PRT) 
/* Connect to Analog Globals */                                                  
#define Rear_RightCliffDetect_AG                     (* (reg8 *) Rear_RightCliffDetect__AG)                       
/* Analog MUX bux enable */
#define Rear_RightCliffDetect_AMUX                   (* (reg8 *) Rear_RightCliffDetect__AMUX) 
/* Bidirectional Enable */                                                        
#define Rear_RightCliffDetect_BIE                    (* (reg8 *) Rear_RightCliffDetect__BIE)
/* Bit-mask for Aliased Register Access */
#define Rear_RightCliffDetect_BIT_MASK               (* (reg8 *) Rear_RightCliffDetect__BIT_MASK)
/* Bypass Enable */
#define Rear_RightCliffDetect_BYP                    (* (reg8 *) Rear_RightCliffDetect__BYP)
/* Port wide control signals */                                                   
#define Rear_RightCliffDetect_CTL                    (* (reg8 *) Rear_RightCliffDetect__CTL)
/* Drive Modes */
#define Rear_RightCliffDetect_DM0                    (* (reg8 *) Rear_RightCliffDetect__DM0) 
#define Rear_RightCliffDetect_DM1                    (* (reg8 *) Rear_RightCliffDetect__DM1)
#define Rear_RightCliffDetect_DM2                    (* (reg8 *) Rear_RightCliffDetect__DM2) 
/* Input Buffer Disable Override */
#define Rear_RightCliffDetect_INP_DIS                (* (reg8 *) Rear_RightCliffDetect__INP_DIS)
/* LCD Common or Segment Drive */
#define Rear_RightCliffDetect_LCD_COM_SEG            (* (reg8 *) Rear_RightCliffDetect__LCD_COM_SEG)
/* Enable Segment LCD */
#define Rear_RightCliffDetect_LCD_EN                 (* (reg8 *) Rear_RightCliffDetect__LCD_EN)
/* Slew Rate Control */
#define Rear_RightCliffDetect_SLW                    (* (reg8 *) Rear_RightCliffDetect__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Rear_RightCliffDetect_PRTDSI__CAPS_SEL       (* (reg8 *) Rear_RightCliffDetect__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Rear_RightCliffDetect_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Rear_RightCliffDetect__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Rear_RightCliffDetect_PRTDSI__OE_SEL0        (* (reg8 *) Rear_RightCliffDetect__PRTDSI__OE_SEL0) 
#define Rear_RightCliffDetect_PRTDSI__OE_SEL1        (* (reg8 *) Rear_RightCliffDetect__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Rear_RightCliffDetect_PRTDSI__OUT_SEL0       (* (reg8 *) Rear_RightCliffDetect__PRTDSI__OUT_SEL0) 
#define Rear_RightCliffDetect_PRTDSI__OUT_SEL1       (* (reg8 *) Rear_RightCliffDetect__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Rear_RightCliffDetect_PRTDSI__SYNC_OUT       (* (reg8 *) Rear_RightCliffDetect__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Rear_RightCliffDetect__SIO_CFG)
    #define Rear_RightCliffDetect_SIO_HYST_EN        (* (reg8 *) Rear_RightCliffDetect__SIO_HYST_EN)
    #define Rear_RightCliffDetect_SIO_REG_HIFREQ     (* (reg8 *) Rear_RightCliffDetect__SIO_REG_HIFREQ)
    #define Rear_RightCliffDetect_SIO_CFG            (* (reg8 *) Rear_RightCliffDetect__SIO_CFG)
    #define Rear_RightCliffDetect_SIO_DIFF           (* (reg8 *) Rear_RightCliffDetect__SIO_DIFF)
#endif /* (Rear_RightCliffDetect__SIO_CFG) */

/* Interrupt Registers */
#if defined(Rear_RightCliffDetect__INTSTAT)
    #define Rear_RightCliffDetect_INTSTAT            (* (reg8 *) Rear_RightCliffDetect__INTSTAT)
    #define Rear_RightCliffDetect_SNAP               (* (reg8 *) Rear_RightCliffDetect__SNAP)
    
	#define Rear_RightCliffDetect_0_INTTYPE_REG 		(* (reg8 *) Rear_RightCliffDetect__0__INTTYPE)
#endif /* (Rear_RightCliffDetect__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Rear_RightCliffDetect_H */


/* [] END OF FILE */
