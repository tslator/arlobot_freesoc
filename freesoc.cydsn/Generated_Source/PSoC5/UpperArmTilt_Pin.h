/*******************************************************************************
* File Name: UpperArmTilt_Pin.h  
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

#if !defined(CY_PINS_UpperArmTilt_Pin_H) /* Pins UpperArmTilt_Pin_H */
#define CY_PINS_UpperArmTilt_Pin_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "UpperArmTilt_Pin_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 UpperArmTilt_Pin__PORT == 15 && ((UpperArmTilt_Pin__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    UpperArmTilt_Pin_Write(uint8 value);
void    UpperArmTilt_Pin_SetDriveMode(uint8 mode);
uint8   UpperArmTilt_Pin_ReadDataReg(void);
uint8   UpperArmTilt_Pin_Read(void);
void    UpperArmTilt_Pin_SetInterruptMode(uint16 position, uint16 mode);
uint8   UpperArmTilt_Pin_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the UpperArmTilt_Pin_SetDriveMode() function.
     *  @{
     */
        #define UpperArmTilt_Pin_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define UpperArmTilt_Pin_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define UpperArmTilt_Pin_DM_RES_UP          PIN_DM_RES_UP
        #define UpperArmTilt_Pin_DM_RES_DWN         PIN_DM_RES_DWN
        #define UpperArmTilt_Pin_DM_OD_LO           PIN_DM_OD_LO
        #define UpperArmTilt_Pin_DM_OD_HI           PIN_DM_OD_HI
        #define UpperArmTilt_Pin_DM_STRONG          PIN_DM_STRONG
        #define UpperArmTilt_Pin_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define UpperArmTilt_Pin_MASK               UpperArmTilt_Pin__MASK
#define UpperArmTilt_Pin_SHIFT              UpperArmTilt_Pin__SHIFT
#define UpperArmTilt_Pin_WIDTH              1u

/* Interrupt constants */
#if defined(UpperArmTilt_Pin__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in UpperArmTilt_Pin_SetInterruptMode() function.
     *  @{
     */
        #define UpperArmTilt_Pin_INTR_NONE      (uint16)(0x0000u)
        #define UpperArmTilt_Pin_INTR_RISING    (uint16)(0x0001u)
        #define UpperArmTilt_Pin_INTR_FALLING   (uint16)(0x0002u)
        #define UpperArmTilt_Pin_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define UpperArmTilt_Pin_INTR_MASK      (0x01u) 
#endif /* (UpperArmTilt_Pin__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define UpperArmTilt_Pin_PS                     (* (reg8 *) UpperArmTilt_Pin__PS)
/* Data Register */
#define UpperArmTilt_Pin_DR                     (* (reg8 *) UpperArmTilt_Pin__DR)
/* Port Number */
#define UpperArmTilt_Pin_PRT_NUM                (* (reg8 *) UpperArmTilt_Pin__PRT) 
/* Connect to Analog Globals */                                                  
#define UpperArmTilt_Pin_AG                     (* (reg8 *) UpperArmTilt_Pin__AG)                       
/* Analog MUX bux enable */
#define UpperArmTilt_Pin_AMUX                   (* (reg8 *) UpperArmTilt_Pin__AMUX) 
/* Bidirectional Enable */                                                        
#define UpperArmTilt_Pin_BIE                    (* (reg8 *) UpperArmTilt_Pin__BIE)
/* Bit-mask for Aliased Register Access */
#define UpperArmTilt_Pin_BIT_MASK               (* (reg8 *) UpperArmTilt_Pin__BIT_MASK)
/* Bypass Enable */
#define UpperArmTilt_Pin_BYP                    (* (reg8 *) UpperArmTilt_Pin__BYP)
/* Port wide control signals */                                                   
#define UpperArmTilt_Pin_CTL                    (* (reg8 *) UpperArmTilt_Pin__CTL)
/* Drive Modes */
#define UpperArmTilt_Pin_DM0                    (* (reg8 *) UpperArmTilt_Pin__DM0) 
#define UpperArmTilt_Pin_DM1                    (* (reg8 *) UpperArmTilt_Pin__DM1)
#define UpperArmTilt_Pin_DM2                    (* (reg8 *) UpperArmTilt_Pin__DM2) 
/* Input Buffer Disable Override */
#define UpperArmTilt_Pin_INP_DIS                (* (reg8 *) UpperArmTilt_Pin__INP_DIS)
/* LCD Common or Segment Drive */
#define UpperArmTilt_Pin_LCD_COM_SEG            (* (reg8 *) UpperArmTilt_Pin__LCD_COM_SEG)
/* Enable Segment LCD */
#define UpperArmTilt_Pin_LCD_EN                 (* (reg8 *) UpperArmTilt_Pin__LCD_EN)
/* Slew Rate Control */
#define UpperArmTilt_Pin_SLW                    (* (reg8 *) UpperArmTilt_Pin__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define UpperArmTilt_Pin_PRTDSI__CAPS_SEL       (* (reg8 *) UpperArmTilt_Pin__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define UpperArmTilt_Pin_PRTDSI__DBL_SYNC_IN    (* (reg8 *) UpperArmTilt_Pin__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define UpperArmTilt_Pin_PRTDSI__OE_SEL0        (* (reg8 *) UpperArmTilt_Pin__PRTDSI__OE_SEL0) 
#define UpperArmTilt_Pin_PRTDSI__OE_SEL1        (* (reg8 *) UpperArmTilt_Pin__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define UpperArmTilt_Pin_PRTDSI__OUT_SEL0       (* (reg8 *) UpperArmTilt_Pin__PRTDSI__OUT_SEL0) 
#define UpperArmTilt_Pin_PRTDSI__OUT_SEL1       (* (reg8 *) UpperArmTilt_Pin__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define UpperArmTilt_Pin_PRTDSI__SYNC_OUT       (* (reg8 *) UpperArmTilt_Pin__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(UpperArmTilt_Pin__SIO_CFG)
    #define UpperArmTilt_Pin_SIO_HYST_EN        (* (reg8 *) UpperArmTilt_Pin__SIO_HYST_EN)
    #define UpperArmTilt_Pin_SIO_REG_HIFREQ     (* (reg8 *) UpperArmTilt_Pin__SIO_REG_HIFREQ)
    #define UpperArmTilt_Pin_SIO_CFG            (* (reg8 *) UpperArmTilt_Pin__SIO_CFG)
    #define UpperArmTilt_Pin_SIO_DIFF           (* (reg8 *) UpperArmTilt_Pin__SIO_DIFF)
#endif /* (UpperArmTilt_Pin__SIO_CFG) */

/* Interrupt Registers */
#if defined(UpperArmTilt_Pin__INTSTAT)
    #define UpperArmTilt_Pin_INTSTAT            (* (reg8 *) UpperArmTilt_Pin__INTSTAT)
    #define UpperArmTilt_Pin_SNAP               (* (reg8 *) UpperArmTilt_Pin__SNAP)
    
	#define UpperArmTilt_Pin_0_INTTYPE_REG 		(* (reg8 *) UpperArmTilt_Pin__0__INTTYPE)
#endif /* (UpperArmTilt_Pin__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_UpperArmTilt_Pin_H */


/* [] END OF FILE */
