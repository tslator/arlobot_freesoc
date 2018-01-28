#ifndef MOTION_H
#define MOTION_H

#include "freesoc.h"

typedef enum {MOVE_FIRST=0, MOVE_LINEAR=MOVE_FIRST, MOVE_ROTATE, MOVE_CIRCLE, MOVE_PAUSE, MOVE_LAST=MOVE_PAUSE} MOVE_ENUM_TYPE;

typedef struct _tag_linear_move_data
{
    FLOAT distance;
    FLOAT speed;
} LINEAR_MOVE_DATA_TYPE;

typedef struct _tag_angular_move_data
{
    FLOAT angle;
    FLOAT speed;
} ANGULAR_MOVE_DATA_TYPE;

typedef struct _tag_pause_move_data
{
    FLOAT duration;
} PAUSE_MOVE_DATA_TYPE;

typedef struct _tag_circle_move_data
{
    FLOAT radius;
    FLOAT speed;
} CIRCLE_MOVE_DATA_TYPE;

void Motion_Init(void);
void Motion_AddMove(MOVE_ENUM_TYPE move, void *data);
void Motion_Start(void);
BOOL Motion_Update(void);
BOOL Motion_Status(void);
void Motion_Results(void);

#endif