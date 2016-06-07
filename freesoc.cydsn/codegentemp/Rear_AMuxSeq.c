/*******************************************************************************
* File Name: Rear_AMuxSeq.c
* Version 1.80
*
*  Description:
*    This file contains functions for the AMuxSeq.
*
*   Note:
*
*******************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "Rear_AMuxSeq.h"

uint8 Rear_AMuxSeq_initVar = 0u;


/*******************************************************************************
* Function Name: Rear_AMuxSeq_Start
********************************************************************************
* Summary:
*  Disconnect all channels. The next time Next is called, channel 0 will be
*  connected.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Rear_AMuxSeq_Start(void)
{
    Rear_AMuxSeq_DisconnectAll();
    Rear_AMuxSeq_initVar = 1u;
}


/*******************************************************************************
* Function Name: Rear_AMuxSeq_Init
********************************************************************************
* Summary:
*  Disconnect all channels. The next time Next is called, channel 0 will be
*  connected.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Rear_AMuxSeq_Init(void)
{
    Rear_AMuxSeq_DisconnectAll();
}


/*******************************************************************************
* Function Name: Rear_AMuxSeq_Stop
********************************************************************************
* Summary:
*  Disconnect all channels. The next time Next is called, channel 0 will be
*  connected.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Rear_AMuxSeq_Stop(void)
{
    Rear_AMuxSeq_DisconnectAll();
}

#if (Rear_AMuxSeq_MUXTYPE == Rear_AMuxSeq_MUX_DIFF)

/*******************************************************************************
* Function Name: Rear_AMuxSeq_Next
********************************************************************************
* Summary:
*  Disconnects the previous channel and connects the next one in the sequence.
*  When Next is called for the first time after Init, Start, Enable, Stop, or
*  DisconnectAll, it connects channel 0.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Rear_AMuxSeq_Next(void)
{
    Rear_AMuxSeq_CYAMUXSIDE_A_Next();
    Rear_AMuxSeq_CYAMUXSIDE_B_Next();
}


/*******************************************************************************
* Function Name: Rear_AMuxSeq_DisconnectAll
********************************************************************************
* Summary:
*  This function disconnects all channels. The next time Next is called, channel
*  0 will be connected.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Rear_AMuxSeq_DisconnectAll(void)
{
    Rear_AMuxSeq_CYAMUXSIDE_A_DisconnectAll();
    Rear_AMuxSeq_CYAMUXSIDE_B_DisconnectAll();
}


/*******************************************************************************
* Function Name: Rear_AMuxSeq_GetChannel
********************************************************************************
* Summary:
*  The currently connected channel is retuned. If no channel is connected
*  returns -1.
*
* Parameters:
*  void
*
* Return:
*  The current channel or -1.
*
*******************************************************************************/
int8 Rear_AMuxSeq_GetChannel(void)
{
    return Rear_AMuxSeq_CYAMUXSIDE_A_curChannel;
}

#else

/*******************************************************************************
* Function Name: Rear_AMuxSeq_GetChannel
********************************************************************************
* Summary:
*  The currently connected channel is retuned. If no channel is connected
*  returns -1.
*
* Parameters:
*  void
*
* Return:
*  The current channel or -1.
*
*******************************************************************************/
int8 Rear_AMuxSeq_GetChannel(void)
{
    return Rear_AMuxSeq_curChannel;
}

#endif


/* [] END OF FILE */
