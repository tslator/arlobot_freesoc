/*******************************************************************************
* File Name: UpperArmTiltRotatePWM.h
* Version 3.30
*
* Description:
*  Contains the prototypes and constants for the functions available to the
*  PWM user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_PWM_UpperArmTiltRotatePWM_H)
#define CY_PWM_UpperArmTiltRotatePWM_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 UpperArmTiltRotatePWM_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define UpperArmTiltRotatePWM_Resolution                     (16u)
#define UpperArmTiltRotatePWM_UsingFixedFunction             (0u)
#define UpperArmTiltRotatePWM_DeadBandMode                   (0u)
#define UpperArmTiltRotatePWM_KillModeMinTime                (0u)
#define UpperArmTiltRotatePWM_KillMode                       (0u)
#define UpperArmTiltRotatePWM_PWMMode                        (1u)
#define UpperArmTiltRotatePWM_PWMModeIsCenterAligned         (0u)
#define UpperArmTiltRotatePWM_DeadBandUsed                   (0u)
#define UpperArmTiltRotatePWM_DeadBand2_4                    (0u)

#if !defined(UpperArmTiltRotatePWM_PWMUDB_genblk8_stsreg__REMOVED)
    #define UpperArmTiltRotatePWM_UseStatus                  (1u)
#else
    #define UpperArmTiltRotatePWM_UseStatus                  (0u)
#endif /* !defined(UpperArmTiltRotatePWM_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(UpperArmTiltRotatePWM_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define UpperArmTiltRotatePWM_UseControl                 (1u)
#else
    #define UpperArmTiltRotatePWM_UseControl                 (0u)
#endif /* !defined(UpperArmTiltRotatePWM_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define UpperArmTiltRotatePWM_UseOneCompareMode              (0u)
#define UpperArmTiltRotatePWM_MinimumKillTime                (1u)
#define UpperArmTiltRotatePWM_EnableMode                     (0u)

#define UpperArmTiltRotatePWM_CompareMode1SW                 (0u)
#define UpperArmTiltRotatePWM_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define UpperArmTiltRotatePWM__B_PWM__DISABLED 0
#define UpperArmTiltRotatePWM__B_PWM__ASYNCHRONOUS 1
#define UpperArmTiltRotatePWM__B_PWM__SINGLECYCLE 2
#define UpperArmTiltRotatePWM__B_PWM__LATCHED 3
#define UpperArmTiltRotatePWM__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define UpperArmTiltRotatePWM__B_PWM__DBMDISABLED 0
#define UpperArmTiltRotatePWM__B_PWM__DBM_2_4_CLOCKS 1
#define UpperArmTiltRotatePWM__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define UpperArmTiltRotatePWM__B_PWM__ONE_OUTPUT 0
#define UpperArmTiltRotatePWM__B_PWM__TWO_OUTPUTS 1
#define UpperArmTiltRotatePWM__B_PWM__DUAL_EDGE 2
#define UpperArmTiltRotatePWM__B_PWM__CENTER_ALIGN 3
#define UpperArmTiltRotatePWM__B_PWM__DITHER 5
#define UpperArmTiltRotatePWM__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define UpperArmTiltRotatePWM__B_PWM__LESS_THAN 1
#define UpperArmTiltRotatePWM__B_PWM__LESS_THAN_OR_EQUAL 2
#define UpperArmTiltRotatePWM__B_PWM__GREATER_THAN 3
#define UpperArmTiltRotatePWM__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define UpperArmTiltRotatePWM__B_PWM__EQUAL 0
#define UpperArmTiltRotatePWM__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!UpperArmTiltRotatePWM_UsingFixedFunction)
        uint16 PWMUdb;               /* PWM Current Counter value  */
        #if(!UpperArmTiltRotatePWM_PWMModeIsCenterAligned)
            uint16 PWMPeriod;
        #endif /* (!UpperArmTiltRotatePWM_PWMModeIsCenterAligned) */
        #if (UpperArmTiltRotatePWM_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (UpperArmTiltRotatePWM_UseStatus) */

        /* Backup for Deadband parameters */
        #if(UpperArmTiltRotatePWM_DeadBandMode == UpperArmTiltRotatePWM__B_PWM__DBM_256_CLOCKS || \
            UpperArmTiltRotatePWM_DeadBandMode == UpperArmTiltRotatePWM__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(UpperArmTiltRotatePWM_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (UpperArmTiltRotatePWM_KillModeMinTime) */

        /* Backup control register */
        #if(UpperArmTiltRotatePWM_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (UpperArmTiltRotatePWM_UseControl) */

    #endif /* (!UpperArmTiltRotatePWM_UsingFixedFunction) */

}UpperArmTiltRotatePWM_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    UpperArmTiltRotatePWM_Start(void) ;
void    UpperArmTiltRotatePWM_Stop(void) ;

