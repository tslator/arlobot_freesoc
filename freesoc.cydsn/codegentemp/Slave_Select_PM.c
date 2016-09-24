/*******************************************************************************
* File Name: Slave_Select_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Slave_Select.h"

/* Check for removal by optimization */
#if !defined(Slave_Select_Sync_ctrl_reg__REMOVED)

static Slave_Select_BACKUP_STRUCT  Slave_Select_backup = {0u};

    
/*******************************************************************************
* Function Name: Slave_Select_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Slave_Select_SaveConfig(void) 
{
    Slave_Select_backup.controlState = Slave_Select_Control;
}


/*******************************************************************************
* Function Name: Slave_Select_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void Slave_Select_RestoreConfig(void) 
{
     Slave_Select_Control = Slave_Select_backup.controlState;
}


/*******************************************************************************
* Function Name: Slave_Select_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Slave_Select_Sleep(void) 
{
    Slave_Select_SaveConfig();
}


/*******************************************************************************
* Function Name: Slave_Select_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Slave_Select_Wakeup(void)  
{
    Slave_Select_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
