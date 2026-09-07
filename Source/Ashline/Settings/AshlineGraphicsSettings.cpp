#include "Settings/AshlineGraphicsSettings.h"

#include "Ashline.h"
#include "AshlineMetalFX.h"
#include "HAL/IConsoleManager.h"

void UAshlineGraphicsSettings::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UAshlineGraphicsSettings::ApplySavedOrDetect()
{
	ProbeAppleCapabilities();

	if (State.bMetalFXAvailable)
	{
		State.Upscaler = EAshlineUpscaler::MetalFXTemporal;
	}
	State.Preset = EAshlineGraphicsPreset::High;
	State.bRayTracingEnabled = false;
	ApplyCVars();
}

void UAshlineGraphicsSettings::ApplyPreset(EAshlineGraphicsPreset Preset)
{
	State.Preset = Preset;
	ApplyCVars();
}

void UAshlineGraphicsSettings::SetUpscaler(EAshlineUpscaler Upscaler)
{
	if (Upscaler != EAshlineUpscaler::Off && !State.bMetalFXAvailable)
	{
		UE_LOG(LogAshline, Warning, TEXT("MetalFX requested but not available on this GPU/OS. Leaving upscaler Off."));
		State.Upscaler = EAshlineUpscaler::Off;
		ApplyCVars();
		return;
	}
	State.Upscaler = Upscaler;
	ApplyCVars();
}

bool UAshlineGraphicsSettings::TryEnableRayTracing(bool bEnable)
{
	if (bEnable && !State.bHardwareRayTracingAvailable)
	{
		State.bRayTracingEnabled = false;
		State.CapabilityNotes = TEXT("Hardware ray tracing is capability-gated. Enable only on Apple Silicon GPUs that expose HW RT (typically M3+ with a Metal RT-capable OS). This machine reported no RT device.");
		UE_LOG(LogAshline, Warning, TEXT("%s"), *State.CapabilityNotes);
		ApplyCVars();
		return false;
	}
	State.bRayTracingEnabled = bEnable;
	ApplyCVars();
	return State.bRayTracingEnabled;
}

void UAshlineGraphicsSettings::ProbeAppleCapabilities()
{
	if (IAshlineMetalFX* MetalFX = IAshlineMetalFX::Get())
	{
		State.bMetalFXAvailable = MetalFX->IsMetalFXAvailable();
		State.bHardwareRayTracingAvailable = MetalFX->IsHardwareRayTracingAvailable();
		State.CapabilityNotes = MetalFX->GetCapabilitySummary();
	}
	else
	{
		State.bMetalFXAvailable = false;
		State.bHardwareRayTracingAvailable = false;
		State.CapabilityNotes = TEXT("AshlineApple module not loaded; MetalFX and HW RT remain off.");
	}
}

void UAshlineGraphicsSettings::ApplyCVars()
{
	auto SetCVar = [](const TCHAR* Name, int32 Value)
	{
		if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
		{
			CVar->Set(Value);
		}
	};

	int32 Scalability = 2;
	switch (State.Preset)
	{
	case EAshlineGraphicsPreset::Low: Scalability = 0; break;
	case EAshlineGraphicsPreset::Medium: Scalability = 1; break;
	case EAshlineGraphicsPreset::High: Scalability = 2; break;
	case EAshlineGraphicsPreset::Epic: Scalability = 3; break;
	case EAshlineGraphicsPreset::Cinematic: Scalability = 3; break;
	}

	SetCVar(TEXT("sg.ViewDistanceQuality"), Scalability);
	SetCVar(TEXT("sg.AntiAliasingQuality"), Scalability);
	SetCVar(TEXT("sg.ShadowQuality"), Scalability);
	SetCVar(TEXT("sg.GlobalIlluminationQuality"), Scalability);
	SetCVar(TEXT("sg.ReflectionQuality"), Scalability);
	SetCVar(TEXT("sg.PostProcessQuality"), Scalability);
	SetCVar(TEXT("sg.TextureQuality"), Scalability);
	SetCVar(TEXT("sg.EffectsQuality"), Scalability);
	SetCVar(TEXT("sg.FoliageQuality"), Scalability);
	SetCVar(TEXT("sg.ShadingQuality"), Scalability);

	SetCVar(TEXT("r.RayTracing"), State.bRayTracingEnabled ? 1 : 0);
	SetCVar(TEXT("r.Lumen.HardwareRayTracing"), State.bRayTracingEnabled ? 1 : 0);
	SetCVar(TEXT("r.Lumen.Reflections.HardwareRayTracing"), State.bRayTracingEnabled ? 1 : 0);

	if (IAshlineMetalFX* MetalFX = IAshlineMetalFX::Get())
	{
		MetalFX->ApplyUpscaler(State.Upscaler == EAshlineUpscaler::MetalFXTemporal
			? 2
			: (State.Upscaler == EAshlineUpscaler::MetalFXSpatial ? 1 : 0));
	}

	UE_LOG(LogAshline, Log, TEXT("Graphics applied. Preset=%d MetalFX=%d HW-RT=%d RTOn=%d"),
		static_cast<int32>(State.Preset),
		State.bMetalFXAvailable ? 1 : 0,
		State.bHardwareRayTracingAvailable ? 1 : 0,
		State.bRayTracingEnabled ? 1 : 0);
}
