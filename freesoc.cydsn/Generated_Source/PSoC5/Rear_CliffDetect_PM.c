/*******************************************************************************
* File Name: Rear_CliffDetect.c
* Version 2.0
*
* Description:
*  This file provides the power management source code APIs for the
*  Comparator.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Rear_CliffDetect.h"

static Rear_CliffDetect_backupStruct Rear_CliffDetect_backup;


/*******************************************************************************
* Function Name: Rear_CliffDetect_SaveConfig
********************************************************************************
*
* Summary:
*  Save the current user configuration
*
* Parameters:
*  void:
*
* Return:
*  void
*
*******************************************************************************/
void Rear_CliffDetect_SaveConfig(void) 
{
    /* Empty since all are system reset for retention flops */
}


/*******************************************************************************
* Function Name: Rear_CliffDetect_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
********************************************************************************/
void Rear_CliffDetect_RestoreConfig(void) 
{
    /* Empty since all are system reset for retention flops */    
}


/*******************************************************************************
* Function Name: Rear_CliffDetect_Sleep
********************************************************************************
*
* Summary:
*  Stop and Save the user configuration
*
* Parameters:
*  void:
*
* Return:
*  void
*
* Global variables:
*  Rear_CliffDetect_backup.enableState:  Is modified depending on the enable 
*   state of the block before entering sleep mode.
*
*******************************************************************************/
void Rear_CliffDetect_Sleep(void) 
{
    /* Save Comp's enable state */    
    if(Rear_CliffDetect_ACT_PWR_EN == (Rear_CliffDetect_PWRMGR & Rear_CliffDetect_ACT_PWR_EN))
    {
        /* Comp is enabled */
        Rear_CliffDetect_backup.enableState = 1u;
    }
    else
    {
        /* Comp is disabled */
        Rear_CliffDetect_backup.enableState = 0u;
    }    
    
    Rear_CliffDetect_Stop();
    Rear_CliffDetect_SaveConfig();
}


/*******************************************************************************
* Function Name: Rear_CliffDetect_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Rear_CliffDetect_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Rear_CliffDetect_Wakeup(void) 
{
    Rear_CliffDetect_RestoreConfig();
    
    if(Rear_CliffDetect_backup.enableState == 1u)
    {
        /* Enable Comp's operation */
        Rear_CliffDetect_Enable();

    } /* Do nothing if Comp was disabled before */ 
}


/* [] END OF FILE */
