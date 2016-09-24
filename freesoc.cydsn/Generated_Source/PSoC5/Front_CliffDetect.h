/*******************************************************************************
* File Name: Front_CliffDetect.c
* Version 2.0
*
* Description:
*  This file contains the function prototypes and constants used in
*  the Analog Comparator User Module.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_COMP_Front_CliffDetect_H) 
#define CY_COMP_Front_CliffDetect_H

#include "cytypes.h"
#include "CyLib.h"
#include "cyfitter.h" 


#define Front_CliffDetect_RECALMODE (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Comp_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*       Type defines
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
   /* uint8 compCRReg; */
}Front_CliffDetect_backupStruct;

#if (CY_PSOC5A)
    /* Stop API changes for PSoC5A */
    typedef struct
    {
        uint8 compCRReg;
    }   Front_CliffDetect_LOWPOWER_BACKUP_STRUCT;
#endif /* CY_PSOC5A */

/* component init state */
extern uint8 Front_CliffDetect_initVar;


/**************************************
*        Function Prototypes 
**************************************/

void    Front_CliffDetect_Start(void)                  ;
void    Front_CliffDetect_Stop(void)                   ;
void    Front_CliffDetect_SetSpeed(uint8 speed)        ;
uint8   Front_CliffDetect_GetCompare(void)             ;
uint16  Front_CliffDetect_ZeroCal(void)                ;
void    Front_CliffDetect_LoadTrim(uint16 trimVal)     ;
void Front_CliffDetect_Init(void)                      ; 
void Front_CliffDetect_Enable(void)                    ;
void Front_CliffDetect_SaveConfig(void)                ;
void Front_CliffDetect_RestoreConfig(void)             ;
void Front_CliffDetect_Sleep(void)                     ;
void Front_CliffDetect_Wakeup(void)                    ;
/* Below APIs are valid only for PSoC3, PSoC5LP silicons.*/
#if (CY_PSOC3 || CY_PSOC5LP) 
    void Front_CliffDetect_PwrDwnOverrideEnable(void)  ;
    void Front_CliffDetect_PwrDwnOverrideDisable(void) ;
#endif /* CY_PSOC3 || CY_PSOC5LP */


/**************************************
*           API Constants        
**************************************/

/* Power constants for SetSpeed() function */
#define Front_CliffDetect_SLOWSPEED   (0x00u)
#define Front_CliffDetect_HIGHSPEED   (0x01u)
#define Front_CliffDetect_LOWPOWER    (0x02u)


/**************************************
*           Parameter Defaults        
**************************************/

#define Front_CliffDetect_DEFAULT_SPEED       (1u) 
#define Front_CliffDetect_DEFAULT_HYSTERESIS  (1u)
#define Front_CliffDetect_DEFAULT_POLARITY    (0u)
#define Front_CliffDetect_DEFAULT_BYPASS_SYNC (1u)
#define Front_CliffDetect_DEFAULT_PWRDWN_OVRD (0u)


/**************************************
*             Registers        
**************************************/
#define Front_CliffDetect_CR_REG            (* (reg8 *) Front_CliffDetect_ctComp__CR )               /* Config register */
#define Front_CliffDetect_CR_PTR            (  (reg8 *) Front_CliffDetect_ctComp__CR )
#define Front_CliffDetect_CLK_REG           (* (reg8 *) Front_CliffDetect_ctComp__CLK )  /* Comp clock control register */
#define Front_CliffDetect_CLK_PTR           (  (reg8 *) Front_CliffDetect_ctComp__CLK )
#define Front_CliffDetect_SW0_REG           (* (reg8 *) Front_CliffDetect_ctComp__SW0 )            /* Routing registers */
#define Front_CliffDetect_SW0_PTR           (  (reg8 *) Front_CliffDetect_ctComp__SW0 )
#define Front_CliffDetect_SW2_REG           (* (reg8 *) Front_CliffDetect_ctComp__SW2 )
#define Front_CliffDetect_SW2_PTR           (  (reg8 *) Front_CliffDetect_ctComp__SW2 )
#define Front_CliffDetect_SW3_REG           (* (reg8 *) Front_CliffDetect_ctComp__SW3 )
#define Front_CliffDetect_SW3_PTR           (  (reg8 *) Front_CliffDetect_ctComp__SW3 )
#define Front_CliffDetect_SW4_REG           (* (reg8 *) Front_CliffDetect_ctComp__SW4 )
#define Front_CliffDetect_SW4_PTR           (  (reg8 *) Front_CliffDetect_ctComp__SW4 )
#define Front_CliffDetect_SW6_REG           (* (reg8 *) Front_CliffDetect_ctComp__SW6 )
#define Front_CliffDetect_SW6_PTR           (  (reg8 *) Front_CliffDetect_ctComp__SW6 )

