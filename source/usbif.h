#ifndef USBIF_H
#define USBIF_H

#include "freesoc.h"

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)

void USBIF_Init(void);
void USBIF_Start(void);
void USBIF_Update(void);
void USBIF_PutString(CHAR* const str);
UINT8 USBIF_GetAll(CHAR* const data);
UINT8 USBIF_GetChar(void);
void USBIF_PutChar(CHAR value);
UINT8 USBIF_GetConnectState(void);

#endif // USBIF_H