#if (UpperArmTiltRotatePWM_UseStatus || UpperArmTiltRotatePWM_UsingFixedFunction)
    void  UpperArmTiltRotatePWM_SetInterruptMode(uint8 interruptMode) ;
    uint8 UpperArmTiltRotatePWM_ReadStatusRegister(void) ;
#endif /* (UpperArmTiltRotatePWM_UseStatus || UpperArmTiltRotatePWM_UsingFixedFunction) */

#define UpperArmTiltRotatePWM_GetInterruptSource() UpperArmTiltRotatePWM_ReadStatusRegister()

#if (UpperArmTiltRotatePWM_UseControl)
    uint8 UpperArmTiltRotatePWM_ReadControlRegister(void) ;
    void  UpperArmTiltRotatePWM_WriteControlRegister(uint8 control)
          ;
#endif /* (UpperArmTiltRotatePWM_UseControl) */

#if (UpperArmTiltRotatePWM_UseOneCompareMode)
   #if (UpperArmTiltRotatePWM_CompareMode1SW)
       void    UpperArmTiltRotatePWM_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (UpperArmTiltRotatePWM_CompareMode1SW) */
#else
    #if (UpperArmTiltRotatePWM_CompareMode1SW)
        void    UpperArmTiltRotatePWM_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (UpperArmTiltRotatePWM_CompareMode1SW) */
    #if (UpperArmTiltRotatePWM_CompareMode2SW)
        void    UpperArmTiltRotatePWM_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (UpperArmTiltRotatePWM_CompareMode2SW) */
#endif /* (UpperArmTiltRotatePWM_UseOneCompareMode) */

#if (!UpperArmTiltRotatePWM_UsingFixedFunction)
    uint16   UpperArmTiltRotatePWM_ReadCounter(void) ;
    uint16 UpperArmTiltRotatePWM_ReadCapture(void) ;

    #if (UpperArmTiltRotatePWM_UseStatus)
            void UpperArmTiltRotatePWM_ClearFIFO(void) ;
    #endif /* (UpperArmTiltRotatePWM_UseStatus) */

    void    UpperArmTiltRotatePWM_WriteCounter(uint16 counter)
            ;
#endif /* (!UpperArmTiltRotatePWM_UsingFixedFunction) */

void    UpperArmTiltRotatePWM_WritePeriod(uint16 period)
        ;
uint16 UpperArmTiltRotatePWM_ReadPeriod(void) ;

#if (UpperArmTiltRotatePWM_UseOneCompareMode)
    void    UpperArmTiltRotatePWM_WriteCompare(uint16 compare)
            ;
    uint16 UpperArmTiltRotatePWM_ReadCompare(void) ;
#else
    void    UpperArmTiltRotatePWM_WriteCompare1(uint16 compare)
            ;
    uint16 UpperArmTiltRotatePWM_ReadCompare1(void) ;
    void    UpperArmTiltRotatePWM_WriteCompare2(uint16 compare)
            ;
    uint16 UpperArmTiltRotatePWM_ReadCompare2(void) ;
#endif /* (UpperArmTiltRotatePWM_UseOneCompareMode) */


#if (UpperArmTiltRotatePWM_DeadBandUsed)
    void    UpperArmTiltRotatePWM_WriteDeadTime(uint8 deadtime) ;
    uint8   UpperArmTiltRotatePWM_ReadDeadTime(void) ;
