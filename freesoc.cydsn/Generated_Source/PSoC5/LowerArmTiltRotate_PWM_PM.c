/*******************************************************************************
* File Name: LowerArmTiltRotate_PWM_PM.c
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

#include "LowerArmTiltRotate_PWM.h"

static LowerArmTiltRotate_PWM_backupStruct LowerArmTiltRotate_PWM_backup;


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_SaveConfig
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
*  LowerArmTiltRotate_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void LowerArmTiltRotate_PWM_SaveConfig(void) 
{

    #if(!LowerArmTiltRotate_PWM_UsingFixedFunction)
        #if(!LowerArmTiltRotate_PWM_PWMModeIsCenterAligned)
            LowerArmTiltRotate_PWM_backup.PWMPeriod = LowerArmTiltRotate_PWM_ReadPeriod();
        #endif /* (!LowerArmTiltRotate_PWM_PWMModeIsCenterAligned) */
        LowerArmTiltRotate_PWM_backup.PWMUdb = LowerArmTiltRotate_PWM_ReadCounter();
        #if (LowerArmTiltRotate_PWM_UseStatus)
            LowerArmTiltRotate_PWM_backup.InterruptMaskValue = LowerArmTiltRotate_PWM_STATUS_MASK;
        #endif /* (LowerArmTiltRotate_PWM_UseStatus) */

        #if(LowerArmTiltRotate_PWM_DeadBandMode == LowerArmTiltRotate_PWM__B_PWM__DBM_256_CLOCKS || \
            LowerArmTiltRotate_PWM_DeadBandMode == LowerArmTiltRotate_PWM__B_PWM__DBM_2_4_CLOCKS)
            LowerArmTiltRotate_PWM_backup.PWMdeadBandValue = LowerArmTiltRotate_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(LowerArmTiltRotate_PWM_KillModeMinTime)
             LowerArmTiltRotate_PWM_backup.PWMKillCounterPeriod = LowerArmTiltRotate_PWM_ReadKillTime();
        #endif /* (LowerArmTiltRotate_PWM_KillModeMinTime) */

        #if(LowerArmTiltRotate_PWM_UseControl)
            LowerArmTiltRotate_PWM_backup.PWMControlRegister = LowerArmTiltRotate_PWM_ReadControlRegister();
        #endif /* (LowerArmTiltRotate_PWM_UseControl) */
    #endif  /* (!LowerArmTiltRotate_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_RestoreConfig
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
*  LowerArmTiltRotate_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void LowerArmTiltRotate_PWM_RestoreConfig(void) 
{
        #if(!LowerArmTiltRotate_PWM_UsingFixedFunction)
            #if(!LowerArmTiltRotate_PWM_PWMModeIsCenterAligned)
                LowerArmTiltRotate_PWM_WritePeriod(LowerArmTiltRotate_PWM_backup.PWMPeriod);
            #endif /* (!LowerArmTiltRotate_PWM_PWMModeIsCenterAligned) */

            LowerArmTiltRotate_PWM_WriteCounter(LowerArmTiltRotate_PWM_backup.PWMUdb);

            #if (LowerArmTiltRotate_PWM_UseStatus)
                LowerArmTiltRotate_PWM_STATUS_MASK = LowerArmTiltRotate_PWM_backup.InterruptMaskValue;
            #endif /* (LowerArmTiltRotate_PWM_UseStatus) */

            #if(LowerArmTiltRotate_PWM_DeadBandMode == LowerArmTiltRotate_PWM__B_PWM__DBM_256_CLOCKS || \
                LowerArmTiltRotate_PWM_DeadBandMode == LowerArmTiltRotate_PWM__B_PWM__DBM_2_4_CLOCKS)
                LowerArmTiltRotate_PWM_WriteDeadTime(LowerArmTiltRotate_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(LowerArmTiltRotate_PWM_KillModeMinTime)
                LowerArmTiltRotate_PWM_WriteKillTime(LowerArmTiltRotate_PWM_backup.PWMKillCounterPeriod);
            #endif /* (LowerArmTiltRotate_PWM_KillModeMinTime) */

            #if(LowerArmTiltRotate_PWM_UseControl)
                LowerArmTiltRotate_PWM_WriteControlRegister(LowerArmTiltRotate_PWM_backup.PWMControlRegister);
            #endif /* (LowerArmTiltRotate_PWM_UseControl) */
        #endif  /* (!LowerArmTiltRotate_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_Sleep
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
*  LowerArmTiltRotate_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void LowerArmTiltRotate_PWM_Sleep(void) 
{
    #if(LowerArmTiltRotate_PWM_UseControl)
        if(LowerArmTiltRotate_PWM_CTRL_ENABLE == (LowerArmTiltRotate_PWM_CONTROL & LowerArmTiltRotate_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            LowerArmTiltRotate_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            LowerArmTiltRotate_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (LowerArmTiltRotate_PWM_UseControl) */

    /* Stop component */
    LowerArmTiltRotate_PWM_Stop();

    /* Save registers configuration */
    LowerArmTiltRotate_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_Wakeup
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
*  LowerArmTiltRotate_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void LowerArmTiltRotate_PWM_Wakeup(void) 
{
     /* Restore registers values */
    LowerArmTiltRotate_PWM_RestoreConfig();

    if(LowerArmTiltRotate_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        LowerArmTiltRotate_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
