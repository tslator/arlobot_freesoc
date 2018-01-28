#ifndef USBIF_H
#define USBIF_H

#include "freesoc.h"

void USBIF_Init(void);
void USBIF_Start(void);
void USBIF_Update(void);
void USBIF_PutString(CHAR* const str);
UINT8 USBIF_GetAll(CHAR* const data);
UINT8 USBIF_GetChar(void);
void USBIF_PutChar(CHAR value);

#endif // USBIF_H
