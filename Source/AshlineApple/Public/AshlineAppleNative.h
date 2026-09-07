#pragma once

#include "CoreMinimal.h"

bool AshlineApple_ProbeMetalFXFramework();

extern "C" bool AshlineApple_HasGameController();
extern "C" void AshlineApple_PulseHaptics(float Intensity);
extern "C" void AshlineApple_SetAdaptiveTriggers(int32 WeaponClass);