#endif /* (UpperArmTiltRotatePWM_DeadBandUsed) */

#if ( UpperArmTiltRotatePWM_KillModeMinTime)
    void UpperArmTiltRotatePWM_WriteKillTime(uint8 killtime) ;
    uint8 UpperArmTiltRotatePWM_ReadKillTime(void) ;
#endif /* ( UpperArmTiltRotatePWM_KillModeMinTime) */

void UpperArmTiltRotatePWM_Init(void) ;
void UpperArmTiltRotatePWM_Enable(void) ;
void UpperArmTiltRotatePWM_Sleep(void) ;
void UpperArmTiltRotatePWM_Wakeup(void) ;
void UpperArmTiltRotatePWM_SaveConfig(void) ;
void UpperArmTiltRotatePWM_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define UpperArmTiltRotatePWM_INIT_PERIOD_VALUE          (20000u)
#define UpperArmTiltRotatePWM_INIT_COMPARE_VALUE1        (1500u)
#define UpperArmTiltRotatePWM_INIT_COMPARE_VALUE2        (1500u)
#define UpperArmTiltRotatePWM_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    UpperArmTiltRotatePWM_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    UpperArmTiltRotatePWM_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    UpperArmTiltRotatePWM_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    UpperArmTiltRotatePWM_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define UpperArmTiltRotatePWM_DEFAULT_COMPARE2_MODE      (uint8)((uint8)1u <<  UpperArmTiltRotatePWM_CTRL_CMPMODE2_SHIFT)
#define UpperArmTiltRotatePWM_DEFAULT_COMPARE1_MODE      (uint8)((uint8)1u <<  UpperArmTiltRotatePWM_CTRL_CMPMODE1_SHIFT)
#define UpperArmTiltRotatePWM_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (UpperArmTiltRotatePWM_UsingFixedFunction)
   #define UpperArmTiltRotatePWM_PERIOD_LSB              (*(reg16 *) UpperArmTiltRotatePWM_PWMHW__PER0)
   #define UpperArmTiltRotatePWM_PERIOD_LSB_PTR          ( (reg16 *) UpperArmTiltRotatePWM_PWMHW__PER0)
   #define UpperArmTiltRotatePWM_COMPARE1_LSB            (*(reg16 *) UpperArmTiltRotatePWM_PWMHW__CNT_CMP0)
   #define UpperArmTiltRotatePWM_COMPARE1_LSB_PTR        ( (reg16 *) UpperArmTiltRotatePWM_PWMHW__CNT_CMP0)
   #define UpperArmTiltRotatePWM_COMPARE2_LSB            (0x00u)
   #define UpperArmTiltRotatePWM_COMPARE2_LSB_PTR        (0x00u)
   #define UpperArmTiltRotatePWM_COUNTER_LSB             (*(reg16 *) UpperArmTiltRotatePWM_PWMHW__CNT_CMP0)
   #define UpperArmTiltRotatePWM_COUNTER_LSB_PTR         ( (reg16 *) UpperArmTiltRotatePWM_PWMHW__CNT_CMP0)
   #define UpperArmTiltRotatePWM_CAPTURE_LSB             (*(reg16 *) UpperArmTiltRotatePWM_PWMHW__CAP0)
   #define UpperArmTiltRotatePWM_CAPTURE_LSB_PTR         ( (reg16 *) UpperArmTiltRotatePWM_PWMHW__CAP0)
   #define UpperArmTiltRotatePWM_RT1                     (*(reg8 *)  UpperArmTiltRotatePWM_PWMHW__RT1)
   #define UpperArmTiltRotatePWM_RT1_PTR                 ( (reg8 *)  UpperArmTiltRotatePWM_PWMHW__RT1)

