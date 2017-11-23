#include "Right_HB25_Enable_Pin_aliases.h"
#include "cypins.h"
#include "cyfitter.h"
#include "cytypes.h"


void Right_HB25_Enable_Pin_Write(uint8 value);

void Right_HB25_Enable_Pin_SetDriveMode(uint8 mode);

uint8 Right_HB25_Enable_Pin_ReadDataReg(void);

uint8 Right_HB25_Enable_Pin_Read(void);

void Right_HB25_Enable_Pin_SetInterruptMode(uint16 position, uint16 mode);

uint8 Right_HB25_Enable_Pin_ClearInterrupt(void);
