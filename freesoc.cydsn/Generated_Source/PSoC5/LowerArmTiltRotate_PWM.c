/*******************************************************************************
* File Name: LowerArmTiltRotate_PWM.c
* Version 3.30
*
* Description:
*  The PWM User Module consist of an 8 or 16-bit counter with two 8 or 16-bit
*  comparitors. Each instance of this user module is capable of generating
*  two PWM outputs with the same period. The pulse width is selectable between
*  1 and 255/65535. The period is selectable between 2 and 255/65536 clocks.
*  The compare value output may be configured to be active when the present
*  counter is less than or less than/equal to the compare value.
*  A terminal count output is also provided. It generates a pulse one clock
*  width wide when the counter is equal to zero.
*
* Note:
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "LowerArmTiltRotate_PWM.h"

/* Error message for removed <resource> through optimization */
#ifdef LowerArmTiltRotate_PWM_PWMUDB_genblk1_ctrlreg__REMOVED
    #error PWM_v3_30 detected with a constant 0 for the enable or \
         constant 1 for reset. This will prevent the component from operating.
#endif /* LowerArmTiltRotate_PWM_PWMUDB_genblk1_ctrlreg__REMOVED */

uint8 LowerArmTiltRotate_PWM_initVar = 0u;


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_Start
********************************************************************************
*
* Summary:
*  The start function initializes the pwm with the default values, the
*  enables the counter to begin counting.  It does not enable interrupts,
*  the EnableInt command should be called if interrupt generation is required.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  LowerArmTiltRotate_PWM_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void LowerArmTiltRotate_PWM_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(LowerArmTiltRotate_PWM_initVar == 0u)
    {
        LowerArmTiltRotate_PWM_Init();
        LowerArmTiltRotate_PWM_initVar = 1u;
    }
    LowerArmTiltRotate_PWM_Enable();

}


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  LowerArmTiltRotate_PWM_Start().
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void LowerArmTiltRotate_PWM_Init(void) 
{
    #if (LowerArmTiltRotate_PWM_UsingFixedFunction || LowerArmTiltRotate_PWM_UseControl)
        uint8 ctrl;
    #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction || LowerArmTiltRotate_PWM_UseControl) */

    #if(!LowerArmTiltRotate_PWM_UsingFixedFunction)
        #if(LowerArmTiltRotate_PWM_UseStatus)
            /* Interrupt State Backup for Critical Region*/
            uint8 LowerArmTiltRotate_PWM_interruptState;
        #endif /* (LowerArmTiltRotate_PWM_UseStatus) */
    #endif /* (!LowerArmTiltRotate_PWM_UsingFixedFunction) */

    #if (LowerArmTiltRotate_PWM_UsingFixedFunction)
        /* You are allowed to write the compare value (FF only) */
        LowerArmTiltRotate_PWM_CONTROL |= LowerArmTiltRotate_PWM_CFG0_MODE;
        #if (LowerArmTiltRotate_PWM_DeadBand2_4)
            LowerArmTiltRotate_PWM_CONTROL |= LowerArmTiltRotate_PWM_CFG0_DB;
        #endif /* (LowerArmTiltRotate_PWM_DeadBand2_4) */

        ctrl = LowerArmTiltRotate_PWM_CONTROL3 & ((uint8 )(~LowerArmTiltRotate_PWM_CTRL_CMPMODE1_MASK));
        LowerArmTiltRotate_PWM_CONTROL3 = ctrl | LowerArmTiltRotate_PWM_DEFAULT_COMPARE1_MODE;

         /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
        LowerArmTiltRotate_PWM_RT1 &= ((uint8)(~LowerArmTiltRotate_PWM_RT1_MASK));
        LowerArmTiltRotate_PWM_RT1 |= LowerArmTiltRotate_PWM_SYNC;

        /*Enable DSI Sync all all inputs of the PWM*/
        LowerArmTiltRotate_PWM_RT1 &= ((uint8)(~LowerArmTiltRotate_PWM_SYNCDSI_MASK));
        LowerArmTiltRotate_PWM_RT1 |= LowerArmTiltRotate_PWM_SYNCDSI_EN;

    #elif (LowerArmTiltRotate_PWM_UseControl)
        /* Set the default compare mode defined in the parameter */
        ctrl = LowerArmTiltRotate_PWM_CONTROL & ((uint8)(~LowerArmTiltRotate_PWM_CTRL_CMPMODE2_MASK)) &
                ((uint8)(~LowerArmTiltRotate_PWM_CTRL_CMPMODE1_MASK));
        LowerArmTiltRotate_PWM_CONTROL = ctrl | LowerArmTiltRotate_PWM_DEFAULT_COMPARE2_MODE |
                                   LowerArmTiltRotate_PWM_DEFAULT_COMPARE1_MODE;
    #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */

    #if (!LowerArmTiltRotate_PWM_UsingFixedFunction)
        #if (LowerArmTiltRotate_PWM_Resolution == 8)
            /* Set FIFO 0 to 1 byte register for period*/
            LowerArmTiltRotate_PWM_AUX_CONTROLDP0 |= (LowerArmTiltRotate_PWM_AUX_CTRL_FIFO0_CLR);
        #else /* (LowerArmTiltRotate_PWM_Resolution == 16)*/
            /* Set FIFO 0 to 1 byte register for period */
            LowerArmTiltRotate_PWM_AUX_CONTROLDP0 |= (LowerArmTiltRotate_PWM_AUX_CTRL_FIFO0_CLR);
            LowerArmTiltRotate_PWM_AUX_CONTROLDP1 |= (LowerArmTiltRotate_PWM_AUX_CTRL_FIFO0_CLR);
        #endif /* (LowerArmTiltRotate_PWM_Resolution == 8) */

        LowerArmTiltRotate_PWM_WriteCounter(LowerArmTiltRotate_PWM_INIT_PERIOD_VALUE);
    #endif /* (!LowerArmTiltRotate_PWM_UsingFixedFunction) */

    LowerArmTiltRotate_PWM_WritePeriod(LowerArmTiltRotate_PWM_INIT_PERIOD_VALUE);

        #if (LowerArmTiltRotate_PWM_UseOneCompareMode)
            LowerArmTiltRotate_PWM_WriteCompare(LowerArmTiltRotate_PWM_INIT_COMPARE_VALUE1);
        #else
            LowerArmTiltRotate_PWM_WriteCompare1(LowerArmTiltRotate_PWM_INIT_COMPARE_VALUE1);
            LowerArmTiltRotate_PWM_WriteCompare2(LowerArmTiltRotate_PWM_INIT_COMPARE_VALUE2);
        #endif /* (LowerArmTiltRotate_PWM_UseOneCompareMode) */

        #if (LowerArmTiltRotate_PWM_KillModeMinTime)
            LowerArmTiltRotate_PWM_WriteKillTime(LowerArmTiltRotate_PWM_MinimumKillTime);
        #endif /* (LowerArmTiltRotate_PWM_KillModeMinTime) */

        #if (LowerArmTiltRotate_PWM_DeadBandUsed)
            LowerArmTiltRotate_PWM_WriteDeadTime(LowerArmTiltRotate_PWM_INIT_DEAD_TIME);
        #endif /* (LowerArmTiltRotate_PWM_DeadBandUsed) */

    #if (LowerArmTiltRotate_PWM_UseStatus || LowerArmTiltRotate_PWM_UsingFixedFunction)
        LowerArmTiltRotate_PWM_SetInterruptMode(LowerArmTiltRotate_PWM_INIT_INTERRUPTS_MODE);
    #endif /* (LowerArmTiltRotate_PWM_UseStatus || LowerArmTiltRotate_PWM_UsingFixedFunction) */

    #if (LowerArmTiltRotate_PWM_UsingFixedFunction)
        /* Globally Enable the Fixed Function Block chosen */
        LowerArmTiltRotate_PWM_GLOBAL_ENABLE |= LowerArmTiltRotate_PWM_BLOCK_EN_MASK;
        /* Set the Interrupt source to come from the status register */
        LowerArmTiltRotate_PWM_CONTROL2 |= LowerArmTiltRotate_PWM_CTRL2_IRQ_SEL;
    #else
        #if(LowerArmTiltRotate_PWM_UseStatus)

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            LowerArmTiltRotate_PWM_interruptState = CyEnterCriticalSection();
            /* Use the interrupt output of the status register for IRQ output */
            LowerArmTiltRotate_PWM_STATUS_AUX_CTRL |= LowerArmTiltRotate_PWM_STATUS_ACTL_INT_EN_MASK;

             /* Exit Critical Region*/
            CyExitCriticalSection(LowerArmTiltRotate_PWM_interruptState);

            /* Clear the FIFO to enable the LowerArmTiltRotate_PWM_STATUS_FIFOFULL
                   bit to be set on FIFO full. */
            LowerArmTiltRotate_PWM_ClearFIFO();
        #endif /* (LowerArmTiltRotate_PWM_UseStatus) */
    #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_Enable
********************************************************************************
*
* Summary:
*  Enables the PWM block operation
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This works only if software enable mode is chosen
*
*******************************************************************************/
void LowerArmTiltRotate_PWM_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (LowerArmTiltRotate_PWM_UsingFixedFunction)
        LowerArmTiltRotate_PWM_GLOBAL_ENABLE |= LowerArmTiltRotate_PWM_BLOCK_EN_MASK;
        LowerArmTiltRotate_PWM_GLOBAL_STBY_ENABLE |= LowerArmTiltRotate_PWM_BLOCK_STBY_EN_MASK;
    #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */

    /* Enable the PWM from the control register  */
    #if (LowerArmTiltRotate_PWM_UseControl || LowerArmTiltRotate_PWM_UsingFixedFunction)
        LowerArmTiltRotate_PWM_CONTROL |= LowerArmTiltRotate_PWM_CTRL_ENABLE;
    #endif /* (LowerArmTiltRotate_PWM_UseControl || LowerArmTiltRotate_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_Stop
********************************************************************************
*
* Summary:
*  The stop function halts the PWM, but does not change any modes or disable
*  interrupts.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  If the Enable mode is set to Hardware only then this function
*  has no effect on the operation of the PWM
*
*******************************************************************************/
void LowerArmTiltRotate_PWM_Stop(void) 
{
    #if (LowerArmTiltRotate_PWM_UseControl || LowerArmTiltRotate_PWM_UsingFixedFunction)
        LowerArmTiltRotate_PWM_CONTROL &= ((uint8)(~LowerArmTiltRotate_PWM_CTRL_ENABLE));
    #endif /* (LowerArmTiltRotate_PWM_UseControl || LowerArmTiltRotate_PWM_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (LowerArmTiltRotate_PWM_UsingFixedFunction)
        LowerArmTiltRotate_PWM_GLOBAL_ENABLE &= ((uint8)(~LowerArmTiltRotate_PWM_BLOCK_EN_MASK));
        LowerArmTiltRotate_PWM_GLOBAL_STBY_ENABLE &= ((uint8)(~LowerArmTiltRotate_PWM_BLOCK_STBY_EN_MASK));
    #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */
}

#if (LowerArmTiltRotate_PWM_UseOneCompareMode)
    #if (LowerArmTiltRotate_PWM_CompareMode1SW)


        /*******************************************************************************
        * Function Name: LowerArmTiltRotate_PWM_SetCompareMode
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm output when in Dither mode,
        *  Center Align Mode or One Output Mode.
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void LowerArmTiltRotate_PWM_SetCompareMode(uint8 comparemode) 
        {
            #if(LowerArmTiltRotate_PWM_UsingFixedFunction)

                #if(0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemodemasked = ((uint8)((uint8)comparemode << LowerArmTiltRotate_PWM_CTRL_CMPMODE1_SHIFT));
                #else
                    uint8 comparemodemasked = comparemode;
                #endif /* (0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE1_SHIFT) */

                LowerArmTiltRotate_PWM_CONTROL3 &= ((uint8)(~LowerArmTiltRotate_PWM_CTRL_CMPMODE1_MASK)); /*Clear Existing Data */
                LowerArmTiltRotate_PWM_CONTROL3 |= comparemodemasked;

            #elif (LowerArmTiltRotate_PWM_UseControl)

                #if(0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemode1masked = ((uint8)((uint8)comparemode << LowerArmTiltRotate_PWM_CTRL_CMPMODE1_SHIFT)) &
                                                LowerArmTiltRotate_PWM_CTRL_CMPMODE1_MASK;
                #else
                    uint8 comparemode1masked = comparemode & LowerArmTiltRotate_PWM_CTRL_CMPMODE1_MASK;
                #endif /* (0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE1_SHIFT) */

                #if(0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE2_SHIFT)
                    uint8 comparemode2masked = ((uint8)((uint8)comparemode << LowerArmTiltRotate_PWM_CTRL_CMPMODE2_SHIFT)) &
                                               LowerArmTiltRotate_PWM_CTRL_CMPMODE2_MASK;
                #else
                    uint8 comparemode2masked = comparemode & LowerArmTiltRotate_PWM_CTRL_CMPMODE2_MASK;
                #endif /* (0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE2_SHIFT) */

                /*Clear existing mode */
                LowerArmTiltRotate_PWM_CONTROL &= ((uint8)(~(LowerArmTiltRotate_PWM_CTRL_CMPMODE1_MASK |
                                            LowerArmTiltRotate_PWM_CTRL_CMPMODE2_MASK)));
                LowerArmTiltRotate_PWM_CONTROL |= (comparemode1masked | comparemode2masked);

            #else
                uint8 temp = comparemode;
            #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */
        }
    #endif /* LowerArmTiltRotate_PWM_CompareMode1SW */