/* Trim registers */
/* PSoC5A */
#if (CY_PSOC5A)
    #define Front_CliffDetect_TR_REG        (* (reg8 *) Front_CliffDetect_ctComp__TR )                /* Trim registers */
    #define Front_CliffDetect_TR_PTR        (  (reg8 *) Front_CliffDetect_ctComp__TR )
#endif /* CY_PSOC5A */

/* PSoC3, PSoC5LP or later */
#if (CY_PSOC3 || CY_PSOC5LP) 
    #define Front_CliffDetect_TR0_REG       (* (reg8 *) Front_CliffDetect_ctComp__TR0 )/* Trim register for P-type load */
    #define Front_CliffDetect_TR0_PTR       (  (reg8 *) Front_CliffDetect_ctComp__TR0 ) 
    #define Front_CliffDetect_TR1_REG       (* (reg8 *) Front_CliffDetect_ctComp__TR1 )/* Trim register for N-type load */
    #define Front_CliffDetect_TR1_PTR       (  (reg8 *) Front_CliffDetect_ctComp__TR1 ) 
#endif /* CY_PSOC3 || CY_PSOC5LP */

#define Front_CliffDetect_WRK_REG           (* (reg8 *) Front_CliffDetect_ctComp__WRK )    /* Working register - output */
#define Front_CliffDetect_WRK_PTR           (  (reg8 *) Front_CliffDetect_ctComp__WRK )
#define Front_CliffDetect_PWRMGR_REG        (* (reg8 *) Front_CliffDetect_ctComp__PM_ACT_CFG )  /* Active Power manager */
#define Front_CliffDetect_PWRMGR_PTR        (  (reg8 *) Front_CliffDetect_ctComp__PM_ACT_CFG )
#define Front_CliffDetect_STBY_PWRMGR_REG   (* (reg8 *) Front_CliffDetect_ctComp__PM_STBY_CFG )/* Standby Power manager */
#define Front_CliffDetect_STBY_PWRMGR_PTR   (  (reg8 *) Front_CliffDetect_ctComp__PM_STBY_CFG )

/*      Trim Locations      */
#define Front_CliffDetect_ctComp__TRIM__TR0_HS_PTR     ((void CYFAR *) Front_CliffDetect_ctComp__TRIM__TR0_HS)
#define Front_CliffDetect_ctComp__TRIM__TR0_HS_REG     CY_GET_XTND_REG8(Front_CliffDetect_ctComp__TRIM__TR0_HS_PTR)
#if (CY_PSOC3 || CY_PSOC5LP)
    #define Front_CliffDetect_ctComp__TRIM__TR1_HS_PTR ((void CYFAR *) Front_CliffDetect_ctComp__TRIM__TR1_HS)
    #define Front_CliffDetect_ctComp__TRIM__TR1_HS_REG CY_GET_XTND_REG8(Front_CliffDetect_ctComp__TRIM__TR1_HS_PTR)
