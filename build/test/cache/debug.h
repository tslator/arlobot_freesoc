#include "serial.h"
#include "config.h"
#include "freesoc.h"


char formatted_string[256];

void Debug_Init();

void Debug_Start();

void Debug_Enable(UINT16 flag);

void Debug_Disable(UINT16 flag);

UINT16 Debug_IsEnabled(UINT16 flag);

void Debug_EnableAll();

void Debug_DisableAll();

void Debug_Store();

void Debug_Restore();