#else /* UseOneCompareMode */

    #if (LowerArmTiltRotate_PWM_CompareMode1SW)


        /*******************************************************************************
        * Function Name: LowerArmTiltRotate_PWM_SetCompareMode1
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm or pwm1 output
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void LowerArmTiltRotate_PWM_SetCompareMode1(uint8 comparemode) 
        {
            #if(0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE1_SHIFT)
                uint8 comparemodemasked = ((uint8)((uint8)comparemode << LowerArmTiltRotate_PWM_CTRL_CMPMODE1_SHIFT)) &
                                           LowerArmTiltRotate_PWM_CTRL_CMPMODE1_MASK;
            #else
                uint8 comparemodemasked = comparemode & LowerArmTiltRotate_PWM_CTRL_CMPMODE1_MASK;
            #endif /* (0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE1_SHIFT) */

            #if (LowerArmTiltRotate_PWM_UseControl)
                LowerArmTiltRotate_PWM_CONTROL &= ((uint8)(~LowerArmTiltRotate_PWM_CTRL_CMPMODE1_MASK)); /*Clear existing mode */
                LowerArmTiltRotate_PWM_CONTROL |= comparemodemasked;
            #endif /* (LowerArmTiltRotate_PWM_UseControl) */
        }
    #endif /* LowerArmTiltRotate_PWM_CompareMode1SW */