#else
   #if (UpperArmTiltRotatePWM_Resolution == 8u) /* 8bit - PWM */

       #if(UpperArmTiltRotatePWM_PWMModeIsCenterAligned)
           #define UpperArmTiltRotatePWM_PERIOD_LSB      (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
           #define UpperArmTiltRotatePWM_PERIOD_LSB_PTR  ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
       #else
           #define UpperArmTiltRotatePWM_PERIOD_LSB      (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__F0_REG)
           #define UpperArmTiltRotatePWM_PERIOD_LSB_PTR  ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__F0_REG)
       #endif /* (UpperArmTiltRotatePWM_PWMModeIsCenterAligned) */

       #define UpperArmTiltRotatePWM_COMPARE1_LSB        (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D0_REG)
       #define UpperArmTiltRotatePWM_COMPARE1_LSB_PTR    ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D0_REG)
       #define UpperArmTiltRotatePWM_COMPARE2_LSB        (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
       #define UpperArmTiltRotatePWM_COMPARE2_LSB_PTR    ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
       #define UpperArmTiltRotatePWM_COUNTERCAP_LSB      (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
       #define UpperArmTiltRotatePWM_COUNTERCAP_LSB_PTR  ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
       #define UpperArmTiltRotatePWM_COUNTER_LSB         (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__A0_REG)
       #define UpperArmTiltRotatePWM_COUNTER_LSB_PTR     ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__A0_REG)
       #define UpperArmTiltRotatePWM_CAPTURE_LSB         (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__F1_REG)
       #define UpperArmTiltRotatePWM_CAPTURE_LSB_PTR     ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(UpperArmTiltRotatePWM_PWMModeIsCenterAligned)
               #define UpperArmTiltRotatePWM_PERIOD_LSB      (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
               #define UpperArmTiltRotatePWM_PERIOD_LSB_PTR  ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
            #else
               #define UpperArmTiltRotatePWM_PERIOD_LSB      (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__F0_REG)
               #define UpperArmTiltRotatePWM_PERIOD_LSB_PTR  ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__F0_REG)
            #endif /* (UpperArmTiltRotatePWM_PWMModeIsCenterAligned) */

            #define UpperArmTiltRotatePWM_COMPARE1_LSB       (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D0_REG)
            #define UpperArmTiltRotatePWM_COMPARE1_LSB_PTR   ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D0_REG)
            #define UpperArmTiltRotatePWM_COMPARE2_LSB       (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
            #define UpperArmTiltRotatePWM_COMPARE2_LSB_PTR   ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
            #define UpperArmTiltRotatePWM_COUNTERCAP_LSB     (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
            #define UpperArmTiltRotatePWM_COUNTERCAP_LSB_PTR ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
            #define UpperArmTiltRotatePWM_COUNTER_LSB        (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__A0_REG)
            #define UpperArmTiltRotatePWM_COUNTER_LSB_PTR    ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__A0_REG)
            #define UpperArmTiltRotatePWM_CAPTURE_LSB        (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__F1_REG)
            #define UpperArmTiltRotatePWM_CAPTURE_LSB_PTR    ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__F1_REG)
        #else
            #if(UpperArmTiltRotatePWM_PWMModeIsCenterAligned)
               #define UpperArmTiltRotatePWM_PERIOD_LSB      (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
               #define UpperArmTiltRotatePWM_PERIOD_LSB_PTR  ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
            #else
               #define UpperArmTiltRotatePWM_PERIOD_LSB      (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_F0_REG)
               #define UpperArmTiltRotatePWM_PERIOD_LSB_PTR  ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_F0_REG)
            #endif /* (UpperArmTiltRotatePWM_PWMModeIsCenterAligned) */

            #define UpperArmTiltRotatePWM_COMPARE1_LSB       (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_D0_REG)
            #define UpperArmTiltRotatePWM_COMPARE1_LSB_PTR   ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_D0_REG)
            #define UpperArmTiltRotatePWM_COMPARE2_LSB       (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
            #define UpperArmTiltRotatePWM_COMPARE2_LSB_PTR   ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
            #define UpperArmTiltRotatePWM_COUNTERCAP_LSB     (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_A1_REG)
            #define UpperArmTiltRotatePWM_COUNTERCAP_LSB_PTR ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_A1_REG)
            #define UpperArmTiltRotatePWM_COUNTER_LSB        (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_A0_REG)
            #define UpperArmTiltRotatePWM_COUNTER_LSB_PTR    ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_A0_REG)
            #define UpperArmTiltRotatePWM_CAPTURE_LSB        (*(reg16 *) UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_F1_REG)
            #define UpperArmTiltRotatePWM_CAPTURE_LSB_PTR    ((reg16 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define UpperArmTiltRotatePWM_AUX_CONTROLDP1          (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u1__DP_AUX_CTL_REG)
       #define UpperArmTiltRotatePWM_AUX_CONTROLDP1_PTR      ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (UpperArmTiltRotatePWM_Resolution == 8) */

   #define UpperArmTiltRotatePWM_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
   #define UpperArmTiltRotatePWM_AUX_CONTROLDP0          (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__DP_AUX_CTL_REG)
   #define UpperArmTiltRotatePWM_AUX_CONTROLDP0_PTR      ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sP16_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (UpperArmTiltRotatePWM_UsingFixedFunction) */

