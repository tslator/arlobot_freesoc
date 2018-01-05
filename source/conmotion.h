#ifndef CONMOTION_H
#define CONMOTION_H

#include "freesoc.h"
#include "concmd.h"


void ConMotion_Init(void);
void ConMotion_Start(void);

CONCMD_IF_TYPE * const ConMotion_InitCalLinear(FLOAT distance);
CONCMD_IF_TYPE * const ConMotion_InitMotionCalAngular(FLOAT angle);
CONCMD_IF_TYPE * const ConMotion_InitMotionCalUmbmark(void);

CONCMD_IF_TYPE * const ConMotion_InitMotionValLinear(FLOAT distance);
CONCMD_IF_TYPE * const ConMotion_InitMotionValAngular(FLOAT angle);
CONCMD_IF_TYPE * const ConMotion_InitMotionValSquare(BOOL left, FLOAT side);
CONCMD_IF_TYPE * const ConMotion_InitMotionValCircle(BOOL cw, FLOAT radius);
CONCMD_IF_TYPE * const ConMotion_InitMotionValOutAndBack(FLOAT distance);


#endif