#if (LowerArmTiltRotate_PWM_CompareMode2SW)


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_SetCompareMode2
    ********************************************************************************
    *
    * Summary:
    *  This function writes the Compare Mode for the pwm or pwm2 output
    *
    * Parameters:
    *  comparemode:  The new compare mode for the PWM output. Use the compare types
    *                defined in the H file as input arguments.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void LowerArmTiltRotate_PWM_SetCompareMode2(uint8 comparemode) 
    {

        #if(0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE2_SHIFT)
            uint8 comparemodemasked = ((uint8)((uint8)comparemode << LowerArmTiltRotate_PWM_CTRL_CMPMODE2_SHIFT)) &
                                                 LowerArmTiltRotate_PWM_CTRL_CMPMODE2_MASK;
        #else
            uint8 comparemodemasked = comparemode & LowerArmTiltRotate_PWM_CTRL_CMPMODE2_MASK;
        #endif /* (0 != LowerArmTiltRotate_PWM_CTRL_CMPMODE2_SHIFT) */

        #if (LowerArmTiltRotate_PWM_UseControl)
            LowerArmTiltRotate_PWM_CONTROL &= ((uint8)(~LowerArmTiltRotate_PWM_CTRL_CMPMODE2_MASK)); /*Clear existing mode */
            LowerArmTiltRotate_PWM_CONTROL |= comparemodemasked;
        #endif /* (LowerArmTiltRotate_PWM_UseControl) */
    }
    #endif /*LowerArmTiltRotate_PWM_CompareMode2SW */

