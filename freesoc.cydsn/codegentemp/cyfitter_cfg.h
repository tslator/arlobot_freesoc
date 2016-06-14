/*******************************************************************************
* File Name: cyfitter_cfg.h
* 
* PSoC Creator  3.3 CP3
*
* Description:
* This file provides basic startup and mux configration settings
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright (c) 2007-2016 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#ifndef CYFITTER_CFG_H
#define CYFITTER_CFG_H

#include "cytypes.h"

extern void cyfitter_cfg(void);

/* Analog Set/Unset methods */
extern void SetAnalogRoutingPumps(uint8 enabled);
extern void Front_AMuxSeq_DisconnectAll(void);
extern void Front_AMuxSeq_Next(void);
extern int8 Front_AMuxSeq_curChannel;
extern void Rear_AMuxSeq_DisconnectAll(void);
extern void Rear_AMuxSeq_Next(void);
extern int8 Rear_AMuxSeq_curChannel;


#endif /* CYFITTER_CFG_H */

/*[]*/
