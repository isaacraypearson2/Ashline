#include "AshlineDualSense.h"
#include "AshlineApple.h"
#include "AshlineAppleNative.h"

#if ASHLINE_APPLE_NATIVE
#include "GenericPlatform/GenericApplication.h"
#endif

void UAshlineDualSense::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RefreshControllers();
}

void UAshlineDualSense::RefreshControllers()
{
	bConnected = false;
	ControllerSummary = TEXT("No DualSense via Game Controller framework.");

#if ASHLINE_APPLE_NATIVE
	bConnected = AshlineApple_HasGameController();
	ControllerSummary = bConnected
		? TEXT("DualSense / Game Controller connected. Adaptive triggers and haptics are live.")
		: TEXT("Apple Game Controller framework linked. Pair a DualSense to enable adaptive triggers.");
#else
	ControllerSummary = TEXT("Non-Apple target: DualSense hooks compile as no-ops.");
#endif

	UE_LOG(LogAshlineApple, Log, TEXT("%s"), *ControllerSummary);
}

bool UAshlineDualSense::IsDualSenseConnected() const
{
	return bConnected;
}

void UAshlineDualSense::TriggerWeaponFeedback(uint8 WeaponClass, float RecoilPitch)
{
	const float Intensity = FMath::Clamp(RecoilPitch / 4.f, 0.15f, 1.f);
	Pulse(EAshlineDualSenseEvent::Fire, Intensity);
	SetAdaptiveTriggersForWeapon(WeaponClass);
}

void UAshlineDualSense::Pulse(EAshlineDualSenseEvent Event, float Intensity)
{
#if ASHLINE_APPLE_NATIVE
	AshlineApple_PulseHaptics(Intensity);
	UE_LOG(LogAshlineApple, Verbose, TEXT("DualSense pulse event=%d intensity=%.2f"), static_cast<int32>(Event), Intensity);
#else
	(void)Event;
	(void)Intensity;
#endif
}

void UAshlineDualSense::SetAdaptiveTriggersForWeapon(uint8 WeaponClass)
{
#if ASHLINE_APPLE_NATIVE
	AshlineApple_SetAdaptiveTriggers(static_cast<int>(WeaponClass));
#else
	(void)WeaponClass;
#endif
}
