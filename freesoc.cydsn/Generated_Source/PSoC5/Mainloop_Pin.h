/*******************************************************************************
* File Name: Mainloop_Pin.h  
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

#if !defined(CY_PINS_Mainloop_Pin_H) /* Pins Mainloop_Pin_H */
#define CY_PINS_Mainloop_Pin_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Mainloop_Pin_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Mainloop_Pin__PORT == 15 && ((Mainloop_Pin__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Mainloop_Pin_Write(uint8 value);
void    Mainloop_Pin_SetDriveMode(uint8 mode);
uint8   Mainloop_Pin_ReadDataReg(void);
uint8   Mainloop_Pin_Read(void);
void    Mainloop_Pin_SetInterruptMode(uint16 position, uint16 mode);
uint8   Mainloop_Pin_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Mainloop_Pin_SetDriveMode() function.
     *  @{
     */
        #define Mainloop_Pin_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Mainloop_Pin_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Mainloop_Pin_DM_RES_UP          PIN_DM_RES_UP
        #define Mainloop_Pin_DM_RES_DWN         PIN_DM_RES_DWN
        #define Mainloop_Pin_DM_OD_LO           PIN_DM_OD_LO
        #define Mainloop_Pin_DM_OD_HI           PIN_DM_OD_HI
        #define Mainloop_Pin_DM_STRONG          PIN_DM_STRONG
        #define Mainloop_Pin_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Mainloop_Pin_MASK               Mainloop_Pin__MASK
#define Mainloop_Pin_SHIFT              Mainloop_Pin__SHIFT
#define Mainloop_Pin_WIDTH              1u

/* Interrupt constants */
#if defined(Mainloop_Pin__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Mainloop_Pin_SetInterruptMode() function.
     *  @{
     */
        #define Mainloop_Pin_INTR_NONE      (uint16)(0x0000u)
        #define Mainloop_Pin_INTR_RISING    (uint16)(0x0001u)
        #define Mainloop_Pin_INTR_FALLING   (uint16)(0x0002u)
        #define Mainloop_Pin_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Mainloop_Pin_INTR_MASK      (0x01u) 
#endif /* (Mainloop_Pin__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Mainloop_Pin_PS                     (* (reg8 *) Mainloop_Pin__PS)
/* Data Register */
#define Mainloop_Pin_DR                     (* (reg8 *) Mainloop_Pin__DR)
/* Port Number */
#define Mainloop_Pin_PRT_NUM                (* (reg8 *) Mainloop_Pin__PRT) 
/* Connect to Analog Globals */                                                  
#define Mainloop_Pin_AG                     (* (reg8 *) Mainloop_Pin__AG)                       
/* Analog MUX bux enable */
#define Mainloop_Pin_AMUX                   (* (reg8 *) Mainloop_Pin__AMUX) 
/* Bidirectional Enable */                                                        
#define Mainloop_Pin_BIE                    (* (reg8 *) Mainloop_Pin__BIE)
/* Bit-mask for Aliased Register Access */
#define Mainloop_Pin_BIT_MASK               (* (reg8 *) Mainloop_Pin__BIT_MASK)
/* Bypass Enable */
#define Mainloop_Pin_BYP                    (* (reg8 *) Mainloop_Pin__BYP)
/* Port wide control signals */                                                   
#define Mainloop_Pin_CTL                    (* (reg8 *) Mainloop_Pin__CTL)
/* Drive Modes */
#define Mainloop_Pin_DM0                    (* (reg8 *) Mainloop_Pin__DM0) 
#define Mainloop_Pin_DM1                    (* (reg8 *) Mainloop_Pin__DM1)
#define Mainloop_Pin_DM2                    (* (reg8 *) Mainloop_Pin__DM2) 
/* Input Buffer Disable Override */
#define Mainloop_Pin_INP_DIS                (* (reg8 *) Mainloop_Pin__INP_DIS)
/* LCD Common or Segment Drive */
#define Mainloop_Pin_LCD_COM_SEG            (* (reg8 *) Mainloop_Pin__LCD_COM_SEG)
/* Enable Segment LCD */
#define Mainloop_Pin_LCD_EN                 (* (reg8 *) Mainloop_Pin__LCD_EN)
/* Slew Rate Control */
#define Mainloop_Pin_SLW                    (* (reg8 *) Mainloop_Pin__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Mainloop_Pin_PRTDSI__CAPS_SEL       (* (reg8 *) Mainloop_Pin__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Mainloop_Pin_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Mainloop_Pin__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Mainloop_Pin_PRTDSI__OE_SEL0        (* (reg8 *) Mainloop_Pin__PRTDSI__OE_SEL0) 
#define Mainloop_Pin_PRTDSI__OE_SEL1        (* (reg8 *) Mainloop_Pin__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Mainloop_Pin_PRTDSI__OUT_SEL0       (* (reg8 *) Mainloop_Pin__PRTDSI__OUT_SEL0) 
#define Mainloop_Pin_PRTDSI__OUT_SEL1       (* (reg8 *) Mainloop_Pin__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Mainloop_Pin_PRTDSI__SYNC_OUT       (* (reg8 *) Mainloop_Pin__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Mainloop_Pin__SIO_CFG)
    #define Mainloop_Pin_SIO_HYST_EN        (* (reg8 *) Mainloop_Pin__SIO_HYST_EN)
    #define Mainloop_Pin_SIO_REG_HIFREQ     (* (reg8 *) Mainloop_Pin__SIO_REG_HIFREQ)
    #define Mainloop_Pin_SIO_CFG            (* (reg8 *) Mainloop_Pin__SIO_CFG)
    #define Mainloop_Pin_SIO_DIFF           (* (reg8 *) Mainloop_Pin__SIO_DIFF)
#endif /* (Mainloop_Pin__SIO_CFG) */

/* Interrupt Registers */
#if defined(Mainloop_Pin__INTSTAT)
    #define Mainloop_Pin_INTSTAT            (* (reg8 *) Mainloop_Pin__INTSTAT)
    #define Mainloop_Pin_SNAP               (* (reg8 *) Mainloop_Pin__SNAP)
    
	#define Mainloop_Pin_0_INTTYPE_REG 		(* (reg8 *) Mainloop_Pin__0__INTTYPE)
#endif /* (Mainloop_Pin__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Mainloop_Pin_H */


/* [] END OF FILE */
