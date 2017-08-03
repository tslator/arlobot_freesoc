/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef PID_H
#define PID_H
    
/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <project.h>
#include "config.h"
#include "pid_controller.h"
    
/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/    
#define DUMP_PID(enable, pid, pwm)  do { \
                                        if (Debug_IsEnabled(enable)) DumpPid(pid.name, &pid.pid, pwm); \
                                    } while (0)
    
    
#ifdef ALTERNATE_PID_FORMULAE

/* Note: The following parameters were empirically derived and are a compromise between forward and backward performance.
   Unexpectedly, it was discovered the performance of the motor in the forward direction is dramatically better than the
   performance of the backward direction.  As a result, it was necessary to use the worst-case parameters (the backward
   case) for both.
        
   The forward performance is much more responsive and has a better rise time, but the same parameters applied in the 
   backward direction caused oscillation.  Too bad.
 */
        
//Classic Ziegler-Nichols   Kp = 0.6 Ku     Ti = 0.5 Tu     Td = 0.125 Tu
//Pessen Integral Rule  Kp = 0.7 Ku     Ti = 0.4 Tu     Td = 0.15 Tu
//Some Overshoot    Kp = 0.33 Ku    Ti = 0.5 Tu     Td = 0.33 Tu
//No Overshoot  Kp = 0.2 Ku     Ti = 0.5 Tu     Td = 0.33 Tu 
    
//loop gain Kp = 0.6 * Ku = 6.7
//integral time constant Ti = 0.5 * Tu = 0.033
//derivative time constant Td = 0.125 * Tu = .0083

//Set proportional gain (Kp) to Kc.
//Set integral gain (Ki) to Kc/Ti, or for integral time (Ti) use Ti/Kc.
//Set derivative gain (Kd) to Kc x Td.

#define Kc (6.5)
#define Pc (0.850)

#define znKp(kc)        (kc*0.6)
#define znKi(kc, pc)    (2*znKp(kc)/pc)
#define znKd(kp, pc)    ((kp*pc)/8)
    
#define zn2Kp(ku)        (0.6*ku)
#define zn2Ti(tu)        (0.5*tu)
#define zn2Ki(kc, tu)    (kc/zn2Ti(tu))
#define zn2Td(tu)        (0.125*tu)
#define zn2Kd(kc, tu)    (kc*zn2Td(tu))

#define pirKp(ku)       (0.7*ku)
#define pirTi(tu)       (0.4*tu)
#define pirKi(kc, tu)   (kc/pirTi(tu))
#define pirTd(tu)       (0.15*tu)
#define pirKd(kc, tu)   (kc*pirTd(tu))

#define soKp(ku)        (0.33*ku)
#define soTi(tu)        (0.5*tu)
#define soKi(kc, tu)    (kc/soTi(tu))
#define soTd(tu)        (0.33*tu)
#define soKd(kc, tu)    (kc*soTd(tu))

#define noKp(ku)        (0.2*ku)
#define noTi(tu)        (0.5*tu)
#define noKi(kc, tu)    (kc/noTi(tu))
#define noTd(tu)        (0.33*tu)
#define noKd(kc, tu)    (kc*noTd(tu))
    
    
#endif

#define PID_SAMPLE_TIME_MS  SAMPLE_TIME_MS(PID_SAMPLE_RATE)
#define PID_SAMPLE_TIME_SEC SAMPLE_TIME_SEC(PID_SAMPLE_RATE)

/*---------------------------------------------------------------------------------------------------
 * Types
 *-------------------------------------------------------------------------------------------------*/
typedef float (*GET_TARGET_FUNC_TYPE)();
typedef float (*GET_INPUT_FUNC_TYPE)();
typedef void (*PID_UPDATE_TYPE)(float target, float input);

typedef struct _pid_tag
{
    char name[6];
    PIDControl pid;
    int sign;
    GET_TARGET_FUNC_TYPE get_target;
    GET_INPUT_FUNC_TYPE get_input;
    PID_UPDATE_TYPE update;
} PID_TYPE;


/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Pid_Init();
void Pid_Start();
void Pid_Update();
void Pid_SetLeftRightTarget(GET_TARGET_FUNC_TYPE left_target, GET_TARGET_FUNC_TYPE right_target);
void Pid_RestoreLeftRightTarget();
void Pid_Reset();
void Pid_Enable(uint8 enable);

#endif

/* [] END OF FILE */
