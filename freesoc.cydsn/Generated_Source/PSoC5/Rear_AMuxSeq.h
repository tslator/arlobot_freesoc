/*******************************************************************************
* File Name: Rear_AMuxSeq.h
* Version 1.80
*
*  Description:
*    This file contains the constants and function prototypes for the AMuxSeq.
*
*   Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_AMUXSEQ_Rear_AMuxSeq_H)
#define CY_AMUXSEQ_Rear_AMuxSeq_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cyfitter_cfg.h"


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define Rear_AMuxSeq_MUX_SINGLE 1
#define Rear_AMuxSeq_MUX_DIFF   2
#define Rear_AMuxSeq_MUXTYPE    1


/***************************************
*        Function Prototypes
***************************************/

void Rear_AMuxSeq_Start(void);
void Rear_AMuxSeq_Init(void);
void Rear_AMuxSeq_Stop(void);
#if (Rear_AMuxSeq_MUXTYPE == Rear_AMuxSeq_MUX_DIFF)
void Rear_AMuxSeq_Next(void);
void Rear_AMuxSeq_DisconnectAll(void);
#else
/* The Next and DisconnectAll functions are declared in cyfitter_cfg.h. */
/* void Rear_AMuxSeq_Next(void); */
/* void Rear_AMuxSeq_DisconnectAll(void); */
#endif
int8 Rear_AMuxSeq_GetChannel(void);


/***************************************
*           Global Variables
***************************************/

extern uint8 Rear_AMuxSeq_initVar;


/***************************************
*         Parameter Constants
***************************************/
#define Rear_AMuxSeq_CHANNELS 8


#endif /* CY_AMUXSEQ_Rear_AMuxSeq_H */


/* [] END OF FILE */
