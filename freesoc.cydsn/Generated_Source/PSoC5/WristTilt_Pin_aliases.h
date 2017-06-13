/*******************************************************************************
* File Name: WristTilt_Pin.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_WristTilt_Pin_ALIASES_H) /* Pins WristTilt_Pin_ALIASES_H */
#define CY_PINS_WristTilt_Pin_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define WristTilt_Pin_0			(WristTilt_Pin__0__PC)
#define WristTilt_Pin_0_INTR	((uint16)((uint16)0x0001u << WristTilt_Pin__0__SHIFT))

#define WristTilt_Pin_INTR_ALL	 ((uint16)(WristTilt_Pin_0_INTR))

#endif /* End Pins WristTilt_Pin_ALIASES_H */


/* [] END OF FILE */
