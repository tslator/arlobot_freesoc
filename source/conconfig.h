#ifndef CONCONFIG_H
#define CONCONFIG_H

#include "freesoc.h"
#include "concmd.h"

void ConConfig_Init(void);
void ConConfig_Start(void);
CONCMD_IF_TYPE * const ConConfig_InitConfigDebug(BOOL enable, UINT16 mask);
CONCMD_IF_TYPE * const ConConfig_InitConfigShow(UINT16 mask, BOOL plain_text);
CONCMD_IF_TYPE * const ConConfig_InitConfigClear(UINT16 mask, BOOL plain_text);

#endif