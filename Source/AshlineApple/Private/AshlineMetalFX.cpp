#include "AshlineMetalFX.h"
#include "AshlineMetalFXSubsystem.h"
#include "AshlineApple.h"
#include "AshlineAppleNative.h"
#include "Engine/Engine.h"
#include "HAL/IConsoleManager.h"
#include "RHI.h"

IAshlineMetalFX* IAshlineMetalFX::Get()
{
	if (GEngine)
	{
		return GEngine->GetEngineSubsystem<UAshlineMetalFXSubsystem>();
	}
	return nullptr;
}

void UAshlineMetalFXSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Probe();
}

void UAshlineMetalFXSubsystem::Probe()
{
	bMetalFX = false;
	bHardwareRT = false;

#if ASHLINE_APPLE_NATIVE
	bMetalFX = AshlineApple_ProbeMetalFXFramework();
	bHardwareRT = GRHISupportsRayTracing;

	Summary = TEXT("Metal RHI target. MetalFX ");
	Summary += bMetalFX ? TEXT("framework present. ") : TEXT("framework not present on this OS/GPU. ");
	if (bHardwareRT)
	{
		Summary += TEXT("Hardware ray tracing reported by RHI; still opt-in from graphics settings.");
	}
	else
	{
		Summary += TEXT("Hardware ray tracing not reported — RT settings stay off.");
	}
#else
	Summary = TEXT("Not an Apple target. MetalFX and Apple HW RT are unavailable.");
#endif

	UE_LOG(LogAshlineApple, Log, TEXT("%s"), *Summary);
}

bool UAshlineMetalFXSubsystem::IsMetalFXAvailable() const
{
	return bMetalFX;
}

bool UAshlineMetalFXSubsystem::IsHardwareRayTracingAvailable() const
{
	return bHardwareRT;
}

FString UAshlineMetalFXSubsystem::GetCapabilitySummary() const
{
	return Summary;
}

void UAshlineMetalFXSubsystem::ApplyUpscaler(int32 Mode)
{
	auto SetCVar = [](const TCHAR* Name, int32 Value)
	{
		if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
		{
			CVar->Set(*FString::FromInt(Value), ECVF_SetByCode);
		}
	};

	if (!bMetalFX || Mode <= 0)
	{
		SetCVar(TEXT("r.MetalFX.Enabled"), 0);
		return;
	}

	SetCVar(TEXT("r.MetalFX.Enabled"), 1);
	SetCVar(TEXT("r.MetalFX.Mode"), Mode);
	SetCVar(TEXT("r.MetalFX.UpscaleMode"), Mode);
}
