/*******************************************************************************
* File Name: Rear_Trigger_Reg_PM.c
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

#include "Rear_Trigger_Reg.h"

/* Check for removal by optimization */
#if !defined(Rear_Trigger_Reg_Sync_ctrl_reg__REMOVED)

static Rear_Trigger_Reg_BACKUP_STRUCT  Rear_Trigger_Reg_backup = {0u};

    
/*******************************************************************************
* Function Name: Rear_Trigger_Reg_SaveConfig
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
void Rear_Trigger_Reg_SaveConfig(void) 
{
    Rear_Trigger_Reg_backup.controlState = Rear_Trigger_Reg_Control;
}


/*******************************************************************************
* Function Name: Rear_Trigger_Reg_RestoreConfig
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
void Rear_Trigger_Reg_RestoreConfig(void) 
{
     Rear_Trigger_Reg_Control = Rear_Trigger_Reg_backup.controlState;
}


/*******************************************************************************
* Function Name: Rear_Trigger_Reg_Sleep
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
void Rear_Trigger_Reg_Sleep(void) 
{
    Rear_Trigger_Reg_SaveConfig();
}


/*******************************************************************************
* Function Name: Rear_Trigger_Reg_Wakeup
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
void Rear_Trigger_Reg_Wakeup(void)  
{
    Rear_Trigger_Reg_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
