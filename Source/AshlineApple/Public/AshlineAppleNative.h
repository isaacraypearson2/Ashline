#pragma once

#ifdef __cplusplus
extern "C" {
#endif

bool AshlineApple_ProbeMetalFXFramework(void);
bool AshlineApple_HasGameController(void);
void AshlineApple_PulseHaptics(float Intensity);
void AshlineApple_SetAdaptiveTriggers(int WeaponClass);

#ifdef __cplusplus
}
#endif
