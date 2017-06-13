/*******************************************************************************
* File Name: UpperArmTiltRotatePWM_PM.c
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

#include "UpperArmTiltRotatePWM.h"

static UpperArmTiltRotatePWM_backupStruct UpperArmTiltRotatePWM_backup;


/*******************************************************************************
* Function Name: UpperArmTiltRotatePWM_SaveConfig
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
*  UpperArmTiltRotatePWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void UpperArmTiltRotatePWM_SaveConfig(void) 
{

    #if(!UpperArmTiltRotatePWM_UsingFixedFunction)
        #if(!UpperArmTiltRotatePWM_PWMModeIsCenterAligned)
            UpperArmTiltRotatePWM_backup.PWMPeriod = UpperArmTiltRotatePWM_ReadPeriod();
        #endif /* (!UpperArmTiltRotatePWM_PWMModeIsCenterAligned) */
        UpperArmTiltRotatePWM_backup.PWMUdb = UpperArmTiltRotatePWM_ReadCounter();
        #if (UpperArmTiltRotatePWM_UseStatus)
            UpperArmTiltRotatePWM_backup.InterruptMaskValue = UpperArmTiltRotatePWM_STATUS_MASK;
        #endif /* (UpperArmTiltRotatePWM_UseStatus) */

        #if(UpperArmTiltRotatePWM_DeadBandMode == UpperArmTiltRotatePWM__B_PWM__DBM_256_CLOCKS || \
            UpperArmTiltRotatePWM_DeadBandMode == UpperArmTiltRotatePWM__B_PWM__DBM_2_4_CLOCKS)
            UpperArmTiltRotatePWM_backup.PWMdeadBandValue = UpperArmTiltRotatePWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(UpperArmTiltRotatePWM_KillModeMinTime)
             UpperArmTiltRotatePWM_backup.PWMKillCounterPeriod = UpperArmTiltRotatePWM_ReadKillTime();
        #endif /* (UpperArmTiltRotatePWM_KillModeMinTime) */

        #if(UpperArmTiltRotatePWM_UseControl)
            UpperArmTiltRotatePWM_backup.PWMControlRegister = UpperArmTiltRotatePWM_ReadControlRegister();
        #endif /* (UpperArmTiltRotatePWM_UseControl) */
    #endif  /* (!UpperArmTiltRotatePWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: UpperArmTiltRotatePWM_RestoreConfig
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
*  UpperArmTiltRotatePWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void UpperArmTiltRotatePWM_RestoreConfig(void) 
{
        #if(!UpperArmTiltRotatePWM_UsingFixedFunction)
            #if(!UpperArmTiltRotatePWM_PWMModeIsCenterAligned)
                UpperArmTiltRotatePWM_WritePeriod(UpperArmTiltRotatePWM_backup.PWMPeriod);
            #endif /* (!UpperArmTiltRotatePWM_PWMModeIsCenterAligned) */

            UpperArmTiltRotatePWM_WriteCounter(UpperArmTiltRotatePWM_backup.PWMUdb);

            #if (UpperArmTiltRotatePWM_UseStatus)
                UpperArmTiltRotatePWM_STATUS_MASK = UpperArmTiltRotatePWM_backup.InterruptMaskValue;
            #endif /* (UpperArmTiltRotatePWM_UseStatus) */

            #if(UpperArmTiltRotatePWM_DeadBandMode == UpperArmTiltRotatePWM__B_PWM__DBM_256_CLOCKS || \
                UpperArmTiltRotatePWM_DeadBandMode == UpperArmTiltRotatePWM__B_PWM__DBM_2_4_CLOCKS)
                UpperArmTiltRotatePWM_WriteDeadTime(UpperArmTiltRotatePWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(UpperArmTiltRotatePWM_KillModeMinTime)
                UpperArmTiltRotatePWM_WriteKillTime(UpperArmTiltRotatePWM_backup.PWMKillCounterPeriod);
            #endif /* (UpperArmTiltRotatePWM_KillModeMinTime) */

            #if(UpperArmTiltRotatePWM_UseControl)
                UpperArmTiltRotatePWM_WriteControlRegister(UpperArmTiltRotatePWM_backup.PWMControlRegister);
            #endif /* (UpperArmTiltRotatePWM_UseControl) */
        #endif  /* (!UpperArmTiltRotatePWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: UpperArmTiltRotatePWM_Sleep
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
*  UpperArmTiltRotatePWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void UpperArmTiltRotatePWM_Sleep(void) 
{
    #if(UpperArmTiltRotatePWM_UseControl)
        if(UpperArmTiltRotatePWM_CTRL_ENABLE == (UpperArmTiltRotatePWM_CONTROL & UpperArmTiltRotatePWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            UpperArmTiltRotatePWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            UpperArmTiltRotatePWM_backup.PWMEnableState = 0u;
        }
    #endif /* (UpperArmTiltRotatePWM_UseControl) */

    /* Stop component */
    UpperArmTiltRotatePWM_Stop();

    /* Save registers configuration */
    UpperArmTiltRotatePWM_SaveConfig();
}


/*******************************************************************************
* Function Name: UpperArmTiltRotatePWM_Wakeup
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
*  UpperArmTiltRotatePWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void UpperArmTiltRotatePWM_Wakeup(void) 
{
     /* Restore registers values */
    UpperArmTiltRotatePWM_RestoreConfig();

    if(UpperArmTiltRotatePWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        UpperArmTiltRotatePWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
