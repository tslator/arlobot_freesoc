/*******************************************************************************
* File Name: `$INSTANCE_NAME`.h  
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides constants and parameter values for the Quadrature
*  Decoder component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_QUADRATURE_DECODER_`$INSTANCE_NAME`_H)
#define CY_QUADRATURE_DECODER_`$INSTANCE_NAME`_H

#include "cyfitter.h"
#include "CyLib.h"
#include "cytypes.h"

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component `$CY_COMPONENT_NAME` requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#define `$INSTANCE_NAME`_COUNTER_SIZE               (`$CounterSize`u)
#define `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT         (8u)
#define `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT        (16u)
#define `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT        (32u)

#if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT)
    #include "`$INSTANCE_NAME`_Cnt8.h"
#else 
    /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT) || 
    *  (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT) 
    */
    #include "`$INSTANCE_NAME`_Cnt16.h"
#endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT */

extern uint8 `$INSTANCE_NAME`_initVar;


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define `$INSTANCE_NAME`_COUNTER_RESOLUTION         (`$CounterResolution`u)


/***************************************
*       Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
} `$INSTANCE_NAME`_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  `$INSTANCE_NAME`_Init(void) `=ReentrantKeil($INSTANCE_NAME . "_Init")`;
void  `$INSTANCE_NAME`_Start(void) `=ReentrantKeil($INSTANCE_NAME . "_Start")`;
void  `$INSTANCE_NAME`_Stop(void) `=ReentrantKeil($INSTANCE_NAME . "_Stop")`;
void  `$INSTANCE_NAME`_Enable(void) `=ReentrantKeil($INSTANCE_NAME . "_Enable")`;
uint8 `$INSTANCE_NAME`_GetEvents(void) `=ReentrantKeil($INSTANCE_NAME . "_GetEvents")`;
void  `$INSTANCE_NAME`_SetInterruptMask(uint8 mask) `=ReentrantKeil($INSTANCE_NAME . "_SetInterruptMask")`;
uint8 `$INSTANCE_NAME`_GetInterruptMask(void) `=ReentrantKeil($INSTANCE_NAME . "_GetInterruptMask")`;
`$CounterSizeReplacementString` `$INSTANCE_NAME`_GetCounter(void) `=ReentrantKeil($INSTANCE_NAME . "_GetCounter")`;
void  `$INSTANCE_NAME`_SetCounter(`$CounterSizeReplacementString` value)
`=ReentrantKeil($INSTANCE_NAME . "_SetCounter")`;
void  `$INSTANCE_NAME`_Sleep(void) `=ReentrantKeil($INSTANCE_NAME . "_Sleep")`;
void  `$INSTANCE_NAME`_Wakeup(void) `=ReentrantKeil($INSTANCE_NAME . "_Wakeup")`;
void  `$INSTANCE_NAME`_SaveConfig(void) `=ReentrantKeil($INSTANCE_NAME . "_SaveConfig")`;
void  `$INSTANCE_NAME`_RestoreConfig(void) `=ReentrantKeil($INSTANCE_NAME . "_RestoreConfig")`;

#if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
    CY_ISR_PROTO(`$INSTANCE_NAME`_ISR);
#endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT */


/***************************************
*           API Constants
***************************************/

#if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
    #define `$INSTANCE_NAME`_ISR_NUMBER             ((uint8) `$INSTANCE_NAME``[isr]`_INTC_NUMBER)
    #define `$INSTANCE_NAME`_ISR_PRIORITY           ((uint8) `$INSTANCE_NAME``[isr]`_INTC_PRIOR_NUM)
#endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT */


/***************************************
*    Enumerated Types and Parameters
***************************************/

#define `$INSTANCE_NAME`_GLITCH_FILTERING           (`$UsingGlitchFiltering`u)
#define `$INSTANCE_NAME`_INDEX_INPUT                (`$UsingIndexInput`u)


/***************************************
*    Initial Parameter Constants
***************************************/

#if (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT)
    #define `$INSTANCE_NAME`_COUNTER_INIT_VALUE    (0x80u)
#else 
    /* (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_16_BIT) ||
    *  (`$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_32_BIT)
    */
    #define `$INSTANCE_NAME`_COUNTER_INIT_VALUE    (0x8000u)
    #define `$INSTANCE_NAME`_COUNTER_MAX_VALUE     (0x7FFFu)
#endif /* `$INSTANCE_NAME`_COUNTER_SIZE == `$INSTANCE_NAME`_COUNTER_SIZE_8_BIT */


/***************************************
*             Registers
***************************************/

#define `$INSTANCE_NAME`_STATUS_REG                 (* (reg8 *) `$INSTANCE_NAME`_bQuadDec_Stsreg__STATUS_REG)
#define `$INSTANCE_NAME`_STATUS_PTR                 (  (reg8 *) `$INSTANCE_NAME`_bQuadDec_Stsreg__STATUS_REG)
#define `$INSTANCE_NAME`_STATUS_MASK                (* (reg8 *) `$INSTANCE_NAME`_bQuadDec_Stsreg__MASK_REG)
#define `$INSTANCE_NAME`_STATUS_MASK_PTR            (  (reg8 *) `$INSTANCE_NAME`_bQuadDec_Stsreg__MASK_REG)
#define `$INSTANCE_NAME`_SR_AUX_CONTROL             (* (reg8 *) `$INSTANCE_NAME`_bQuadDec_Stsreg__STATUS_AUX_CTL_REG)
#define `$INSTANCE_NAME`_SR_AUX_CONTROL_PTR         (  (reg8 *) `$INSTANCE_NAME`_bQuadDec_Stsreg__STATUS_AUX_CTL_REG)


/***************************************
*        Register Constants
***************************************/

#define `$INSTANCE_NAME`_COUNTER_OVERFLOW_SHIFT     (0x00u)
#define `$INSTANCE_NAME`_COUNTER_UNDERFLOW_SHIFT    (0x01u)
#define `$INSTANCE_NAME`_COUNTER_RESET_SHIFT        (0x02u)
#define `$INSTANCE_NAME`_INVALID_IN_SHIFT           (0x03u)
#define `$INSTANCE_NAME`_COUNTER_OVERFLOW           ((uint8) (0x01u << `$INSTANCE_NAME`_COUNTER_OVERFLOW_SHIFT))
#define `$INSTANCE_NAME`_COUNTER_UNDERFLOW          ((uint8) (0x01u << `$INSTANCE_NAME`_COUNTER_UNDERFLOW_SHIFT))
#define `$INSTANCE_NAME`_COUNTER_RESET              ((uint8) (0x01u << `$INSTANCE_NAME`_COUNTER_RESET_SHIFT))
#define `$INSTANCE_NAME`_INVALID_IN                 ((uint8) (0x01u << `$INSTANCE_NAME`_INVALID_IN_SHIFT))

#define `$INSTANCE_NAME`_INTERRUPTS_ENABLE_SHIFT    (0x04u)
#define `$INSTANCE_NAME`_INTERRUPTS_ENABLE          ((uint8)(0x01u << `$INSTANCE_NAME`_INTERRUPTS_ENABLE_SHIFT))
#define `$INSTANCE_NAME`_INIT_INT_MASK              (0x0Fu)


/******************************************************************************************
* Following code are OBSOLETE and must not be used starting from Quadrature Decoder 2.20
******************************************************************************************/
#define `$INSTANCE_NAME`_DISABLE                    (0x00u)


#endif /* CY_QUADRATURE_DECODER_`$INSTANCE_NAME`_H */


/* [] END OF FILE */
