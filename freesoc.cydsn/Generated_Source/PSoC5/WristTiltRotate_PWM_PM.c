/*******************************************************************************
* File Name: WristTiltRotate_PWM_PM.c
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

#include "WristTiltRotate_PWM.h"

static WristTiltRotate_PWM_backupStruct WristTiltRotate_PWM_backup;


/*******************************************************************************
* Function Name: WristTiltRotate_PWM_SaveConfig
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
*  WristTiltRotate_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void WristTiltRotate_PWM_SaveConfig(void) 
{

    #if(!WristTiltRotate_PWM_UsingFixedFunction)
        #if(!WristTiltRotate_PWM_PWMModeIsCenterAligned)
            WristTiltRotate_PWM_backup.PWMPeriod = WristTiltRotate_PWM_ReadPeriod();
        #endif /* (!WristTiltRotate_PWM_PWMModeIsCenterAligned) */
        WristTiltRotate_PWM_backup.PWMUdb = WristTiltRotate_PWM_ReadCounter();
        #if (WristTiltRotate_PWM_UseStatus)
            WristTiltRotate_PWM_backup.InterruptMaskValue = WristTiltRotate_PWM_STATUS_MASK;
        #endif /* (WristTiltRotate_PWM_UseStatus) */

        #if(WristTiltRotate_PWM_DeadBandMode == WristTiltRotate_PWM__B_PWM__DBM_256_CLOCKS || \
            WristTiltRotate_PWM_DeadBandMode == WristTiltRotate_PWM__B_PWM__DBM_2_4_CLOCKS)
            WristTiltRotate_PWM_backup.PWMdeadBandValue = WristTiltRotate_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(WristTiltRotate_PWM_KillModeMinTime)
             WristTiltRotate_PWM_backup.PWMKillCounterPeriod = WristTiltRotate_PWM_ReadKillTime();
        #endif /* (WristTiltRotate_PWM_KillModeMinTime) */

        #if(WristTiltRotate_PWM_UseControl)
            WristTiltRotate_PWM_backup.PWMControlRegister = WristTiltRotate_PWM_ReadControlRegister();
        #endif /* (WristTiltRotate_PWM_UseControl) */
    #endif  /* (!WristTiltRotate_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: WristTiltRotate_PWM_RestoreConfig
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
*  WristTiltRotate_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void WristTiltRotate_PWM_RestoreConfig(void) 
{
        #if(!WristTiltRotate_PWM_UsingFixedFunction)
            #if(!WristTiltRotate_PWM_PWMModeIsCenterAligned)
                WristTiltRotate_PWM_WritePeriod(WristTiltRotate_PWM_backup.PWMPeriod);
            #endif /* (!WristTiltRotate_PWM_PWMModeIsCenterAligned) */

            WristTiltRotate_PWM_WriteCounter(WristTiltRotate_PWM_backup.PWMUdb);

            #if (WristTiltRotate_PWM_UseStatus)
                WristTiltRotate_PWM_STATUS_MASK = WristTiltRotate_PWM_backup.InterruptMaskValue;
            #endif /* (WristTiltRotate_PWM_UseStatus) */

            #if(WristTiltRotate_PWM_DeadBandMode == WristTiltRotate_PWM__B_PWM__DBM_256_CLOCKS || \
                WristTiltRotate_PWM_DeadBandMode == WristTiltRotate_PWM__B_PWM__DBM_2_4_CLOCKS)
                WristTiltRotate_PWM_WriteDeadTime(WristTiltRotate_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(WristTiltRotate_PWM_KillModeMinTime)
                WristTiltRotate_PWM_WriteKillTime(WristTiltRotate_PWM_backup.PWMKillCounterPeriod);
            #endif /* (WristTiltRotate_PWM_KillModeMinTime) */

            #if(WristTiltRotate_PWM_UseControl)
                WristTiltRotate_PWM_WriteControlRegister(WristTiltRotate_PWM_backup.PWMControlRegister);
            #endif /* (WristTiltRotate_PWM_UseControl) */
        #endif  /* (!WristTiltRotate_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: WristTiltRotate_PWM_Sleep
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
*  WristTiltRotate_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void WristTiltRotate_PWM_Sleep(void) 
{
    #if(WristTiltRotate_PWM_UseControl)
        if(WristTiltRotate_PWM_CTRL_ENABLE == (WristTiltRotate_PWM_CONTROL & WristTiltRotate_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            WristTiltRotate_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            WristTiltRotate_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (WristTiltRotate_PWM_UseControl) */

    /* Stop component */
    WristTiltRotate_PWM_Stop();

    /* Save registers configuration */
    WristTiltRotate_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: WristTiltRotate_PWM_Wakeup
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
*  WristTiltRotate_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void WristTiltRotate_PWM_Wakeup(void) 
{
     /* Restore registers values */
    WristTiltRotate_PWM_RestoreConfig();

    if(WristTiltRotate_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        WristTiltRotate_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
