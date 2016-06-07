/*******************************************************************************
* File Name: Front_Trigger_Reg.h  
* Version 1.80
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CONTROL_REG_Front_Trigger_Reg_H) /* CY_CONTROL_REG_Front_Trigger_Reg_H */
#define CY_CONTROL_REG_Front_Trigger_Reg_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} Front_Trigger_Reg_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    Front_Trigger_Reg_Write(uint8 control) ;
uint8   Front_Trigger_Reg_Read(void) ;

void Front_Trigger_Reg_SaveConfig(void) ;
void Front_Trigger_Reg_RestoreConfig(void) ;
void Front_Trigger_Reg_Sleep(void) ; 
void Front_Trigger_Reg_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define Front_Trigger_Reg_Control        (* (reg8 *) Front_Trigger_Reg_Sync_ctrl_reg__CONTROL_REG )
#define Front_Trigger_Reg_Control_PTR    (  (reg8 *) Front_Trigger_Reg_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_Front_Trigger_Reg_H */


/* [] END OF FILE */