#endif /* UseOneCompareMode */


#if (!LowerArmTiltRotate_PWM_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_WriteCounter
    ********************************************************************************
    *
    * Summary:
    *  Writes a new counter value directly to the counter register. This will be
    *  implemented for that currently running period and only that period. This API
    *  is valid only for UDB implementation and not available for fixed function
    *  PWM implementation.
    *
    * Parameters:
    *  counter:  The period new period counter value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  The PWM Period will be reloaded when a counter value will be a zero
    *
    *******************************************************************************/
    void LowerArmTiltRotate_PWM_WriteCounter(uint16 counter) \
                                       
    {
        CY_SET_REG16(LowerArmTiltRotate_PWM_COUNTER_LSB_PTR, counter);
    }


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_ReadCounter
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current value of the counter.  It doesn't matter
    *  if the counter is enabled or running.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  The current value of the counter.
    *
    *******************************************************************************/
    uint16 LowerArmTiltRotate_PWM_ReadCounter(void) 
    {
        /* Force capture by reading Accumulator */
        /* Must first do a software capture to be able to read the counter */
        /* It is up to the user code to make sure there isn't already captured data in the FIFO */
          (void)CY_GET_REG8(LowerArmTiltRotate_PWM_COUNTERCAP_LSB_PTR_8BIT);

        /* Read the data from the FIFO */
        return (CY_GET_REG16(LowerArmTiltRotate_PWM_CAPTURE_LSB_PTR));
    }

    #if (LowerArmTiltRotate_PWM_UseStatus)


        /*******************************************************************************
        * Function Name: LowerArmTiltRotate_PWM_ClearFIFO
        ********************************************************************************
        *
        * Summary:
        *  This function clears all capture data from the capture FIFO
        *
        * Parameters:
        *  None
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void LowerArmTiltRotate_PWM_ClearFIFO(void) 
        {
            while(0u != (LowerArmTiltRotate_PWM_ReadStatusRegister() & LowerArmTiltRotate_PWM_STATUS_FIFONEMPTY))
            {
                (void)LowerArmTiltRotate_PWM_ReadCapture();
            }
        }

    #endif /* LowerArmTiltRotate_PWM_UseStatus */

