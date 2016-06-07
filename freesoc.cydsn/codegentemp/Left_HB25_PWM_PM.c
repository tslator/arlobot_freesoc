/*******************************************************************************
* File Name: Left_HB25_PWM_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Left_HB25_PWM.h"

static Left_HB25_PWM_backupStruct Left_HB25_PWM_backup;


/*******************************************************************************
* Function Name: Left_HB25_PWM_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Left_HB25_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Left_HB25_PWM_SaveConfig(void) 
{

    #if(!Left_HB25_PWM_UsingFixedFunction)
        #if(!Left_HB25_PWM_PWMModeIsCenterAligned)
            Left_HB25_PWM_backup.PWMPeriod = Left_HB25_PWM_ReadPeriod();
        #endif /* (!Left_HB25_PWM_PWMModeIsCenterAligned) */
        Left_HB25_PWM_backup.PWMUdb = Left_HB25_PWM_ReadCounter();
        #if (Left_HB25_PWM_UseStatus)
            Left_HB25_PWM_backup.InterruptMaskValue = Left_HB25_PWM_STATUS_MASK;
        #endif /* (Left_HB25_PWM_UseStatus) */

        #if(Left_HB25_PWM_DeadBandMode == Left_HB25_PWM__B_PWM__DBM_256_CLOCKS || \
            Left_HB25_PWM_DeadBandMode == Left_HB25_PWM__B_PWM__DBM_2_4_CLOCKS)
            Left_HB25_PWM_backup.PWMdeadBandValue = Left_HB25_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(Left_HB25_PWM_KillModeMinTime)
             Left_HB25_PWM_backup.PWMKillCounterPeriod = Left_HB25_PWM_ReadKillTime();
        #endif /* (Left_HB25_PWM_KillModeMinTime) */

        #if(Left_HB25_PWM_UseControl)
            Left_HB25_PWM_backup.PWMControlRegister = Left_HB25_PWM_ReadControlRegister();
        #endif /* (Left_HB25_PWM_UseControl) */
    #endif  /* (!Left_HB25_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Left_HB25_PWM_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Left_HB25_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Left_HB25_PWM_RestoreConfig(void) 
{
        #if(!Left_HB25_PWM_UsingFixedFunction)
            #if(!Left_HB25_PWM_PWMModeIsCenterAligned)
                Left_HB25_PWM_WritePeriod(Left_HB25_PWM_backup.PWMPeriod);
            #endif /* (!Left_HB25_PWM_PWMModeIsCenterAligned) */

            Left_HB25_PWM_WriteCounter(Left_HB25_PWM_backup.PWMUdb);

            #if (Left_HB25_PWM_UseStatus)
                Left_HB25_PWM_STATUS_MASK = Left_HB25_PWM_backup.InterruptMaskValue;
            #endif /* (Left_HB25_PWM_UseStatus) */

            #if(Left_HB25_PWM_DeadBandMode == Left_HB25_PWM__B_PWM__DBM_256_CLOCKS || \
                Left_HB25_PWM_DeadBandMode == Left_HB25_PWM__B_PWM__DBM_2_4_CLOCKS)
                Left_HB25_PWM_WriteDeadTime(Left_HB25_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(Left_HB25_PWM_KillModeMinTime)
                Left_HB25_PWM_WriteKillTime(Left_HB25_PWM_backup.PWMKillCounterPeriod);
            #endif /* (Left_HB25_PWM_KillModeMinTime) */

            #if(Left_HB25_PWM_UseControl)
                Left_HB25_PWM_WriteControlRegister(Left_HB25_PWM_backup.PWMControlRegister);
            #endif /* (Left_HB25_PWM_UseControl) */
        #endif  /* (!Left_HB25_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: Left_HB25_PWM_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Left_HB25_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Left_HB25_PWM_Sleep(void) 
{
    #if(Left_HB25_PWM_UseControl)
        if(Left_HB25_PWM_CTRL_ENABLE == (Left_HB25_PWM_CONTROL & Left_HB25_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            Left_HB25_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            Left_HB25_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (Left_HB25_PWM_UseControl) */

    /* Stop component */
    Left_HB25_PWM_Stop();

    /* Save registers configuration */
    Left_HB25_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: Left_HB25_PWM_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Left_HB25_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Left_HB25_PWM_Wakeup(void) 
{
     /* Restore registers values */
    Left_HB25_PWM_RestoreConfig();

    if(Left_HB25_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        Left_HB25_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
