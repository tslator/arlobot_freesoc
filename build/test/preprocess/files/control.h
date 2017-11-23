#include "freesoc.h"
typedef void (*COMMAND_FUNC_TYPE)(FLOAT *linear, FLOAT *angular, UINT32 *timeout);









void Control_Init();

void Control_Start();

void Control_Update();

void Control_SetCommandVelocityFunc(COMMAND_FUNC_TYPE cmd);

void Control_RestoreCommandVelocityFunc();

FLOAT Control_LeftGetCmdVelocity();

FLOAT Control_RightGetCmdVelocity();

FLOAT Control_LinearGetCmdVelocity();

FLOAT Control_AngularGetCmdVelocity();

void Control_GetCmdVelocity(FLOAT *linear, FLOAT *angular);

void Control_SetCmdVelocity(FLOAT linear, FLOAT angular);

void Control_OverrideDebug(BOOL override);



void Control_SetDeviceStatusBit(UINT16 bit);

void Control_ClearDeviceStatusBit(UINT16 bit);



void Control_SetCalibrationStatus(UINT16 status);

void Control_SetCalibrationStatusBit(UINT16 bit);

void Control_ClearCalibrationStatusBit(UINT16 bit);



void Control_WriteOdom(FLOAT linear, FLOAT angular, FLOAT left_dist, FLOAT right_dist, FLOAT heading);

void Control_UpdateHeartbeat(UINT32 heartbeat);



void Control_SetLeftRightVelocityOverride(BOOL enable);

void Control_SetLeftRightVelocity(FLOAT left, FLOAT right);
