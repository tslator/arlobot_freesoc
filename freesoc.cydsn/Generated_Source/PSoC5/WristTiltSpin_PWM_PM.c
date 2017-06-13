/*******************************************************************************
* File Name: WristTiltSpin_PWM_PM.c
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

#include "WristTiltSpin_PWM.h"

static WristTiltSpin_PWM_backupStruct WristTiltSpin_PWM_backup;


/*******************************************************************************
* Function Name: WristTiltSpin_PWM_SaveConfig
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
*  WristTiltSpin_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void WristTiltSpin_PWM_SaveConfig(void) 
{

    #if(!WristTiltSpin_PWM_UsingFixedFunction)
        #if(!WristTiltSpin_PWM_PWMModeIsCenterAligned)
            WristTiltSpin_PWM_backup.PWMPeriod = WristTiltSpin_PWM_ReadPeriod();
        #endif /* (!WristTiltSpin_PWM_PWMModeIsCenterAligned) */
        WristTiltSpin_PWM_backup.PWMUdb = WristTiltSpin_PWM_ReadCounter();
        #if (WristTiltSpin_PWM_UseStatus)
            WristTiltSpin_PWM_backup.InterruptMaskValue = WristTiltSpin_PWM_STATUS_MASK;
        #endif /* (WristTiltSpin_PWM_UseStatus) */

        #if(WristTiltSpin_PWM_DeadBandMode == WristTiltSpin_PWM__B_PWM__DBM_256_CLOCKS || \
            WristTiltSpin_PWM_DeadBandMode == WristTiltSpin_PWM__B_PWM__DBM_2_4_CLOCKS)
            WristTiltSpin_PWM_backup.PWMdeadBandValue = WristTiltSpin_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(WristTiltSpin_PWM_KillModeMinTime)
             WristTiltSpin_PWM_backup.PWMKillCounterPeriod = WristTiltSpin_PWM_ReadKillTime();
        #endif /* (WristTiltSpin_PWM_KillModeMinTime) */

        #if(WristTiltSpin_PWM_UseControl)
            WristTiltSpin_PWM_backup.PWMControlRegister = WristTiltSpin_PWM_ReadControlRegister();
        #endif /* (WristTiltSpin_PWM_UseControl) */
    #endif  /* (!WristTiltSpin_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: WristTiltSpin_PWM_RestoreConfig
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
*  WristTiltSpin_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void WristTiltSpin_PWM_RestoreConfig(void) 
{
        #if(!WristTiltSpin_PWM_UsingFixedFunction)
            #if(!WristTiltSpin_PWM_PWMModeIsCenterAligned)
                WristTiltSpin_PWM_WritePeriod(WristTiltSpin_PWM_backup.PWMPeriod);
            #endif /* (!WristTiltSpin_PWM_PWMModeIsCenterAligned) */

            WristTiltSpin_PWM_WriteCounter(WristTiltSpin_PWM_backup.PWMUdb);

            #if (WristTiltSpin_PWM_UseStatus)
                WristTiltSpin_PWM_STATUS_MASK = WristTiltSpin_PWM_backup.InterruptMaskValue;
            #endif /* (WristTiltSpin_PWM_UseStatus) */

            #if(WristTiltSpin_PWM_DeadBandMode == WristTiltSpin_PWM__B_PWM__DBM_256_CLOCKS || \
                WristTiltSpin_PWM_DeadBandMode == WristTiltSpin_PWM__B_PWM__DBM_2_4_CLOCKS)
                WristTiltSpin_PWM_WriteDeadTime(WristTiltSpin_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(WristTiltSpin_PWM_KillModeMinTime)
                WristTiltSpin_PWM_WriteKillTime(WristTiltSpin_PWM_backup.PWMKillCounterPeriod);
            #endif /* (WristTiltSpin_PWM_KillModeMinTime) */

            #if(WristTiltSpin_PWM_UseControl)
                WristTiltSpin_PWM_WriteControlRegister(WristTiltSpin_PWM_backup.PWMControlRegister);
            #endif /* (WristTiltSpin_PWM_UseControl) */
        #endif  /* (!WristTiltSpin_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: WristTiltSpin_PWM_Sleep
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
*  WristTiltSpin_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void WristTiltSpin_PWM_Sleep(void) 
{
    #if(WristTiltSpin_PWM_UseControl)
        if(WristTiltSpin_PWM_CTRL_ENABLE == (WristTiltSpin_PWM_CONTROL & WristTiltSpin_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            WristTiltSpin_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            WristTiltSpin_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (WristTiltSpin_PWM_UseControl) */

    /* Stop component */
    WristTiltSpin_PWM_Stop();

    /* Save registers configuration */
    WristTiltSpin_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: WristTiltSpin_PWM_Wakeup
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
*  WristTiltSpin_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void WristTiltSpin_PWM_Wakeup(void) 
{
     /* Restore registers values */
    WristTiltSpin_PWM_RestoreConfig();

    if(WristTiltSpin_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        WristTiltSpin_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