#if(UpperArmTiltRotatePWM_KillModeMinTime )
    #define UpperArmTiltRotatePWM_KILLMODEMINTIME        (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define UpperArmTiltRotatePWM_KILLMODEMINTIME_PTR    ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (UpperArmTiltRotatePWM_KillModeMinTime ) */

#if(UpperArmTiltRotatePWM_DeadBandMode == UpperArmTiltRotatePWM__B_PWM__DBM_256_CLOCKS)
    #define UpperArmTiltRotatePWM_DEADBAND_COUNT         (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define UpperArmTiltRotatePWM_DEADBAND_COUNT_PTR     ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define UpperArmTiltRotatePWM_DEADBAND_LSB_PTR       ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define UpperArmTiltRotatePWM_DEADBAND_LSB           (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(UpperArmTiltRotatePWM_DeadBandMode == UpperArmTiltRotatePWM__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (UpperArmTiltRotatePWM_UsingFixedFunction)
        #define UpperArmTiltRotatePWM_DEADBAND_COUNT         (*(reg8 *)  UpperArmTiltRotatePWM_PWMHW__CFG0)
        #define UpperArmTiltRotatePWM_DEADBAND_COUNT_PTR     ((reg8 *)   UpperArmTiltRotatePWM_PWMHW__CFG0)
        #define UpperArmTiltRotatePWM_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << UpperArmTiltRotatePWM_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define UpperArmTiltRotatePWM_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define UpperArmTiltRotatePWM_DEADBAND_COUNT         (*(reg8 *)  UpperArmTiltRotatePWM_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define UpperArmTiltRotatePWM_DEADBAND_COUNT_PTR     ((reg8 *)   UpperArmTiltRotatePWM_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define UpperArmTiltRotatePWM_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << UpperArmTiltRotatePWM_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define UpperArmTiltRotatePWM_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (UpperArmTiltRotatePWM_UsingFixedFunction) */
#endif /* (UpperArmTiltRotatePWM_DeadBandMode == UpperArmTiltRotatePWM__B_PWM__DBM_256_CLOCKS) */



