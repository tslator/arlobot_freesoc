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
    
#include <project.h>
#include "config.h"
    
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

/*------------------------------------------------------------------------------------------------
 Description: Initializes the gain parameters and sample rate
 Parameters: None
 Return: None
 ------------------------------------------------------------------------------------------------*/
void Pid_Init(GET_TARGET_TYPE left_target, GET_TARGET_TYPE right_target);

/*------------------------------------------------------------------------------------------------
 Description: Starts the PID controller
 Parameters: None
 Return: None
 ------------------------------------------------------------------------------------------------*/
void Pid_Start();

/*------------------------------------------------------------------------------------------------
 Description: Updates the PID controller
 Parameters: None
 Return: None
 ------------------------------------------------------------------------------------------------*/
void Pid_Update();

float Pid_LeftStepInput(float *gains, float velocity, uint32 run_time);
float Pid_RightStepInput(float *gains, float velocity, uint32 run_time);

void Pid_SetLeftRightTarget(GET_TARGET_TYPE left_target, GET_TARGET_TYPE right_target);
#endif

/* [] END OF FILE */