#endif /* !LowerArmTiltRotate_PWM_UsingFixedFunction */


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_WritePeriod
********************************************************************************
*
* Summary:
*  This function is used to change the period of the counter.  The new period
*  will be loaded the next time terminal count is detected.
*
* Parameters:
*  period:  Period value. May be between 1 and (2^Resolution)-1.  A value of 0
*           will result in the counter remaining at zero.
*
* Return:
*  None
*
*******************************************************************************/
void LowerArmTiltRotate_PWM_WritePeriod(uint16 period) 
{
    #if(LowerArmTiltRotate_PWM_UsingFixedFunction)
        CY_SET_REG16(LowerArmTiltRotate_PWM_PERIOD_LSB_PTR, (uint16)period);
    #else
        CY_SET_REG16(LowerArmTiltRotate_PWM_PERIOD_LSB_PTR, period);
    #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */
}

#if (LowerArmTiltRotate_PWM_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_WriteCompare
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value when the PWM is in Dither
    *  mode. The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  compared to the compare value based on the compare mode defined in
    *  Dither Mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  This function is only available if the PWM mode parameter is set to
    *  Dither Mode, Center Aligned Mode or One Output Mode
    *
    *******************************************************************************/
    void LowerArmTiltRotate_PWM_WriteCompare(uint16 compare) \
                                       
    {
        #if(LowerArmTiltRotate_PWM_UsingFixedFunction)
            CY_SET_REG16(LowerArmTiltRotate_PWM_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG16(LowerArmTiltRotate_PWM_COMPARE1_LSB_PTR, compare);
        #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */

        #if (LowerArmTiltRotate_PWM_PWMMode == LowerArmTiltRotate_PWM__B_PWM__DITHER)
            #if(LowerArmTiltRotate_PWM_UsingFixedFunction)
                CY_SET_REG16(LowerArmTiltRotate_PWM_COMPARE2_LSB_PTR, (uint16)(compare + 1u));
            #else
                CY_SET_REG16(LowerArmTiltRotate_PWM_COMPARE2_LSB_PTR, (compare + 1u));
            #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */
        #endif /* (LowerArmTiltRotate_PWM_PWMMode == LowerArmTiltRotate_PWM__B_PWM__DITHER) */
    }


#else


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_WriteCompare1
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value.  The compare output will
    *  reflect the new value on the next UDB clock.  The compare output will be
    *  driven high when the present counter value is less than or less than or
    *  equal to the compare register, depending on the mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void LowerArmTiltRotate_PWM_WriteCompare1(uint16 compare) \
                                        
    {
        #if(LowerArmTiltRotate_PWM_UsingFixedFunction)
            CY_SET_REG16(LowerArmTiltRotate_PWM_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG16(LowerArmTiltRotate_PWM_COMPARE1_LSB_PTR, compare);
        #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */
    }


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_WriteCompare2
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare value, for compare1 output.
    *  The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  less than or less than or equal to the compare register, depending on the
    *  mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void LowerArmTiltRotate_PWM_WriteCompare2(uint16 compare) \
                                        
    {
        #if(LowerArmTiltRotate_PWM_UsingFixedFunction)
            CY_SET_REG16(LowerArmTiltRotate_PWM_COMPARE2_LSB_PTR, compare);
        #else
            CY_SET_REG16(LowerArmTiltRotate_PWM_COMPARE2_LSB_PTR, compare);
        #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */
    }
#endif /* UseOneCompareMode */

#if (LowerArmTiltRotate_PWM_DeadBandUsed)


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_WriteDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function writes the dead-band counts to the corresponding register
    *
    * Parameters:
    *  deadtime:  Number of counts for dead time
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void LowerArmTiltRotate_PWM_WriteDeadTime(uint8 deadtime) 
    {
        /* If using the Dead Band 1-255 mode then just write the register */
        #if(!LowerArmTiltRotate_PWM_DeadBand2_4)
            CY_SET_REG8(LowerArmTiltRotate_PWM_DEADBAND_COUNT_PTR, deadtime);
        #else
            /* Otherwise the data has to be masked and offset */
            /* Clear existing data */
            LowerArmTiltRotate_PWM_DEADBAND_COUNT &= ((uint8)(~LowerArmTiltRotate_PWM_DEADBAND_COUNT_MASK));

            /* Set new dead time */
            #if(LowerArmTiltRotate_PWM_DEADBAND_COUNT_SHIFT)
                LowerArmTiltRotate_PWM_DEADBAND_COUNT |= ((uint8)((uint8)deadtime << LowerArmTiltRotate_PWM_DEADBAND_COUNT_SHIFT)) &
                                                    LowerArmTiltRotate_PWM_DEADBAND_COUNT_MASK;
            #else
                LowerArmTiltRotate_PWM_DEADBAND_COUNT |= deadtime & LowerArmTiltRotate_PWM_DEADBAND_COUNT_MASK;
            #endif /* (LowerArmTiltRotate_PWM_DEADBAND_COUNT_SHIFT) */

        #endif /* (!LowerArmTiltRotate_PWM_DeadBand2_4) */
    }


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_ReadDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function reads the dead-band counts from the corresponding register
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Dead Band Counts
    *
    *******************************************************************************/
    uint8 LowerArmTiltRotate_PWM_ReadDeadTime(void) 
    {
        /* If using the Dead Band 1-255 mode then just read the register */
        #if(!LowerArmTiltRotate_PWM_DeadBand2_4)
            return (CY_GET_REG8(LowerArmTiltRotate_PWM_DEADBAND_COUNT_PTR));
        #else

            /* Otherwise the data has to be masked and offset */
            #if(LowerArmTiltRotate_PWM_DEADBAND_COUNT_SHIFT)
                return ((uint8)(((uint8)(LowerArmTiltRotate_PWM_DEADBAND_COUNT & LowerArmTiltRotate_PWM_DEADBAND_COUNT_MASK)) >>
                                                                           LowerArmTiltRotate_PWM_DEADBAND_COUNT_SHIFT));
            #else
                return (LowerArmTiltRotate_PWM_DEADBAND_COUNT & LowerArmTiltRotate_PWM_DEADBAND_COUNT_MASK);
            #endif /* (LowerArmTiltRotate_PWM_DEADBAND_COUNT_SHIFT) */
        #endif /* (!LowerArmTiltRotate_PWM_DeadBand2_4) */
    }
#endif /* DeadBandUsed */

#if (LowerArmTiltRotate_PWM_UseStatus || LowerArmTiltRotate_PWM_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_SetInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  This function configures the interrupts mask control of theinterrupt
    *  source status register.
    *
    * Parameters:
    *  uint8 interruptMode: Bit field containing the interrupt sources enabled
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void LowerArmTiltRotate_PWM_SetInterruptMode(uint8 interruptMode) 
    {
        CY_SET_REG8(LowerArmTiltRotate_PWM_STATUS_MASK_PTR, interruptMode);
    }


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_ReadStatusRegister
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current state of the status register.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current status register value. The status register bits are:
    *  [7:6] : Unused(0)
    *  [5]   : Kill event output
    *  [4]   : FIFO not empty
    *  [3]   : FIFO full
    *  [2]   : Terminal count
    *  [1]   : Compare output 2
    *  [0]   : Compare output 1
    *
    *******************************************************************************/
    uint8 LowerArmTiltRotate_PWM_ReadStatusRegister(void) 
    {
        return (CY_GET_REG8(LowerArmTiltRotate_PWM_STATUS_PTR));
    }

#endif /* (LowerArmTiltRotate_PWM_UseStatus || LowerArmTiltRotate_PWM_UsingFixedFunction) */


#if (LowerArmTiltRotate_PWM_UseControl)


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_ReadControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the control register. This API is available
    *  only if the control register is not removed.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current control register value
    *
    *******************************************************************************/
    uint8 LowerArmTiltRotate_PWM_ReadControlRegister(void) 
    {
        uint8 result;

        result = CY_GET_REG8(LowerArmTiltRotate_PWM_CONTROL_PTR);
        return (result);
    }


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_WriteControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Sets the bit field of the control register. This API is available only if
    *  the control register is not removed.
    *
    * Parameters:
    *  uint8 control: Control register bit field, The status register bits are:
    *  [7]   : PWM Enable
    *  [6]   : Reset
    *  [5:3] : Compare Mode2
    *  [2:0] : Compare Mode2
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void LowerArmTiltRotate_PWM_WriteControlRegister(uint8 control) 
    {
        CY_SET_REG8(LowerArmTiltRotate_PWM_CONTROL_PTR, control);
    }

#endif /* (LowerArmTiltRotate_PWM_UseControl) */


#if (!LowerArmTiltRotate_PWM_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_ReadCapture
    ********************************************************************************
    *
    * Summary:
    *  Reads the capture value from the capture FIFO.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: The current capture value
    *
    *******************************************************************************/
    uint16 LowerArmTiltRotate_PWM_ReadCapture(void) 
    {
        return (CY_GET_REG16(LowerArmTiltRotate_PWM_CAPTURE_LSB_PTR));
    }

#endif /* (!LowerArmTiltRotate_PWM_UsingFixedFunction) */


#if (LowerArmTiltRotate_PWM_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_ReadCompare
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare output when the PWM Mode parameter is
    *  set to Dither mode, Center Aligned mode, or One Output mode.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value
    *
    *******************************************************************************/
    uint16 LowerArmTiltRotate_PWM_ReadCompare(void) 
    {
        #if(LowerArmTiltRotate_PWM_UsingFixedFunction)
            return ((uint16)CY_GET_REG16(LowerArmTiltRotate_PWM_COMPARE1_LSB_PTR));
        #else
            return (CY_GET_REG16(LowerArmTiltRotate_PWM_COMPARE1_LSB_PTR));
        #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */
    }

#else


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_ReadCompare1
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare1 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint16 LowerArmTiltRotate_PWM_ReadCompare1(void) 
    {
        return (CY_GET_REG16(LowerArmTiltRotate_PWM_COMPARE1_LSB_PTR));
    }


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_ReadCompare2
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare2 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint16 LowerArmTiltRotate_PWM_ReadCompare2(void) 
    {
        return (CY_GET_REG16(LowerArmTiltRotate_PWM_COMPARE2_LSB_PTR));
    }

#endif /* (LowerArmTiltRotate_PWM_UseOneCompareMode) */


/*******************************************************************************
* Function Name: LowerArmTiltRotate_PWM_ReadPeriod
********************************************************************************
*
* Summary:
*  Reads the period value used by the PWM hardware.
*
* Parameters:
*  None
*
* Return:
*  uint8/16: Period value
*
*******************************************************************************/
uint16 LowerArmTiltRotate_PWM_ReadPeriod(void) 
{
    #if(LowerArmTiltRotate_PWM_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(LowerArmTiltRotate_PWM_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(LowerArmTiltRotate_PWM_PERIOD_LSB_PTR));
    #endif /* (LowerArmTiltRotate_PWM_UsingFixedFunction) */
}

#if ( LowerArmTiltRotate_PWM_KillModeMinTime)


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_WriteKillTime
    ********************************************************************************
    *
    * Summary:
    *  Writes the kill time value used by the hardware when the Kill Mode
    *  is set to Minimum Time.
    *
    * Parameters:
    *  uint8: Minimum Time kill counts
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void LowerArmTiltRotate_PWM_WriteKillTime(uint8 killtime) 
    {
        CY_SET_REG8(LowerArmTiltRotate_PWM_KILLMODEMINTIME_PTR, killtime);
    }


    /*******************************************************************************
    * Function Name: LowerArmTiltRotate_PWM_ReadKillTime
    ********************************************************************************
    *
    * Summary:
    *  Reads the kill time value used by the hardware when the Kill Mode is set
    *  to Minimum Time.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8: The current Minimum Time kill counts
    *
    *******************************************************************************/
    uint8 LowerArmTiltRotate_PWM_ReadKillTime(void) 
    {
        return (CY_GET_REG8(LowerArmTiltRotate_PWM_KILLMODEMINTIME_PTR));
    }

#endif /* ( LowerArmTiltRotate_PWM_KillModeMinTime) */

/* [] END OF FILE */
