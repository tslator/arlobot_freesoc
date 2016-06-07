/*******************************************************************************
* File Name: Right_Phase_Counter_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Right_Phase_Counter.h"

static Right_Phase_Counter_backupStruct Right_Phase_Counter_backup;


/*******************************************************************************
* Function Name: Right_Phase_Counter_SaveConfig
********************************************************************************
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
*  Right_Phase_Counter_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Right_Phase_Counter_SaveConfig(void) 
{
    #if (!Right_Phase_Counter_UsingFixedFunction)

        Right_Phase_Counter_backup.CounterUdb = Right_Phase_Counter_ReadCounter();

        #if(!Right_Phase_Counter_ControlRegRemoved)
            Right_Phase_Counter_backup.CounterControlRegister = Right_Phase_Counter_ReadControlRegister();
        #endif /* (!Right_Phase_Counter_ControlRegRemoved) */

    #endif /* (!Right_Phase_Counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Right_Phase_Counter_RestoreConfig
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
*  Right_Phase_Counter_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Right_Phase_Counter_RestoreConfig(void) 
{      
    #if (!Right_Phase_Counter_UsingFixedFunction)

       Right_Phase_Counter_WriteCounter(Right_Phase_Counter_backup.CounterUdb);

        #if(!Right_Phase_Counter_ControlRegRemoved)
            Right_Phase_Counter_WriteControlRegister(Right_Phase_Counter_backup.CounterControlRegister);
        #endif /* (!Right_Phase_Counter_ControlRegRemoved) */

    #endif /* (!Right_Phase_Counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Right_Phase_Counter_Sleep
********************************************************************************
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
*  Right_Phase_Counter_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Right_Phase_Counter_Sleep(void) 
{
    #if(!Right_Phase_Counter_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Right_Phase_Counter_CTRL_ENABLE == (Right_Phase_Counter_CONTROL & Right_Phase_Counter_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Right_Phase_Counter_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Right_Phase_Counter_backup.CounterEnableState = 0u;
        }
    #else
        Right_Phase_Counter_backup.CounterEnableState = 1u;
        if(Right_Phase_Counter_backup.CounterEnableState != 0u)
        {
            Right_Phase_Counter_backup.CounterEnableState = 0u;
        }
    #endif /* (!Right_Phase_Counter_ControlRegRemoved) */
    
    Right_Phase_Counter_Stop();
    Right_Phase_Counter_SaveConfig();
}


/*******************************************************************************
* Function Name: Right_Phase_Counter_Wakeup
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
*  Right_Phase_Counter_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Right_Phase_Counter_Wakeup(void) 
{
    Right_Phase_Counter_RestoreConfig();
    #if(!Right_Phase_Counter_ControlRegRemoved)
        if(Right_Phase_Counter_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Right_Phase_Counter_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Right_Phase_Counter_ControlRegRemoved) */
    
}


/* [] END OF FILE */
