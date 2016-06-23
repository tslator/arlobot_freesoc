/*******************************************************************************
* File Name: .h
* Version 2.0
*
* Description:
*  This file provides private constants and parameter values for the EZI2C
*  component.
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_EZI2C_PVT_EZI2C_Slave_H)
#define CY_EZI2C_PVT_EZI2C_Slave_H

#include "EZI2C_Slave.h"


/***************************************
*     Vars with External Linkage
***************************************/

extern EZI2C_Slave_BACKUP_STRUCT  EZI2C_Slave_backup;

/* Status and state variables */
extern volatile uint8 EZI2C_Slave_curStatus;
extern volatile uint8 EZI2C_Slave_curState;

/* Primary slave address variables */
extern volatile uint8* EZI2C_Slave_dataPtrS1;
extern volatile uint16 EZI2C_Slave_bufSizeS1;
extern volatile uint16 EZI2C_Slave_wrProtectS1;
extern volatile uint8 EZI2C_Slave_rwOffsetS1;
extern volatile uint8 EZI2C_Slave_rwIndexS1;

/* Secondary slave address variables */
#if (EZI2C_Slave_ADDRESSES == EZI2C_Slave_TWO_ADDRESSES)
    extern volatile uint8  EZI2C_Slave_addrS1;
    extern volatile uint8  EZI2C_Slave_addrS2;
    extern volatile uint8* EZI2C_Slave_dataPtrS2;
    extern volatile uint16 EZI2C_Slave_bufSizeS2;
    extern volatile uint16 EZI2C_Slave_wrProtectS2;
    extern volatile uint8 EZI2C_Slave_rwOffsetS2;
    extern volatile uint8 EZI2C_Slave_rwIndexS2;
#endif  /* (EZI2C_Slave_ADDRESSES == EZI2C_Slave_TWO_ADDRESSES) */

#if (EZI2C_Slave_WAKEUP_ENABLED)
    extern volatile uint8 EZI2C_Slave_wakeupSource;
#endif /* (EZI2C_Slave_WAKEUP_ENABLED) */

#endif /* CY_EZI2C_PVT_EZI2C_Slave_H */


/* [] END OF FILE */
