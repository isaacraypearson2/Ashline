#include "AshlineAppleNative.h"

#if !ASHLINE_APPLE_NATIVE

bool AshlineApple_ProbeMetalFXFramework(void)
{
	return false;
}

bool AshlineApple_HasGameController(void)
{
	return false;
}

void AshlineApple_PulseHaptics(float Intensity)
{
	(void)Intensity;
}

void AshlineApple_SetAdaptiveTriggers(int WeaponClass)
{
	(void)WeaponClass;
}

#endif