#endif /* (CY_PSOC3 || CY_PSOC5LP) */
#define Front_CliffDetect_ctComp__TRIM__TR0_LS_PTR     ((void CYFAR *) ((uint32)Front_CliffDetect_ctComp__TRIM__TR0 + 1u))
#define Front_CliffDetect_ctComp__TRIM__TR0_LS_REG     CY_GET_XTND_REG8(Front_CliffDetect_ctComp__TRIM__TR0_LS_PTR)
#if (CY_PSOC3 || CY_PSOC5LP)
    #define Front_CliffDetect_ctComp__TRIM__TR1_LS_PTR ((void CYFAR *) ((uint32)Front_CliffDetect_ctComp__TRIM__TR1 + 1u))
    #define Front_CliffDetect_ctComp__TRIM__TR1_LS_REG CY_GET_XTND_REG8(Front_CliffDetect_ctComp__TRIM__TR1_LS_PTR)
#endif /* CY_PSOC3 || CY_PSOC5LP */


/***************************************
*  Registers definitions
* for backward capability        
***************************************/
#define Front_CliffDetect_CR                (* (reg8 *) Front_CliffDetect_ctComp__CR )               /* Config register */
#define Front_CliffDetect_CLK               (* (reg8 *) Front_CliffDetect_ctComp__CLK )  /* Comp clock control register */
#define Front_CliffDetect_SW0               (* (reg8 *) Front_CliffDetect_ctComp__SW0 )            /* Routing registers */
#define Front_CliffDetect_SW2               (* (reg8 *) Front_CliffDetect_ctComp__SW2 )
#define Front_CliffDetect_SW3               (* (reg8 *) Front_CliffDetect_ctComp__SW3 )
#define Front_CliffDetect_SW4               (* (reg8 *) Front_CliffDetect_ctComp__SW4 )
#define Front_CliffDetect_SW6               (* (reg8 *) Front_CliffDetect_ctComp__SW6 )

/* Trim registers */
/* PSoC5A */
#if (CY_PSOC5A)
    #define Front_CliffDetect_TR            (* (reg8 *) Front_CliffDetect_ctComp__TR )                /* Trim registers */
#endif /* CY_PSOC5A */

/* PSoC3, PSoC5LP or later */
#if (CY_PSOC3 || CY_PSOC5LP) 
    #define Front_CliffDetect_TR0           (* (reg8 *) Front_CliffDetect_ctComp__TR0 )/* Trim register for P-type load */
    #define Front_CliffDetect_TR1           (* (reg8 *) Front_CliffDetect_ctComp__TR1 )/* Trim register for N-type load */
#endif /* CY_PSOC3 || CY_PSOC5LP */

#define Front_CliffDetect_WRK               (* (reg8 *) Front_CliffDetect_ctComp__WRK )    /* Working register - output */
#define Front_CliffDetect_PWRMGR            (* (reg8 *) Front_CliffDetect_ctComp__PM_ACT_CFG )  /* Active Power manager */
#define Front_CliffDetect_STBY_PWRMGR       (* (reg8 *) Front_CliffDetect_ctComp__PM_STBY_CFG )/* Standby Power manager */

/*      Trim Locations      */
#define Front_CliffDetect_HS_TRIM_TR0        CY_GET_XTND_REG8(Front_CliffDetect_ctComp__TRIM__TR0_HS_PTR)
#if (CY_PSOC3 || CY_PSOC5LP)
    #define Front_CliffDetect_HS_TRIM_TR1    CY_GET_XTND_REG8(Front_CliffDetect_ctComp__TRIM__TR1_HS_PTR)
#endif /* (CY_PSOC3 || CY_PSOC5LP) */
#define Front_CliffDetect_LS_TRIM_TR0        CY_GET_XTND_REG8(Front_CliffDetect_ctComp__TRIM__TR0_LS_PTR)
#if (CY_PSOC3 || CY_PSOC5LP)
    #define Front_CliffDetect_LS_TRIM_TR1    CY_GET_XTND_REG8(Front_CliffDetect_ctComp__TRIM__TR1_LS_PTR)
#endif /* CY_PSOC3 || CY_PSOC5LP */


/**************************************
*       Register Constants        
**************************************/

