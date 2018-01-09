#ifndef CONMOTOR_H
#define CONMOTOR_H
    
#include "freesoc.h"
#include "concmd.h"

void ConMotor_Init(void);
void ConMotor_Start(void);

CONCMD_IF_PTR_TYPE ConMotor_InitMotorShow(WHEEL_TYPE wheel, BOOL plain_text);
CONCMD_IF_PTR_TYPE ConMotor_InitMotorRepeat(
            WHEEL_TYPE wheel,
            FLOAT first,
            FLOAT second,
            FLOAT intvl,
            INT8 iters,
            BOOL no_pid,
            BOOL no_accel);
CONCMD_IF_PTR_TYPE ConMotor_InitMotorCal(
            WHEEL_TYPE wheel,
            INT8 iters);
CONCMD_IF_PTR_TYPE ConMotor_InitMotorVal(
            WHEEL_TYPE wheel,
            DIR_TYPE direction,
            FLOAT min_percent,
            FLOAT max_percent,
            INT8 num_points);
CONCMD_IF_PTR_TYPE ConMotor_InitMotorMove(
            FLOAT left_speed,
            FLOAT right_speed,
            FLOAT duration,
            BOOL no_pid,
            BOOL no_accel);

#endif
    