#if (UpperArmTiltRotatePWM_UsingFixedFunction)
    #define UpperArmTiltRotatePWM_STATUS                 (*(reg8 *) UpperArmTiltRotatePWM_PWMHW__SR0)
    #define UpperArmTiltRotatePWM_STATUS_PTR             ((reg8 *) UpperArmTiltRotatePWM_PWMHW__SR0)
    #define UpperArmTiltRotatePWM_STATUS_MASK            (*(reg8 *) UpperArmTiltRotatePWM_PWMHW__SR0)
    #define UpperArmTiltRotatePWM_STATUS_MASK_PTR        ((reg8 *) UpperArmTiltRotatePWM_PWMHW__SR0)
    #define UpperArmTiltRotatePWM_CONTROL                (*(reg8 *) UpperArmTiltRotatePWM_PWMHW__CFG0)
    #define UpperArmTiltRotatePWM_CONTROL_PTR            ((reg8 *) UpperArmTiltRotatePWM_PWMHW__CFG0)
    #define UpperArmTiltRotatePWM_CONTROL2               (*(reg8 *) UpperArmTiltRotatePWM_PWMHW__CFG1)
    #define UpperArmTiltRotatePWM_CONTROL3               (*(reg8 *) UpperArmTiltRotatePWM_PWMHW__CFG2)
    #define UpperArmTiltRotatePWM_GLOBAL_ENABLE          (*(reg8 *) UpperArmTiltRotatePWM_PWMHW__PM_ACT_CFG)
    #define UpperArmTiltRotatePWM_GLOBAL_ENABLE_PTR      ( (reg8 *) UpperArmTiltRotatePWM_PWMHW__PM_ACT_CFG)
    #define UpperArmTiltRotatePWM_GLOBAL_STBY_ENABLE     (*(reg8 *) UpperArmTiltRotatePWM_PWMHW__PM_STBY_CFG)
    #define UpperArmTiltRotatePWM_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) UpperArmTiltRotatePWM_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define UpperArmTiltRotatePWM_BLOCK_EN_MASK          (UpperArmTiltRotatePWM_PWMHW__PM_ACT_MSK)
    #define UpperArmTiltRotatePWM_BLOCK_STBY_EN_MASK     (UpperArmTiltRotatePWM_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define UpperArmTiltRotatePWM_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define UpperArmTiltRotatePWM_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define UpperArmTiltRotatePWM_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define UpperArmTiltRotatePWM_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define UpperArmTiltRotatePWM_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define UpperArmTiltRotatePWM_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define UpperArmTiltRotatePWM_CTRL_ENABLE            (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_CTRL_ENABLE_SHIFT)
    #define UpperArmTiltRotatePWM_CTRL_RESET             (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_CTRL_RESET_SHIFT)
    #define UpperArmTiltRotatePWM_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << UpperArmTiltRotatePWM_CTRL_CMPMODE2_SHIFT)
    #define UpperArmTiltRotatePWM_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << UpperArmTiltRotatePWM_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define UpperArmTiltRotatePWM_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define UpperArmTiltRotatePWM_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define UpperArmTiltRotatePWM_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define UpperArmTiltRotatePWM_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define UpperArmTiltRotatePWM_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define UpperArmTiltRotatePWM_STATUS_TC_INT_EN_MASK_SHIFT            (UpperArmTiltRotatePWM_STATUS_TC_SHIFT - 4u)
    #define UpperArmTiltRotatePWM_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define UpperArmTiltRotatePWM_STATUS_CMP1_INT_EN_MASK_SHIFT          (UpperArmTiltRotatePWM_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define UpperArmTiltRotatePWM_STATUS_TC              (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_STATUS_TC_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_CMP1            (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define UpperArmTiltRotatePWM_STATUS_TC_INT_EN_MASK              (uint8)((uint8)UpperArmTiltRotatePWM_STATUS_TC >> 4u)
    #define UpperArmTiltRotatePWM_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)UpperArmTiltRotatePWM_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define UpperArmTiltRotatePWM_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define UpperArmTiltRotatePWM_RT1_MASK              (uint8)((uint8)0x03u << UpperArmTiltRotatePWM_RT1_SHIFT)
    #define UpperArmTiltRotatePWM_SYNC                  (uint8)((uint8)0x03u << UpperArmTiltRotatePWM_RT1_SHIFT)
    #define UpperArmTiltRotatePWM_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define UpperArmTiltRotatePWM_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << UpperArmTiltRotatePWM_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define UpperArmTiltRotatePWM_SYNCDSI_EN            (uint8)((uint8)0x0Fu << UpperArmTiltRotatePWM_SYNCDSI_SHIFT)


