/*******************************************************************************
* File Name: Front_CliffMux.c
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

#include "Front_CliffMux.h"

static uint8 Front_CliffMux_lastChannel = Front_CliffMux_NULL_CHANNEL;


/*******************************************************************************
* Function Name: Front_CliffMux_Start
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
void Front_CliffMux_Start(void) 
{
    uint8 chan;

    for(chan = 0u; chan < Front_CliffMux_CHANNELS ; chan++)
    {
#if (Front_CliffMux_MUXTYPE == Front_CliffMux_MUX_SINGLE)
        Front_CliffMux_Unset(chan);
#else
        Front_CliffMux_CYAMUXSIDE_A_Unset(chan);
        Front_CliffMux_CYAMUXSIDE_B_Unset(chan);
#endif
    }

    Front_CliffMux_lastChannel = Front_CliffMux_NULL_CHANNEL;
}


#if (!Front_CliffMux_ATMOSTONE)
/*******************************************************************************
* Function Name: Front_CliffMux_Select
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
void Front_CliffMux_Select(uint8 channel) 
{
    Front_CliffMux_DisconnectAll();        /* Disconnect all previous connections */
    Front_CliffMux_Connect(channel);       /* Make the given selection */
    Front_CliffMux_lastChannel = channel;  /* Update last channel */
}
#endif


/*******************************************************************************
* Function Name: Front_CliffMux_FastSelect
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
void Front_CliffMux_FastSelect(uint8 channel) 
{
    /* Disconnect the last valid channel */
    if( Front_CliffMux_lastChannel != Front_CliffMux_NULL_CHANNEL)
    {
        Front_CliffMux_Disconnect(Front_CliffMux_lastChannel);
    }

    /* Make the new channel connection */
#if (Front_CliffMux_MUXTYPE == Front_CliffMux_MUX_SINGLE)
    Front_CliffMux_Set(channel);
#else
    Front_CliffMux_CYAMUXSIDE_A_Set(channel);
    Front_CliffMux_CYAMUXSIDE_B_Set(channel);
#endif


    Front_CliffMux_lastChannel = channel;   /* Update last channel */
}


#if (Front_CliffMux_MUXTYPE == Front_CliffMux_MUX_DIFF)
#if (!Front_CliffMux_ATMOSTONE)
/*******************************************************************************
* Function Name: Front_CliffMux_Connect
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
void Front_CliffMux_Connect(uint8 channel) 
{
    Front_CliffMux_CYAMUXSIDE_A_Set(channel);
    Front_CliffMux_CYAMUXSIDE_B_Set(channel);
}
#endif

/*******************************************************************************
* Function Name: Front_CliffMux_Disconnect
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
void Front_CliffMux_Disconnect(uint8 channel) 
{
    Front_CliffMux_CYAMUXSIDE_A_Unset(channel);
    Front_CliffMux_CYAMUXSIDE_B_Unset(channel);
}
#endif

#if (Front_CliffMux_ATMOSTONE)
/*******************************************************************************
* Function Name: Front_CliffMux_DisconnectAll
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
void Front_CliffMux_DisconnectAll(void) 
{
    if(Front_CliffMux_lastChannel != Front_CliffMux_NULL_CHANNEL) 
    {
        Front_CliffMux_Disconnect(Front_CliffMux_lastChannel);
        Front_CliffMux_lastChannel = Front_CliffMux_NULL_CHANNEL;
    }
}
#endif

/* [] END OF FILE */
