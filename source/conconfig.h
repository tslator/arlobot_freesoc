#ifndef CONCONFIG_H
#define CONCONFIG_H

#include "freesoc.h"
#include "concmd.h"

typedef enum { CONCONFIG_MOTOR_BIT=0x0001, CONCONFIG_PID_BIT=0x0002, CONCONFIG_BIAS_BIT=0x0004, CONCONFIG_DEBUG_BIT=0x0008, CONCONFIG_STATUS_BIT=0x0010, CONCONFIG_PARAMS_BIT=0x0020} CONCONFIG_BITS_TYPE;

void ConConfig_Init(void);
void ConConfig_Start(void);
CONCMD_IF_PTR_TYPE ConConfig_InitConfigDebug(BOOL enable, UINT16 mask);
CONCMD_IF_PTR_TYPE ConConfig_InitConfigShow(UINT16 mask, BOOL plain_text);
CONCMD_IF_PTR_TYPE ConConfig_InitConfigClear(UINT16 mask, BOOL plain_text);

#endif