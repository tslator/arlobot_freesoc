/*******************************************************************************
* File Name: Front_Echo_Timer_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Front_Echo_Timer.h"

static Front_Echo_Timer_backupStruct Front_Echo_Timer_backup;


/*******************************************************************************
* Function Name: Front_Echo_Timer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Front_Echo_Timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Front_Echo_Timer_SaveConfig(void) 
{
    #if (!Front_Echo_Timer_UsingFixedFunction)
        Front_Echo_Timer_backup.TimerUdb = Front_Echo_Timer_ReadCounter();
        Front_Echo_Timer_backup.InterruptMaskValue = Front_Echo_Timer_STATUS_MASK;
        #if (Front_Echo_Timer_UsingHWCaptureCounter)
            Front_Echo_Timer_backup.TimerCaptureCounter = Front_Echo_Timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Front_Echo_Timer_UDB_CONTROL_REG_REMOVED)
            Front_Echo_Timer_backup.TimerControlRegister = Front_Echo_Timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Front_Echo_Timer_RestoreConfig
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
* Global variables:
*  Front_Echo_Timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Front_Echo_Timer_RestoreConfig(void) 
{   
    #if (!Front_Echo_Timer_UsingFixedFunction)

        Front_Echo_Timer_WriteCounter(Front_Echo_Timer_backup.TimerUdb);
        Front_Echo_Timer_STATUS_MASK =Front_Echo_Timer_backup.InterruptMaskValue;
        #if (Front_Echo_Timer_UsingHWCaptureCounter)
            Front_Echo_Timer_SetCaptureCount(Front_Echo_Timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Front_Echo_Timer_UDB_CONTROL_REG_REMOVED)
            Front_Echo_Timer_WriteControlRegister(Front_Echo_Timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Front_Echo_Timer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Front_Echo_Timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Front_Echo_Timer_Sleep(void) 
{
    #if(!Front_Echo_Timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Front_Echo_Timer_CTRL_ENABLE == (Front_Echo_Timer_CONTROL & Front_Echo_Timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Front_Echo_Timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Front_Echo_Timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Front_Echo_Timer_Stop();
    Front_Echo_Timer_SaveConfig();
}


/*******************************************************************************
* Function Name: Front_Echo_Timer_Wakeup
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
*  Front_Echo_Timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Front_Echo_Timer_Wakeup(void) 
{
    Front_Echo_Timer_RestoreConfig();
    #if(!Front_Echo_Timer_UDB_CONTROL_REG_REMOVED)
        if(Front_Echo_Timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Front_Echo_Timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
