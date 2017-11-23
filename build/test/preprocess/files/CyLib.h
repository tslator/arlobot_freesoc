#include "cyPm.h"
#include "cydevice_trm.h"
#include "cyfitter.h"
#include "cytypes.h"






extern uint8 CyResetStatus;

extern uint32 cydelay_freq_hz;

extern uint32 cydelay_freq_khz;

extern uint8 cydelay_freq_mhz;

extern uint32 cydelay_32k_ms;











cystatus CyPLL_OUT_Start(uint8 wait) ;

void CyPLL_OUT_Stop(void) ;

void CyPLL_OUT_SetPQ(uint8 pDiv, uint8 qDiv, uint8 current) ;

void CyPLL_OUT_SetSource(uint8 source) ;



void CyIMO_Start(uint8 wait) ;

void CyIMO_Stop(void) ;

void CyIMO_SetFreq(uint8 freq) ;

void CyIMO_SetSource(uint8 source) ;

void CyIMO_EnableDoubler(void) ;

void CyIMO_DisableDoubler(void) ;



void CyMasterClk_SetSource(uint8 source) ;

void CyMasterClk_SetDivider(uint8 divider) ;

void CyBusClk_SetDivider(uint16 divider) ;











void CyUsbClk_SetSource(uint8 source) ;



void CyILO_Start1K(void) ;

void CyILO_Stop1K(void) ;

void CyILO_Start100K(void) ;

void CyILO_Stop100K(void) ;

void CyILO_Enable33K(void) ;

void CyILO_Disable33K(void) ;

void CyILO_SetSource(uint8 source) ;

uint8 CyILO_SetPowerMode(uint8 mode) ;



uint8 CyXTAL_32KHZ_ReadStatus(void) ;

uint8 CyXTAL_32KHZ_SetPowerMode(uint8 mode) ;

void CyXTAL_32KHZ_Start(void) ;

void CyXTAL_32KHZ_Stop(void) ;



cystatus CyXTAL_Start(uint8 wait) ;

void CyXTAL_Stop(void) ;

void CyXTAL_SetStartup(uint8 setting) ;



void CyXTAL_EnableErrStatus(void) ;

void CyXTAL_DisableErrStatus(void) ;

uint8 CyXTAL_ReadStatus(void) ;

void CyXTAL_EnableFaultRecovery(void) ;

void CyXTAL_DisableFaultRecovery(void) ;



void CyXTAL_SetFbVoltage(uint8 setting) ;

void CyXTAL_SetWdVoltage(uint8 setting) ;



void CyWdtStart(uint8 ticks, uint8 lpMode) ;

void CyWdtClear(void) ;





void CyDelay(uint32 milliseconds) ;

void CyDelayUs(uint16 microseconds);

void CyDelayFreq(uint32 freq) ;

void CyDelayCycles(uint32 cycles);



void CySoftwareReset(void) ;



uint8 CyEnterCriticalSection(void);

void CyExitCriticalSection(uint8 savedIntrStatus);

void CyHalt(uint8 reason) ;









    cyisraddress CyIntSetSysVector(uint8 number, cyisraddress address) ;

    cyisraddress CyIntGetSysVector(uint8 number) ;





cyisraddress CyIntSetVector(uint8 number, cyisraddress address) ;

cyisraddress CyIntGetVector(uint8 number) ;



void CyIntSetPriority(uint8 number, uint8 priority) ;

uint8 CyIntGetPriority(uint8 number) ;



uint8 CyIntGetState(uint8 number) ;



uint32 CyDisableInts(void) ;

void CyEnableInts(uint32 mask) ;







    void CyFlushCache(void);









void CyVdLvDigitEnable(uint8 reset, uint8 threshold) ;

void CyVdLvAnalogEnable(uint8 reset, uint8 threshold) ;

void CyVdLvDigitDisable(void) ;

void CyVdLvAnalogDisable(void) ;

void CyVdHvAnalogEnable(void) ;

void CyVdHvAnalogDisable(void) ;

uint8 CyVdStickyStatus(uint8 mask) ;

uint8 CyVdRealTimeStatus(void) ;



void CySetScPumps(uint8 enable) ;







    void IntDefaultHandler (void);









    typedef void (*cySysTickCallback)(void);



    void CySysTickStart(void);

    void CySysTickInit(void);

    void CySysTickEnable(void);

    void CySysTickStop(void);

    void CySysTickEnableInterrupt(void);

    void CySysTickDisableInterrupt(void);

    void CySysTickSetReload(uint32 value);

    uint32 CySysTickGetReload(void);

    uint32 CySysTickGetValue(void);

    cySysTickCallback CySysTickSetCallback(uint32 number, cySysTickCallback function);

    cySysTickCallback CySysTickGetCallback(uint32 number);

    void CySysTickSetClockSource(uint32 clockSource);

    uint32 CySysTickGetCountFlag(void);

    void CySysTickClear(void);





void CyGetUniqueId(uint32* uniqueId);