#else
    #define UpperArmTiltRotatePWM_STATUS                (*(reg8 *)   UpperArmTiltRotatePWM_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define UpperArmTiltRotatePWM_STATUS_PTR            ((reg8 *)    UpperArmTiltRotatePWM_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define UpperArmTiltRotatePWM_STATUS_MASK           (*(reg8 *)   UpperArmTiltRotatePWM_PWMUDB_genblk8_stsreg__MASK_REG)
    #define UpperArmTiltRotatePWM_STATUS_MASK_PTR       ((reg8 *)    UpperArmTiltRotatePWM_PWMUDB_genblk8_stsreg__MASK_REG)
    #define UpperArmTiltRotatePWM_STATUS_AUX_CTRL       (*(reg8 *)   UpperArmTiltRotatePWM_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define UpperArmTiltRotatePWM_CONTROL               (*(reg8 *)   UpperArmTiltRotatePWM_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define UpperArmTiltRotatePWM_CONTROL_PTR           ((reg8 *)    UpperArmTiltRotatePWM_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define UpperArmTiltRotatePWM_CTRL_ENABLE_SHIFT      (0x07u)
    #define UpperArmTiltRotatePWM_CTRL_RESET_SHIFT       (0x06u)
    #define UpperArmTiltRotatePWM_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define UpperArmTiltRotatePWM_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define UpperArmTiltRotatePWM_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define UpperArmTiltRotatePWM_CTRL_ENABLE            (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_CTRL_ENABLE_SHIFT)
    #define UpperArmTiltRotatePWM_CTRL_RESET             (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_CTRL_RESET_SHIFT)
    #define UpperArmTiltRotatePWM_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << UpperArmTiltRotatePWM_CTRL_CMPMODE2_SHIFT)
    #define UpperArmTiltRotatePWM_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << UpperArmTiltRotatePWM_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define UpperArmTiltRotatePWM_STATUS_KILL_SHIFT          (0x05u)
    #define UpperArmTiltRotatePWM_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define UpperArmTiltRotatePWM_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define UpperArmTiltRotatePWM_STATUS_TC_SHIFT            (0x02u)
    #define UpperArmTiltRotatePWM_STATUS_CMP2_SHIFT          (0x01u)
    #define UpperArmTiltRotatePWM_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define UpperArmTiltRotatePWM_STATUS_KILL_INT_EN_MASK_SHIFT          (UpperArmTiltRotatePWM_STATUS_KILL_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (UpperArmTiltRotatePWM_STATUS_FIFONEMPTY_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (UpperArmTiltRotatePWM_STATUS_FIFOFULL_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_TC_INT_EN_MASK_SHIFT            (UpperArmTiltRotatePWM_STATUS_TC_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_CMP2_INT_EN_MASK_SHIFT          (UpperArmTiltRotatePWM_STATUS_CMP2_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_CMP1_INT_EN_MASK_SHIFT          (UpperArmTiltRotatePWM_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define UpperArmTiltRotatePWM_STATUS_KILL            (uint8)((uint8)0x00u << UpperArmTiltRotatePWM_STATUS_KILL_SHIFT )
    #define UpperArmTiltRotatePWM_STATUS_FIFOFULL        (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_STATUS_FIFOFULL_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_STATUS_FIFONEMPTY_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_TC              (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_STATUS_TC_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_CMP2            (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_STATUS_CMP2_SHIFT)
    #define UpperArmTiltRotatePWM_STATUS_CMP1            (uint8)((uint8)0x01u << UpperArmTiltRotatePWM_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define UpperArmTiltRotatePWM_STATUS_KILL_INT_EN_MASK            (UpperArmTiltRotatePWM_STATUS_KILL)
    #define UpperArmTiltRotatePWM_STATUS_FIFOFULL_INT_EN_MASK        (UpperArmTiltRotatePWM_STATUS_FIFOFULL)
    #define UpperArmTiltRotatePWM_STATUS_FIFONEMPTY_INT_EN_MASK      (UpperArmTiltRotatePWM_STATUS_FIFONEMPTY)
    #define UpperArmTiltRotatePWM_STATUS_TC_INT_EN_MASK              (UpperArmTiltRotatePWM_STATUS_TC)
    #define UpperArmTiltRotatePWM_STATUS_CMP2_INT_EN_MASK            (UpperArmTiltRotatePWM_STATUS_CMP2)
    #define UpperArmTiltRotatePWM_STATUS_CMP1_INT_EN_MASK            (UpperArmTiltRotatePWM_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define UpperArmTiltRotatePWM_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define UpperArmTiltRotatePWM_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define UpperArmTiltRotatePWM_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define UpperArmTiltRotatePWM_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define UpperArmTiltRotatePWM_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* UpperArmTiltRotatePWM_UsingFixedFunction */

#endif  /* CY_PWM_UpperArmTiltRotatePWM_H */


/* [] END OF FILE */
