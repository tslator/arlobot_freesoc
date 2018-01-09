#ifndef CONMOTION_H
#define CONMOTION_H

#include "freesoc.h"
#include "concmd.h"


void ConMotion_Init(void);
void ConMotion_Start(void);

CONCMD_IF_PTR_TYPE ConMotion_InitCalLinear(FLOAT distance);
CONCMD_IF_PTR_TYPE ConMotion_InitMotionCalAngular(FLOAT angle);
CONCMD_IF_PTR_TYPE ConMotion_InitMotionCalUmbmark(void);

CONCMD_IF_PTR_TYPE ConMotion_InitMotionValLinear(FLOAT distance);
CONCMD_IF_PTR_TYPE ConMotion_InitMotionValAngular(FLOAT angle);
CONCMD_IF_PTR_TYPE ConMotion_InitMotionValSquare(BOOL left, FLOAT side);
CONCMD_IF_PTR_TYPE ConMotion_InitMotionValCircle(BOOL cw, FLOAT radius);
CONCMD_IF_PTR_TYPE ConMotion_InitMotionValOutAndBack(FLOAT distance);


#endif