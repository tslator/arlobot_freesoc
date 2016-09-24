/*******************************************************************************
* File Name: Front_CliffMux.h
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

#if !defined(CY_AMUX_Front_CliffMux_H)
#define CY_AMUX_Front_CliffMux_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cyfitter_cfg.h"


/***************************************
*        Function Prototypes
***************************************/

void Front_CliffMux_Start(void) ;
#define Front_CliffMux_Init() Front_CliffMux_Start()
void Front_CliffMux_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void Front_CliffMux_Stop(void); */
/* void Front_CliffMux_Select(uint8 channel); */
/* void Front_CliffMux_Connect(uint8 channel); */
/* void Front_CliffMux_Disconnect(uint8 channel); */
/* void Front_CliffMux_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define Front_CliffMux_CHANNELS  2u
#define Front_CliffMux_MUXTYPE   1
#define Front_CliffMux_ATMOSTONE 0

/***************************************
*             API Constants
***************************************/

#define Front_CliffMux_NULL_CHANNEL 0xFFu
#define Front_CliffMux_MUX_SINGLE   1
#define Front_CliffMux_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if Front_CliffMux_MUXTYPE == Front_CliffMux_MUX_SINGLE
# if !Front_CliffMux_ATMOSTONE
#  define Front_CliffMux_Connect(channel) Front_CliffMux_Set(channel)
# endif
# define Front_CliffMux_Disconnect(channel) Front_CliffMux_Unset(channel)
#else
# if !Front_CliffMux_ATMOSTONE
void Front_CliffMux_Connect(uint8 channel) ;
# endif
void Front_CliffMux_Disconnect(uint8 channel) ;
#endif

#if Front_CliffMux_ATMOSTONE
# define Front_CliffMux_Stop() Front_CliffMux_DisconnectAll()
# define Front_CliffMux_Select(channel) Front_CliffMux_FastSelect(channel)
void Front_CliffMux_DisconnectAll(void) ;
#else
# define Front_CliffMux_Stop() Front_CliffMux_Start()
void Front_CliffMux_Select(uint8 channel) ;
# define Front_CliffMux_DisconnectAll() Front_CliffMux_Start()
#endif

#endif /* CY_AMUX_Front_CliffMux_H */


/* [] END OF FILE */
