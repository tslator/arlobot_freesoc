#include "freesoc.h"








void Ser_Init();

void Ser_Start(UINT8 disable);

void Ser_Update();



void Ser_PutString(char *str);

void Ser_PutStringFormat(const char *fmt, ...);

UINT8 Ser_ReadData(UINT8 *data);

UINT8 Ser_ReadByte();

UINT8 Ser_ReadLine(char *line, UINT8 echo);

void Ser_WriteByte(UINT8 value);
