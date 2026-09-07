#include "AshlineApple.h"

#if !ASHLINE_APPLE_NATIVE

bool AshlineApple_ProbeMetalFXFramework()
{
	return false;
}

extern "C" bool AshlineApple_HasGameController()
{
	return false;
}

extern "C" void AshlineApple_PulseHaptics(float Intensity)
{
	(void)Intensity;
}

extern "C" void AshlineApple_SetAdaptiveTriggers(int32 WeaponClass)
{
	(void)WeaponClass;
}

#endif
