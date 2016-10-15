#ifndef PID_AutoTune_v0
#define PID_AutoTune_v0
#define LIBRARY_VERSION	0.0.1

void PIDATune_Init();
int PIDATune_Runtime();
void PIDATune_Cancel();
void PIDATune_SetOutputStep(float output);
float PIDATune_GetOutputStep();
void PIDATune_SetControlType(int type);
int PIDATune_GetControlType();
void PIDATune_SetLookbackSec(int lookback);
int PIDATune_GetLookbackSec();
void PIDATune_SetNoiseBand(float band);
float PIDATune_GetNoiseBand();
float PIDATune_GetKp();
float PIDATune_GetKi();
float PIDATune_GetKd();

#endif