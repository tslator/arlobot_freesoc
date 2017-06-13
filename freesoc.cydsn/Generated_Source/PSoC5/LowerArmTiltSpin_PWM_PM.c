/*******************************************************************************
* File Name: LowerArmTiltSpin_PWM_PM.c
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

#include "LowerArmTiltSpin_PWM.h"

static LowerArmTiltSpin_PWM_backupStruct LowerArmTiltSpin_PWM_backup;


/*******************************************************************************
* Function Name: LowerArmTiltSpin_PWM_SaveConfig
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
*  LowerArmTiltSpin_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void LowerArmTiltSpin_PWM_SaveConfig(void) 
{

    #if(!LowerArmTiltSpin_PWM_UsingFixedFunction)
        #if(!LowerArmTiltSpin_PWM_PWMModeIsCenterAligned)
            LowerArmTiltSpin_PWM_backup.PWMPeriod = LowerArmTiltSpin_PWM_ReadPeriod();
        #endif /* (!LowerArmTiltSpin_PWM_PWMModeIsCenterAligned) */
        LowerArmTiltSpin_PWM_backup.PWMUdb = LowerArmTiltSpin_PWM_ReadCounter();
        #if (LowerArmTiltSpin_PWM_UseStatus)
            LowerArmTiltSpin_PWM_backup.InterruptMaskValue = LowerArmTiltSpin_PWM_STATUS_MASK;
        #endif /* (LowerArmTiltSpin_PWM_UseStatus) */

        #if(LowerArmTiltSpin_PWM_DeadBandMode == LowerArmTiltSpin_PWM__B_PWM__DBM_256_CLOCKS || \
            LowerArmTiltSpin_PWM_DeadBandMode == LowerArmTiltSpin_PWM__B_PWM__DBM_2_4_CLOCKS)
            LowerArmTiltSpin_PWM_backup.PWMdeadBandValue = LowerArmTiltSpin_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(LowerArmTiltSpin_PWM_KillModeMinTime)
             LowerArmTiltSpin_PWM_backup.PWMKillCounterPeriod = LowerArmTiltSpin_PWM_ReadKillTime();
        #endif /* (LowerArmTiltSpin_PWM_KillModeMinTime) */

        #if(LowerArmTiltSpin_PWM_UseControl)
            LowerArmTiltSpin_PWM_backup.PWMControlRegister = LowerArmTiltSpin_PWM_ReadControlRegister();
        #endif /* (LowerArmTiltSpin_PWM_UseControl) */
    #endif  /* (!LowerArmTiltSpin_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: LowerArmTiltSpin_PWM_RestoreConfig
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
*  LowerArmTiltSpin_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void LowerArmTiltSpin_PWM_RestoreConfig(void) 
{
        #if(!LowerArmTiltSpin_PWM_UsingFixedFunction)
            #if(!LowerArmTiltSpin_PWM_PWMModeIsCenterAligned)
                LowerArmTiltSpin_PWM_WritePeriod(LowerArmTiltSpin_PWM_backup.PWMPeriod);
            #endif /* (!LowerArmTiltSpin_PWM_PWMModeIsCenterAligned) */

            LowerArmTiltSpin_PWM_WriteCounter(LowerArmTiltSpin_PWM_backup.PWMUdb);

            #if (LowerArmTiltSpin_PWM_UseStatus)
                LowerArmTiltSpin_PWM_STATUS_MASK = LowerArmTiltSpin_PWM_backup.InterruptMaskValue;
            #endif /* (LowerArmTiltSpin_PWM_UseStatus) */

            #if(LowerArmTiltSpin_PWM_DeadBandMode == LowerArmTiltSpin_PWM__B_PWM__DBM_256_CLOCKS || \
                LowerArmTiltSpin_PWM_DeadBandMode == LowerArmTiltSpin_PWM__B_PWM__DBM_2_4_CLOCKS)
                LowerArmTiltSpin_PWM_WriteDeadTime(LowerArmTiltSpin_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(LowerArmTiltSpin_PWM_KillModeMinTime)
                LowerArmTiltSpin_PWM_WriteKillTime(LowerArmTiltSpin_PWM_backup.PWMKillCounterPeriod);
            #endif /* (LowerArmTiltSpin_PWM_KillModeMinTime) */

            #if(LowerArmTiltSpin_PWM_UseControl)
                LowerArmTiltSpin_PWM_WriteControlRegister(LowerArmTiltSpin_PWM_backup.PWMControlRegister);
            #endif /* (LowerArmTiltSpin_PWM_UseControl) */
        #endif  /* (!LowerArmTiltSpin_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: LowerArmTiltSpin_PWM_Sleep
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
*  LowerArmTiltSpin_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void LowerArmTiltSpin_PWM_Sleep(void) 
{
    #if(LowerArmTiltSpin_PWM_UseControl)
        if(LowerArmTiltSpin_PWM_CTRL_ENABLE == (LowerArmTiltSpin_PWM_CONTROL & LowerArmTiltSpin_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            LowerArmTiltSpin_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            LowerArmTiltSpin_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (LowerArmTiltSpin_PWM_UseControl) */

    /* Stop component */
    LowerArmTiltSpin_PWM_Stop();

    /* Save registers configuration */
    LowerArmTiltSpin_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: LowerArmTiltSpin_PWM_Wakeup
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
*  LowerArmTiltSpin_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void LowerArmTiltSpin_PWM_Wakeup(void) 
{
     /* Restore registers values */
    LowerArmTiltSpin_PWM_RestoreConfig();

    if(LowerArmTiltSpin_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        LowerArmTiltSpin_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
