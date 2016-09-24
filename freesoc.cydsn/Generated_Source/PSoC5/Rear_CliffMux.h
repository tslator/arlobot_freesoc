/*******************************************************************************
* File Name: Rear_CliffMux.h
* Version 1.80
*
*  Description:
*    This file contains the constants and function prototypes for the Analog
*    Multiplexer User Module AMux.
*
*   Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_AMUX_Rear_CliffMux_H)
#define CY_AMUX_Rear_CliffMux_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cyfitter_cfg.h"


/***************************************
*        Function Prototypes
***************************************/

void Rear_CliffMux_Start(void) ;
#define Rear_CliffMux_Init() Rear_CliffMux_Start()
void Rear_CliffMux_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void Rear_CliffMux_Stop(void); */
/* void Rear_CliffMux_Select(uint8 channel); */
/* void Rear_CliffMux_Connect(uint8 channel); */
/* void Rear_CliffMux_Disconnect(uint8 channel); */
/* void Rear_CliffMux_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define Rear_CliffMux_CHANNELS  2u
#define Rear_CliffMux_MUXTYPE   1
#define Rear_CliffMux_ATMOSTONE 0

/***************************************
*             API Constants
***************************************/

#define Rear_CliffMux_NULL_CHANNEL 0xFFu
#define Rear_CliffMux_MUX_SINGLE   1
#define Rear_CliffMux_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if Rear_CliffMux_MUXTYPE == Rear_CliffMux_MUX_SINGLE
# if !Rear_CliffMux_ATMOSTONE
#  define Rear_CliffMux_Connect(channel) Rear_CliffMux_Set(channel)
# endif
# define Rear_CliffMux_Disconnect(channel) Rear_CliffMux_Unset(channel)
#else
# if !Rear_CliffMux_ATMOSTONE
void Rear_CliffMux_Connect(uint8 channel) ;
# endif
void Rear_CliffMux_Disconnect(uint8 channel) ;
#endif

#if Rear_CliffMux_ATMOSTONE
# define Rear_CliffMux_Stop() Rear_CliffMux_DisconnectAll()
# define Rear_CliffMux_Select(channel) Rear_CliffMux_FastSelect(channel)
void Rear_CliffMux_DisconnectAll(void) ;
#else
# define Rear_CliffMux_Stop() Rear_CliffMux_Start()
void Rear_CliffMux_Select(uint8 channel) ;
# define Rear_CliffMux_DisconnectAll() Rear_CliffMux_Start()
#endif

#endif /* CY_AMUX_Rear_CliffMux_H */


/* [] END OF FILE */
