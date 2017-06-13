/*******************************************************************************
* File Name: WristTilt_Pin.h  
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

#if !defined(CY_PINS_WristTilt_Pin_H) /* Pins WristTilt_Pin_H */
#define CY_PINS_WristTilt_Pin_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "WristTilt_Pin_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 WristTilt_Pin__PORT == 15 && ((WristTilt_Pin__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    WristTilt_Pin_Write(uint8 value);
void    WristTilt_Pin_SetDriveMode(uint8 mode);
uint8   WristTilt_Pin_ReadDataReg(void);
uint8   WristTilt_Pin_Read(void);
void    WristTilt_Pin_SetInterruptMode(uint16 position, uint16 mode);
uint8   WristTilt_Pin_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the WristTilt_Pin_SetDriveMode() function.
     *  @{
     */
        #define WristTilt_Pin_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define WristTilt_Pin_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define WristTilt_Pin_DM_RES_UP          PIN_DM_RES_UP
        #define WristTilt_Pin_DM_RES_DWN         PIN_DM_RES_DWN
        #define WristTilt_Pin_DM_OD_LO           PIN_DM_OD_LO
        #define WristTilt_Pin_DM_OD_HI           PIN_DM_OD_HI
        #define WristTilt_Pin_DM_STRONG          PIN_DM_STRONG
        #define WristTilt_Pin_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define WristTilt_Pin_MASK               WristTilt_Pin__MASK
#define WristTilt_Pin_SHIFT              WristTilt_Pin__SHIFT
#define WristTilt_Pin_WIDTH              1u

/* Interrupt constants */
#if defined(WristTilt_Pin__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in WristTilt_Pin_SetInterruptMode() function.
     *  @{
     */
        #define WristTilt_Pin_INTR_NONE      (uint16)(0x0000u)
        #define WristTilt_Pin_INTR_RISING    (uint16)(0x0001u)
        #define WristTilt_Pin_INTR_FALLING   (uint16)(0x0002u)
        #define WristTilt_Pin_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define WristTilt_Pin_INTR_MASK      (0x01u) 
#endif /* (WristTilt_Pin__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define WristTilt_Pin_PS                     (* (reg8 *) WristTilt_Pin__PS)
/* Data Register */
#define WristTilt_Pin_DR                     (* (reg8 *) WristTilt_Pin__DR)
/* Port Number */
#define WristTilt_Pin_PRT_NUM                (* (reg8 *) WristTilt_Pin__PRT) 
/* Connect to Analog Globals */                                                  
#define WristTilt_Pin_AG                     (* (reg8 *) WristTilt_Pin__AG)                       
/* Analog MUX bux enable */
#define WristTilt_Pin_AMUX                   (* (reg8 *) WristTilt_Pin__AMUX) 
/* Bidirectional Enable */                                                        
#define WristTilt_Pin_BIE                    (* (reg8 *) WristTilt_Pin__BIE)
/* Bit-mask for Aliased Register Access */
#define WristTilt_Pin_BIT_MASK               (* (reg8 *) WristTilt_Pin__BIT_MASK)
/* Bypass Enable */
#define WristTilt_Pin_BYP                    (* (reg8 *) WristTilt_Pin__BYP)
/* Port wide control signals */                                                   
#define WristTilt_Pin_CTL                    (* (reg8 *) WristTilt_Pin__CTL)
/* Drive Modes */
#define WristTilt_Pin_DM0                    (* (reg8 *) WristTilt_Pin__DM0) 
#define WristTilt_Pin_DM1                    (* (reg8 *) WristTilt_Pin__DM1)
#define WristTilt_Pin_DM2                    (* (reg8 *) WristTilt_Pin__DM2) 
/* Input Buffer Disable Override */
#define WristTilt_Pin_INP_DIS                (* (reg8 *) WristTilt_Pin__INP_DIS)
/* LCD Common or Segment Drive */
#define WristTilt_Pin_LCD_COM_SEG            (* (reg8 *) WristTilt_Pin__LCD_COM_SEG)
/* Enable Segment LCD */
#define WristTilt_Pin_LCD_EN                 (* (reg8 *) WristTilt_Pin__LCD_EN)
/* Slew Rate Control */
#define WristTilt_Pin_SLW                    (* (reg8 *) WristTilt_Pin__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define WristTilt_Pin_PRTDSI__CAPS_SEL       (* (reg8 *) WristTilt_Pin__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define WristTilt_Pin_PRTDSI__DBL_SYNC_IN    (* (reg8 *) WristTilt_Pin__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define WristTilt_Pin_PRTDSI__OE_SEL0        (* (reg8 *) WristTilt_Pin__PRTDSI__OE_SEL0) 
#define WristTilt_Pin_PRTDSI__OE_SEL1        (* (reg8 *) WristTilt_Pin__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define WristTilt_Pin_PRTDSI__OUT_SEL0       (* (reg8 *) WristTilt_Pin__PRTDSI__OUT_SEL0) 
#define WristTilt_Pin_PRTDSI__OUT_SEL1       (* (reg8 *) WristTilt_Pin__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define WristTilt_Pin_PRTDSI__SYNC_OUT       (* (reg8 *) WristTilt_Pin__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(WristTilt_Pin__SIO_CFG)
    #define WristTilt_Pin_SIO_HYST_EN        (* (reg8 *) WristTilt_Pin__SIO_HYST_EN)
    #define WristTilt_Pin_SIO_REG_HIFREQ     (* (reg8 *) WristTilt_Pin__SIO_REG_HIFREQ)
    #define WristTilt_Pin_SIO_CFG            (* (reg8 *) WristTilt_Pin__SIO_CFG)
    #define WristTilt_Pin_SIO_DIFF           (* (reg8 *) WristTilt_Pin__SIO_DIFF)
#endif /* (WristTilt_Pin__SIO_CFG) */

/* Interrupt Registers */
#if defined(WristTilt_Pin__INTSTAT)
    #define WristTilt_Pin_INTSTAT            (* (reg8 *) WristTilt_Pin__INTSTAT)
    #define WristTilt_Pin_SNAP               (* (reg8 *) WristTilt_Pin__SNAP)
    
	#define WristTilt_Pin_0_INTTYPE_REG 		(* (reg8 *) WristTilt_Pin__0__INTTYPE)
#endif /* (WristTilt_Pin__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_WristTilt_Pin_H */


/* [] END OF FILE */
