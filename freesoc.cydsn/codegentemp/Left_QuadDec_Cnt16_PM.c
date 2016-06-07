/*******************************************************************************
* File Name: Left_QuadDec_Cnt16_PM.c  
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

#include "Left_QuadDec_Cnt16.h"

static Left_QuadDec_Cnt16_backupStruct Left_QuadDec_Cnt16_backup;


/*******************************************************************************
* Function Name: Left_QuadDec_Cnt16_SaveConfig
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
*  Left_QuadDec_Cnt16_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Left_QuadDec_Cnt16_SaveConfig(void) 
{
    #if (!Left_QuadDec_Cnt16_UsingFixedFunction)

        Left_QuadDec_Cnt16_backup.CounterUdb = Left_QuadDec_Cnt16_ReadCounter();

        #if(!Left_QuadDec_Cnt16_ControlRegRemoved)
            Left_QuadDec_Cnt16_backup.CounterControlRegister = Left_QuadDec_Cnt16_ReadControlRegister();
        #endif /* (!Left_QuadDec_Cnt16_ControlRegRemoved) */

    #endif /* (!Left_QuadDec_Cnt16_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Left_QuadDec_Cnt16_RestoreConfig
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
*  Left_QuadDec_Cnt16_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Left_QuadDec_Cnt16_RestoreConfig(void) 
{      
    #if (!Left_QuadDec_Cnt16_UsingFixedFunction)

       Left_QuadDec_Cnt16_WriteCounter(Left_QuadDec_Cnt16_backup.CounterUdb);

        #if(!Left_QuadDec_Cnt16_ControlRegRemoved)
            Left_QuadDec_Cnt16_WriteControlRegister(Left_QuadDec_Cnt16_backup.CounterControlRegister);
        #endif /* (!Left_QuadDec_Cnt16_ControlRegRemoved) */

    #endif /* (!Left_QuadDec_Cnt16_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Left_QuadDec_Cnt16_Sleep
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
*  Left_QuadDec_Cnt16_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Left_QuadDec_Cnt16_Sleep(void) 
{
    #if(!Left_QuadDec_Cnt16_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Left_QuadDec_Cnt16_CTRL_ENABLE == (Left_QuadDec_Cnt16_CONTROL & Left_QuadDec_Cnt16_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Left_QuadDec_Cnt16_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Left_QuadDec_Cnt16_backup.CounterEnableState = 0u;
        }
    #else
        Left_QuadDec_Cnt16_backup.CounterEnableState = 1u;
        if(Left_QuadDec_Cnt16_backup.CounterEnableState != 0u)
        {
            Left_QuadDec_Cnt16_backup.CounterEnableState = 0u;
        }
    #endif /* (!Left_QuadDec_Cnt16_ControlRegRemoved) */
    
    Left_QuadDec_Cnt16_Stop();
    Left_QuadDec_Cnt16_SaveConfig();
}


/*******************************************************************************
* Function Name: Left_QuadDec_Cnt16_Wakeup
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
*  Left_QuadDec_Cnt16_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Left_QuadDec_Cnt16_Wakeup(void) 
{
    Left_QuadDec_Cnt16_RestoreConfig();
    #if(!Left_QuadDec_Cnt16_ControlRegRemoved)
        if(Left_QuadDec_Cnt16_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Left_QuadDec_Cnt16_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Left_QuadDec_Cnt16_ControlRegRemoved) */
    
}


/* [] END OF FILE */