/* CR (Comp Control Register)             */
#define Front_CliffDetect_CFG_MODE_MASK      (0x78u)
#define Front_CliffDetect_FILTER_ON          (0x40u)
#define Front_CliffDetect_HYST_OFF           (0x20u)
#define Front_CliffDetect_CAL_ON             (0x10u)
#define Front_CliffDetect_MX_AO              (0x08u)
#define Front_CliffDetect_PWRDWN_OVRD        (0x04u)

#define Front_CliffDetect_PWR_MODE_SHIFT     (0x00u)
#define Front_CliffDetect_PWR_MODE_MASK      ((uint8)(0x03u << Front_CliffDetect_PWR_MODE_SHIFT))
#define Front_CliffDetect_PWR_MODE_SLOW      ((uint8)(0x00u << Front_CliffDetect_PWR_MODE_SHIFT))
#define Front_CliffDetect_PWR_MODE_FAST      ((uint8)(0x01u << Front_CliffDetect_PWR_MODE_SHIFT))
#define Front_CliffDetect_PWR_MODE_ULOW      ((uint8)(0x02u << Front_CliffDetect_PWR_MODE_SHIFT))

/* CLK (Comp Clock Control Register)      */
#define Front_CliffDetect_BYPASS_SYNC        (0x10u)
#define Front_CliffDetect_SYNC_CLK_EN        (0x08u)
#define Front_CliffDetect_SYNCCLK_MASK       (Front_CliffDetect_BYPASS_SYNC | Front_CliffDetect_SYNC_CLK_EN)

/* SW3 Routing Register definitions */
#define Front_CliffDetect_CMP_SW3_INPCTL_MASK (0x09u)   /* SW3 bits for inP routing control */

/* TR (Comp Trim Register)     */
#define Front_CliffDetect_DEFAULT_CMP_TRIM    (0x00u)

/* PSoC5A */
#if (CY_PSOC5A)
    #define Front_CliffDetect_CMP_TRIM1_DIR   (0x08u)   /* Trim direction for N-type load for offset calibration */
    #define Front_CliffDetect_CMP_TRIM1_MASK  (0x07u)   /* Trim for N-type load for offset calibration */
    #define Front_CliffDetect_CMP_TRIM2_DIR   (0x80u)   /* Trim direction for P-type load for offset calibration */
    #define Front_CliffDetect_CMP_TRIM2_MASK  (0x70u)   /* Trim for P-type load for offset calibration */
#endif /* CY_PSOC5A */

/* PSoC3, PSoC5LP or later */
#if (CY_PSOC3 || CY_PSOC5LP)
    #define Front_CliffDetect_CMP_TR0_DIR     (0x10u)   /* Trim direction for N-type load for offset calibration */
    #define Front_CliffDetect_CMP_TR0_MASK    (0x0Fu)   /* Trim for N-type load for offset calibration */
    #define Front_CliffDetect_CMP_TR1_DIR     (0x10u)   /* Trim direction for P-type load for offset calibration */
    #define Front_CliffDetect_CMP_TR1_MASK    (0x07u)   /* Trim for P-type load for offset calibration */ 
#endif /* CY_PSOC3 || CY_PSOC5LP */


/* WRK (Comp output working register)     */ 
#define Front_CliffDetect_CMP_OUT_MASK       Front_CliffDetect_ctComp__WRK_MASK /* Specific comparator out mask */

/* PM_ACT_CFG7 (Active Power Mode CFG Register)     */ 
#define Front_CliffDetect_ACT_PWR_EN         Front_CliffDetect_ctComp__PM_ACT_MSK /* Power enable mask */

/* PM_STBY_CFG7 (Standby Power Mode CFG Register)     */ 
#define Front_CliffDetect_STBY_PWR_EN        Front_CliffDetect_ctComp__PM_STBY_MSK /* Standby Power enable mask */

#if (CY_PSOC5A)
    /* For stop API changes mask to make the COMP register CR to 0X00  */
    #define Front_CliffDetect_COMP_REG_CLR   (0x00u)
#endif /* CY_PSOC5A */

#endif /* CY_COMP_Front_CliffDetect_H */


/* [] END OF FILE */
