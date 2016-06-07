/*******************************************************************************
* File Name: Right_QuadDec_Cnt16_PM.c  
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

#include "Right_QuadDec_Cnt16.h"

static Right_QuadDec_Cnt16_backupStruct Right_QuadDec_Cnt16_backup;


/*******************************************************************************
* Function Name: Right_QuadDec_Cnt16_SaveConfig
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
*  Right_QuadDec_Cnt16_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Right_QuadDec_Cnt16_SaveConfig(void) 
{
    #if (!Right_QuadDec_Cnt16_UsingFixedFunction)

        Right_QuadDec_Cnt16_backup.CounterUdb = Right_QuadDec_Cnt16_ReadCounter();

        #if(!Right_QuadDec_Cnt16_ControlRegRemoved)
            Right_QuadDec_Cnt16_backup.CounterControlRegister = Right_QuadDec_Cnt16_ReadControlRegister();
        #endif /* (!Right_QuadDec_Cnt16_ControlRegRemoved) */

    #endif /* (!Right_QuadDec_Cnt16_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Right_QuadDec_Cnt16_RestoreConfig
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
*  Right_QuadDec_Cnt16_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Right_QuadDec_Cnt16_RestoreConfig(void) 
{      
    #if (!Right_QuadDec_Cnt16_UsingFixedFunction)

       Right_QuadDec_Cnt16_WriteCounter(Right_QuadDec_Cnt16_backup.CounterUdb);

        #if(!Right_QuadDec_Cnt16_ControlRegRemoved)
            Right_QuadDec_Cnt16_WriteControlRegister(Right_QuadDec_Cnt16_backup.CounterControlRegister);
        #endif /* (!Right_QuadDec_Cnt16_ControlRegRemoved) */

    #endif /* (!Right_QuadDec_Cnt16_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Right_QuadDec_Cnt16_Sleep
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
*  Right_QuadDec_Cnt16_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Right_QuadDec_Cnt16_Sleep(void) 
{
    #if(!Right_QuadDec_Cnt16_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Right_QuadDec_Cnt16_CTRL_ENABLE == (Right_QuadDec_Cnt16_CONTROL & Right_QuadDec_Cnt16_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Right_QuadDec_Cnt16_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Right_QuadDec_Cnt16_backup.CounterEnableState = 0u;
        }
    #else
        Right_QuadDec_Cnt16_backup.CounterEnableState = 1u;
        if(Right_QuadDec_Cnt16_backup.CounterEnableState != 0u)
        {
            Right_QuadDec_Cnt16_backup.CounterEnableState = 0u;
        }
    #endif /* (!Right_QuadDec_Cnt16_ControlRegRemoved) */
    
    Right_QuadDec_Cnt16_Stop();
    Right_QuadDec_Cnt16_SaveConfig();
}


/*******************************************************************************
* Function Name: Right_QuadDec_Cnt16_Wakeup
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
*  Right_QuadDec_Cnt16_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Right_QuadDec_Cnt16_Wakeup(void) 
{
    Right_QuadDec_Cnt16_RestoreConfig();
    #if(!Right_QuadDec_Cnt16_ControlRegRemoved)
        if(Right_QuadDec_Cnt16_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Right_QuadDec_Cnt16_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Right_QuadDec_Cnt16_ControlRegRemoved) */
    
}


/* [] END OF FILE */
