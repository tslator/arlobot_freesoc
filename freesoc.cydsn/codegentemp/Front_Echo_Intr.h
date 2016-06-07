/*******************************************************************************
* File Name: Front_Echo_Intr.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_Front_Echo_Intr_H)
#define CY_ISR_Front_Echo_Intr_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Front_Echo_Intr_Start(void);
void Front_Echo_Intr_StartEx(cyisraddress address);
void Front_Echo_Intr_Stop(void);

CY_ISR_PROTO(Front_Echo_Intr_Interrupt);

void Front_Echo_Intr_SetVector(cyisraddress address);
cyisraddress Front_Echo_Intr_GetVector(void);

void Front_Echo_Intr_SetPriority(uint8 priority);
uint8 Front_Echo_Intr_GetPriority(void);

void Front_Echo_Intr_Enable(void);
uint8 Front_Echo_Intr_GetState(void);
void Front_Echo_Intr_Disable(void);

void Front_Echo_Intr_SetPending(void);
void Front_Echo_Intr_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Front_Echo_Intr ISR. */
#define Front_Echo_Intr_INTC_VECTOR            ((reg32 *) Front_Echo_Intr__INTC_VECT)

/* Address of the Front_Echo_Intr ISR priority. */
#define Front_Echo_Intr_INTC_PRIOR             ((reg8 *) Front_Echo_Intr__INTC_PRIOR_REG)

/* Priority of the Front_Echo_Intr interrupt. */
#define Front_Echo_Intr_INTC_PRIOR_NUMBER      Front_Echo_Intr__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Front_Echo_Intr interrupt. */
#define Front_Echo_Intr_INTC_SET_EN            ((reg32 *) Front_Echo_Intr__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Front_Echo_Intr interrupt. */
#define Front_Echo_Intr_INTC_CLR_EN            ((reg32 *) Front_Echo_Intr__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Front_Echo_Intr interrupt state to pending. */
#define Front_Echo_Intr_INTC_SET_PD            ((reg32 *) Front_Echo_Intr__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Front_Echo_Intr interrupt. */
#define Front_Echo_Intr_INTC_CLR_PD            ((reg32 *) Front_Echo_Intr__INTC_CLR_PD_REG)


#endif /* CY_ISR_Front_Echo_Intr_H */


/* [] END OF FILE */
