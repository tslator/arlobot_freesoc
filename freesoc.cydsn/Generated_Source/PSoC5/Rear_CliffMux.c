/*******************************************************************************
* File Name: Rear_CliffMux.c
* Version 1.80
*
*  Description:
*    This file contains all functions required for the analog multiplexer
*    AMux User Module.
*
*   Note:
*
*******************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "Rear_CliffMux.h"

static uint8 Rear_CliffMux_lastChannel = Rear_CliffMux_NULL_CHANNEL;


/*******************************************************************************
* Function Name: Rear_CliffMux_Start
********************************************************************************
* Summary:
*  Disconnect all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Rear_CliffMux_Start(void) 
{
    uint8 chan;

    for(chan = 0u; chan < Rear_CliffMux_CHANNELS ; chan++)
    {
#if (Rear_CliffMux_MUXTYPE == Rear_CliffMux_MUX_SINGLE)
        Rear_CliffMux_Unset(chan);
#else
        Rear_CliffMux_CYAMUXSIDE_A_Unset(chan);
        Rear_CliffMux_CYAMUXSIDE_B_Unset(chan);
#endif
    }

    Rear_CliffMux_lastChannel = Rear_CliffMux_NULL_CHANNEL;
}


#if (!Rear_CliffMux_ATMOSTONE)
/*******************************************************************************
* Function Name: Rear_CliffMux_Select
********************************************************************************
* Summary:
*  This functions first disconnects all channels then connects the given
*  channel.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void Rear_CliffMux_Select(uint8 channel) 
{
    Rear_CliffMux_DisconnectAll();        /* Disconnect all previous connections */
    Rear_CliffMux_Connect(channel);       /* Make the given selection */
    Rear_CliffMux_lastChannel = channel;  /* Update last channel */
}
#endif


/*******************************************************************************
* Function Name: Rear_CliffMux_FastSelect
********************************************************************************
* Summary:
*  This function first disconnects the last connection made with FastSelect or
*  Select, then connects the given channel. The FastSelect function is similar
*  to the Select function, except it is faster since it only disconnects the
*  last channel selected rather than all channels.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void Rear_CliffMux_FastSelect(uint8 channel) 
{
    /* Disconnect the last valid channel */
    if( Rear_CliffMux_lastChannel != Rear_CliffMux_NULL_CHANNEL)
    {
        Rear_CliffMux_Disconnect(Rear_CliffMux_lastChannel);
    }

    /* Make the new channel connection */
#if (Rear_CliffMux_MUXTYPE == Rear_CliffMux_MUX_SINGLE)
    Rear_CliffMux_Set(channel);
#else
    Rear_CliffMux_CYAMUXSIDE_A_Set(channel);
    Rear_CliffMux_CYAMUXSIDE_B_Set(channel);
#endif


    Rear_CliffMux_lastChannel = channel;   /* Update last channel */
}


#if (Rear_CliffMux_MUXTYPE == Rear_CliffMux_MUX_DIFF)
#if (!Rear_CliffMux_ATMOSTONE)
/*******************************************************************************
* Function Name: Rear_CliffMux_Connect
********************************************************************************
* Summary:
*  This function connects the given channel without affecting other connections.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void Rear_CliffMux_Connect(uint8 channel) 
{
    Rear_CliffMux_CYAMUXSIDE_A_Set(channel);
    Rear_CliffMux_CYAMUXSIDE_B_Set(channel);
}
#endif

/*******************************************************************************
* Function Name: Rear_CliffMux_Disconnect
********************************************************************************
* Summary:
*  This function disconnects the given channel from the common or output
*  terminal without affecting other connections.
*
* Parameters:
*  channel:  The channel to disconnect from the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void Rear_CliffMux_Disconnect(uint8 channel) 
{
    Rear_CliffMux_CYAMUXSIDE_A_Unset(channel);
    Rear_CliffMux_CYAMUXSIDE_B_Unset(channel);
}
#endif

#if (Rear_CliffMux_ATMOSTONE)
/*******************************************************************************
* Function Name: Rear_CliffMux_DisconnectAll
********************************************************************************
* Summary:
*  This function disconnects all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Rear_CliffMux_DisconnectAll(void) 
{
    if(Rear_CliffMux_lastChannel != Rear_CliffMux_NULL_CHANNEL) 
    {
        Rear_CliffMux_Disconnect(Rear_CliffMux_lastChannel);
        Rear_CliffMux_lastChannel = Rear_CliffMux_NULL_CHANNEL;
    }
}
#endif

/* [] END OF FILE */
