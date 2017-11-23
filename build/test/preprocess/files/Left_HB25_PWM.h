#include "CyLib.h"
#include "cytypes.h"
#include "cyfitter.h"


extern uint8 Left_HB25_PWM_initVar;

typedef struct

{



    uint8 PWMEnableState;





        uint16 PWMUdb;



            uint16 PWMPeriod;

}Left_HB25_PWM_backupStruct;













void Left_HB25_PWM_Start(void) ;

void Left_HB25_PWM_Stop(void) ;

    uint16 Left_HB25_PWM_ReadCounter(void) ;

    uint16 Left_HB25_PWM_ReadCapture(void) ;











    void Left_HB25_PWM_WriteCounter(uint16 counter)

            ;





void Left_HB25_PWM_WritePeriod(uint16 period)

        ;

uint16 Left_HB25_PWM_ReadPeriod(void) ;





    void Left_HB25_PWM_WriteCompare(uint16 compare)

            ;

    uint16 Left_HB25_PWM_ReadCompare(void) ;

void Left_HB25_PWM_Init(void) ;

void Left_HB25_PWM_Enable(void) ;

void Left_HB25_PWM_Sleep(void) ;

void Left_HB25_PWM_Wakeup(void) ;

void Left_HB25_PWM_SaveConfig(void) ;

void Left_HB25_PWM_RestoreConfig(void) ;
