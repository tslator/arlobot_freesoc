#include "CyLib.h"
#include "cytypes.h"
#include "cyfitter.h"


extern uint8 Right_HB25_PWM_initVar;

typedef struct

{



    uint8 PWMEnableState;





        uint16 PWMUdb;



            uint16 PWMPeriod;

}Right_HB25_PWM_backupStruct;













void Right_HB25_PWM_Start(void) ;

void Right_HB25_PWM_Stop(void) ;

    uint16 Right_HB25_PWM_ReadCounter(void) ;

    uint16 Right_HB25_PWM_ReadCapture(void) ;











    void Right_HB25_PWM_WriteCounter(uint16 counter)

            ;





void Right_HB25_PWM_WritePeriod(uint16 period)

        ;

uint16 Right_HB25_PWM_ReadPeriod(void) ;





    void Right_HB25_PWM_WriteCompare(uint16 compare)

            ;

    uint16 Right_HB25_PWM_ReadCompare(void) ;

void Right_HB25_PWM_Init(void) ;

void Right_HB25_PWM_Enable(void) ;

void Right_HB25_PWM_Sleep(void) ;

void Right_HB25_PWM_Wakeup(void) ;

void Right_HB25_PWM_SaveConfig(void) ;

void Right_HB25_PWM_RestoreConfig(void) ;
