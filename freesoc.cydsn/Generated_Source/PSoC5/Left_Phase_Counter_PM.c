/*******************************************************************************
* File Name: Left_Phase_Counter_PM.c  
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

#include "Left_Phase_Counter.h"

static Left_Phase_Counter_backupStruct Left_Phase_Counter_backup;


/*******************************************************************************
* Function Name: Left_Phase_Counter_SaveConfig
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
*  Left_Phase_Counter_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Left_Phase_Counter_SaveConfig(void) 
{
    #if (!Left_Phase_Counter_UsingFixedFunction)

        Left_Phase_Counter_backup.CounterUdb = Left_Phase_Counter_ReadCounter();

        #if(!Left_Phase_Counter_ControlRegRemoved)
            Left_Phase_Counter_backup.CounterControlRegister = Left_Phase_Counter_ReadControlRegister();
        #endif /* (!Left_Phase_Counter_ControlRegRemoved) */

    #endif /* (!Left_Phase_Counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Left_Phase_Counter_RestoreConfig
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
*  Left_Phase_Counter_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Left_Phase_Counter_RestoreConfig(void) 
{      
    #if (!Left_Phase_Counter_UsingFixedFunction)

       Left_Phase_Counter_WriteCounter(Left_Phase_Counter_backup.CounterUdb);

        #if(!Left_Phase_Counter_ControlRegRemoved)
            Left_Phase_Counter_WriteControlRegister(Left_Phase_Counter_backup.CounterControlRegister);
        #endif /* (!Left_Phase_Counter_ControlRegRemoved) */

    #endif /* (!Left_Phase_Counter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Left_Phase_Counter_Sleep
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
*  Left_Phase_Counter_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Left_Phase_Counter_Sleep(void) 
{
    #if(!Left_Phase_Counter_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Left_Phase_Counter_CTRL_ENABLE == (Left_Phase_Counter_CONTROL & Left_Phase_Counter_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Left_Phase_Counter_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Left_Phase_Counter_backup.CounterEnableState = 0u;
        }
    #else
        Left_Phase_Counter_backup.CounterEnableState = 1u;
        if(Left_Phase_Counter_backup.CounterEnableState != 0u)
        {
            Left_Phase_Counter_backup.CounterEnableState = 0u;
        }
    #endif /* (!Left_Phase_Counter_ControlRegRemoved) */
    
    Left_Phase_Counter_Stop();
    Left_Phase_Counter_SaveConfig();
}


/*******************************************************************************
* Function Name: Left_Phase_Counter_Wakeup
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
*  Left_Phase_Counter_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Left_Phase_Counter_Wakeup(void) 
{
    Left_Phase_Counter_RestoreConfig();
    #if(!Left_Phase_Counter_ControlRegRemoved)
        if(Left_Phase_Counter_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Left_Phase_Counter_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Left_Phase_Counter_ControlRegRemoved) */
    
}


/* [] END OF FILE */
