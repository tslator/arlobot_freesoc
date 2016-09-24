/*******************************************************************************
* File Name: Slave_Select.h  
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

#if !defined(CY_CONTROL_REG_Slave_Select_H) /* CY_CONTROL_REG_Slave_Select_H */
#define CY_CONTROL_REG_Slave_Select_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} Slave_Select_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    Slave_Select_Write(uint8 control) ;
uint8   Slave_Select_Read(void) ;

void Slave_Select_SaveConfig(void) ;
void Slave_Select_RestoreConfig(void) ;
void Slave_Select_Sleep(void) ; 
void Slave_Select_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define Slave_Select_Control        (* (reg8 *) Slave_Select_Sync_ctrl_reg__CONTROL_REG )
#define Slave_Select_Control_PTR    (  (reg8 *) Slave_Select_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_Slave_Select_H */


/* [] END OF FILE */
