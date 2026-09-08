#include "Settings/AshlineGraphicsSettings.h"

#include "Ashline.h"
#include "AshlineMetalFX.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMisc.h"
#include "RHI.h"
#include "DynamicRHI.h"
#include "Settings/AshlineGameUserSettings.h"

void UAshlineGraphicsSettings::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RegisterConsoleCommands();
}

void UAshlineGraphicsSettings::Deinitialize()
{
	UnregisterConsoleCommands();
	Super::Deinitialize();
}

void UAshlineGraphicsSettings::ApplySavedOrDetect()
{
	ProbeCapabilities();

#if PLATFORM_WINDOWS
	if (DetectSteamDeck())
	{
		State.Preset = EAshlineGraphicsPreset::SteamDeck;
		State.Upscaler = State.bFSR3Available ? EAshlineUpscaler::FSR3 : EAshlineUpscaler::TSR;
		State.bRayTracingEnabled = false;
	}
	else
	{
		State.Preset = EAshlineGraphicsPreset::PC_Ultra;
		if (State.bFSR3Available)
		{
			State.Upscaler = EAshlineUpscaler::FSR3;
		}
		else
		{
			State.Upscaler = EAshlineUpscaler::TSR;
		}
		State.bRayTracingEnabled = State.bHardwareRayTracingAvailable;
	}
#elif PLATFORM_MAC || PLATFORM_IOS || PLATFORM_TVOS
	if (State.bMetalFXAvailable)
	{
		State.Upscaler = EAshlineUpscaler::MetalFXTemporal;
	}
	else
	{
		State.Upscaler = EAshlineUpscaler::TSR;
	}
	State.Preset = EAshlineGraphicsPreset::High;
	State.bRayTracingEnabled = false;
#else
	State.Preset = EAshlineGraphicsPreset::High;
	State.Upscaler = EAshlineUpscaler::TSR;
	State.bRayTracingEnabled = State.bHardwareRayTracingAvailable;
#endif

	ApplyCVars();
}

void UAshlineGraphicsSettings::ApplyPreset(EAshlineGraphicsPreset Preset)
{
	State.Preset = Preset;
	if (Preset == EAshlineGraphicsPreset::PC_Ultra || Preset == EAshlineGraphicsPreset::PC_Balanced)
	{
		if (State.bHardwareRayTracingAvailable)
		{
			State.bRayTracingEnabled = true;
		}
		if (State.Upscaler == EAshlineUpscaler::Off || State.Upscaler == EAshlineUpscaler::MetalFXSpatial || State.Upscaler == EAshlineUpscaler::MetalFXTemporal)
		{
			State.Upscaler = State.bFSR3Available ? EAshlineUpscaler::FSR3 : EAshlineUpscaler::TSR;
		}
	}
	else if (Preset == EAshlineGraphicsPreset::SteamDeck)
	{
		State.bRayTracingEnabled = false;
		State.Upscaler = State.bFSR3Available ? EAshlineUpscaler::FSR3 : EAshlineUpscaler::TSR;
		if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
		{
			User->Feel.bForceHandheldHUD = true;
			User->Feel.HUDScale = FMath::Max(User->Feel.HUDScale, 1.2f);
			User->Feel.SafeZone = FMath::Max(User->Feel.SafeZone, 0.08f);
		}
	}
	ApplyCVars();
}

void UAshlineGraphicsSettings::SetUpscaler(EAshlineUpscaler Upscaler)
{
	if (Upscaler == EAshlineUpscaler::MetalFXSpatial || Upscaler == EAshlineUpscaler::MetalFXTemporal)
	{
		if (!State.bMetalFXAvailable)
		{
			UE_LOG(LogAshline, Warning, TEXT("MetalFX requested but not available. Falling back to TSR."));
			State.Upscaler = EAshlineUpscaler::TSR;
			ApplyCVars();
			return;
		}
	}
	if (Upscaler == EAshlineUpscaler::FSR3 && !State.bFSR3Available)
	{
		UE_LOG(LogAshline, Warning, TEXT("FSR3 CVars not present — using TSR. Enable the AMD FSR3 plugin in the editor if desired."));
		State.Upscaler = EAshlineUpscaler::TSR;
		ApplyCVars();
		return;
	}
	if (Upscaler == EAshlineUpscaler::DLSS && !State.bDLSSAvailable)
	{
		UE_LOG(LogAshline, Warning, TEXT("DLSS requested but plugin/CVars are missing. Leaving TSR/FSR path."));
		State.Upscaler = State.bFSR3Available ? EAshlineUpscaler::FSR3 : EAshlineUpscaler::TSR;
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
		State.CapabilityNotes = TEXT("Hardware ray tracing is capability-gated (GRHISupportsRayTracing). This GPU/RHI did not report an RT device.");
		UE_LOG(LogAshline, Warning, TEXT("%s"), *State.CapabilityNotes);
		ApplyCVars();
		return false;
	}
	State.bRayTracingEnabled = bEnable;
	ApplyCVars();
	return State.bRayTracingEnabled;
}

void UAshlineGraphicsSettings::SetFrameGeneration(bool bEnable)
{
	bFrameGeneration = bEnable;
	ApplyCVars();
}

FString UAshlineGraphicsSettings::GetPresetDisplayName(EAshlineGraphicsPreset Preset)
{
	switch (Preset)
	{
	case EAshlineGraphicsPreset::Low: return TEXT("Low");
	case EAshlineGraphicsPreset::Medium: return TEXT("Medium");
	case EAshlineGraphicsPreset::High: return TEXT("High");
	case EAshlineGraphicsPreset::Epic: return TEXT("Epic");
	case EAshlineGraphicsPreset::Cinematic: return TEXT("Cinematic");
	case EAshlineGraphicsPreset::PC_Balanced: return TEXT("Ashline_PC_Balanced");
	case EAshlineGraphicsPreset::PC_Ultra: return TEXT("Ashline_PC_Ultra");
	case EAshlineGraphicsPreset::SteamDeck: return TEXT("Ashline_SteamDeck");
	default: return TEXT("Unknown");
	}
}

FString UAshlineGraphicsSettings::DescribeTargetHardware()
{
	return TEXT("Target PC: AMD Ryzen 5 7500X3D, 32 GB DDR5-6000, Radeon RX 9070 GRE (RDNA4). 1440p Ultra / high-refresh. DX12 + SM6 + Nanite + Lumen + VSM. Upscale: FSR 3 (TSR fallback). DLSS optional.");
}

void UAshlineGraphicsSettings::ProbeCapabilities()
{
	State.bWindows = false;
	State.bDX12 = false;
	State.bMetalFXAvailable = false;
	State.bFSR3Available = HasCVar(TEXT("r.FidelityFX.FSR3.Enabled")) || HasCVar(TEXT("r.FidelityFX.FSR.Enabled")) || HasCVar(TEXT("r.FX.FSR3.Enabled"));
	State.bDLSSAvailable = HasCVar(TEXT("r.NGX.DLSS.Enable")) || HasCVar(TEXT("r.Streamline.DLSS.Enable"));
	State.bHardwareRayTracingAvailable = GRHISupportsRayTracing;
	State.RHIName = GDynamicRHI ? FString(GDynamicRHI->GetName()) : FString(TEXT("Unknown"));
	State.bDX12 = State.RHIName.Contains(TEXT("D3D12")) || State.RHIName.Contains(TEXT("DirectX 12")) || State.RHIName.Contains(TEXT("Direct3D12"));

#if PLATFORM_WINDOWS
	State.bWindows = true;
#endif

	if (IAshlineMetalFX* MetalFX = IAshlineMetalFX::Get())
	{
		State.bMetalFXAvailable = MetalFX->IsMetalFXAvailable();
#if PLATFORM_MAC || PLATFORM_IOS || PLATFORM_TVOS
		State.bHardwareRayTracingAvailable = MetalFX->IsHardwareRayTracingAvailable();
		State.CapabilityNotes = MetalFX->GetCapabilitySummary();
#endif
	}

	if (State.bWindows)
	{
		State.CapabilityNotes = FString::Printf(
			TEXT("Windows-first. RHI=%s DX12=%d HW-RT=%d FSR3CVars=%d DLSS=%d. %s"),
			*State.RHIName,
			State.bDX12 ? 1 : 0,
			State.bHardwareRayTracingAvailable ? 1 : 0,
			State.bFSR3Available ? 1 : 0,
			State.bDLSSAvailable ? 1 : 0,
			*DescribeTargetHardware());
	}
	else if (State.CapabilityNotes.IsEmpty())
	{
		State.CapabilityNotes = FString::Printf(TEXT("RHI=%s HW-RT=%d MetalFX=%d"), *State.RHIName,
			State.bHardwareRayTracingAvailable ? 1 : 0, State.bMetalFXAvailable ? 1 : 0);
	}

	UE_LOG(LogAshline, Log, TEXT("Graphics probe: %s"), *State.CapabilityNotes);
}

void UAshlineGraphicsSettings::ApplyCVars()
{
	int32 Scalability = 2;
	switch (State.Preset)
	{
	case EAshlineGraphicsPreset::Low: Scalability = 0; break;
	case EAshlineGraphicsPreset::Medium: Scalability = 1; break;
	case EAshlineGraphicsPreset::High: Scalability = 2; break;
	case EAshlineGraphicsPreset::Epic: Scalability = 3; break;
	case EAshlineGraphicsPreset::Cinematic: Scalability = 3; break;
	case EAshlineGraphicsPreset::PC_Balanced: Scalability = 3; break;
	case EAshlineGraphicsPreset::PC_Ultra: Scalability = 3; break;
	case EAshlineGraphicsPreset::SteamDeck: Scalability = 2; break;
	}

	SetCVarInt(TEXT("sg.ViewDistanceQuality"), Scalability);
	SetCVarInt(TEXT("sg.AntiAliasingQuality"), Scalability);
	SetCVarInt(TEXT("sg.ShadowQuality"), Scalability);
	SetCVarInt(TEXT("sg.GlobalIlluminationQuality"), Scalability);
	SetCVarInt(TEXT("sg.ReflectionQuality"), Scalability);
	SetCVarInt(TEXT("sg.PostProcessQuality"), Scalability);
	SetCVarInt(TEXT("sg.TextureQuality"), Scalability);
	SetCVarInt(TEXT("sg.EffectsQuality"), Scalability);
	SetCVarInt(TEXT("sg.FoliageQuality"), Scalability);
	SetCVarInt(TEXT("sg.ShadingQuality"), Scalability);
	SetCVarInt(TEXT("sg.LandscapeQuality"), Scalability);
	SetCVarInt(TEXT("sg.ShadingQuality"), Scalability);

	SetCVarInt(TEXT("r.Nanite"), 1);
	SetCVarInt(TEXT("r.Nanite.ProjectEnabled"), 1);
	SetCVarInt(TEXT("r.Shadow.Virtual.Enable"), 1);
	SetCVarInt(TEXT("r.DynamicGlobalIlluminationMethod"), 1);
	SetCVarInt(TEXT("r.ReflectionMethod"), 1);
	SetCVarInt(TEXT("r.Lumen.DiffuseIndirect.Allow"), 1);
	SetCVarInt(TEXT("r.Lumen.Reflections.Allow"), 1);
	SetCVarInt(TEXT("r.DefaultFeature.AntiAliasing"), 4);

	if (State.Preset == EAshlineGraphicsPreset::PC_Ultra || State.Preset == EAshlineGraphicsPreset::PC_Balanced)
	{
		ApplyNamedPCPreset(State.Preset);
	}
	else if (State.Preset == EAshlineGraphicsPreset::SteamDeck)
	{
		ApplyHandheldPreset();
	}
	else
	{
		SetCVarInt(TEXT("r.VSync"), 1);
		SetCVarFloat(TEXT("r.ScreenPercentage"), State.Preset == EAshlineGraphicsPreset::Low ? 67.f : 100.f);
		SetCVarInt(TEXT("r.Streaming.PoolSize"), Scalability >= 3 ? 3000 : 1800);
	}

	ApplyRayTracingCVars();
	ApplyUpscalerCVars();

	if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
	{
		User->NamedPreset = State.Preset;
		User->PreferredUpscaler = State.Upscaler;
	}

	UE_LOG(LogAshline, Log, TEXT("Graphics applied. Preset=%s Upscaler=%d RT=%d FSR3=%d DX12=%d"),
		*GetPresetDisplayName(State.Preset),
		static_cast<int32>(State.Upscaler),
		State.bRayTracingEnabled ? 1 : 0,
		State.bFSR3Available ? 1 : 0,
		State.bDX12 ? 1 : 0);
}

void UAshlineGraphicsSettings::ApplyNamedPCPreset(EAshlineGraphicsPreset Preset)
{
	const bool bUltra = Preset == EAshlineGraphicsPreset::PC_Ultra;

	// 9070 GRE / 32 GB DDR5 / 1440p high-refresh.
	SetCVarInt(TEXT("r.VSync"), 0);
	SetCVarFloat(TEXT("t.MaxFPS"), 0.f);
	SetCVarInt(TEXT("r.FinishCurrentFrame"), 0);
	SetCVarInt(TEXT("r.MaxAnisotropy"), 16);
	SetCVarInt(TEXT("r.VT.MaxAnisotropy"), 8);
	SetCVarInt(TEXT("r.Streaming.PoolSize"), bUltra ? 5600 : 3800);
	SetCVarInt(TEXT("r.Streaming.LimitPoolSizeToVRAM"), 1);
	SetCVarFloat(TEXT("r.ViewDistanceScale"), bUltra ? 1.15f : 0.9f);
	SetCVarFloat(TEXT("r.Shadow.DistanceScale"), bUltra ? 1.1f : 0.85f);
	SetCVarInt(TEXT("r.Shadow.Virtual.MaxQuality"), bUltra ? 3 : 2);
	SetCVarInt(TEXT("r.Shadow.Virtual.SMRT.RayCountDirectional"), bUltra ? 8 : 4);
	SetCVarInt(TEXT("r.Lumen.ScreenProbeGather.RadianceCache.ProbeResolution"), bUltra ? 32 : 16);
	SetCVarInt(TEXT("r.Lumen.ScreenProbeGather.DownsampleFactor"), bUltra ? 16 : 32);
	SetCVarInt(TEXT("r.Lumen.Reflections.DownsampleFactor"), bUltra ? 1 : 2);
	SetCVarInt(TEXT("r.Lumen.TraceMeshSDFs"), 1);
	SetCVarInt(TEXT("r.AmbientOcclusionLevels"), bUltra ? 2 : 1);
	SetCVarInt(TEXT("r.BloomQuality"), bUltra ? 5 : 4);
	SetCVarInt(TEXT("r.MotionBlurQuality"), 0);
	SetCVarInt(TEXT("r.DepthOfFieldQuality"), bUltra ? 2 : 0);
	SetCVarFloat(TEXT("foliage.DensityScale"), bUltra ? 1.f : 0.7f);
	SetCVarFloat(TEXT("r.Tonemapper.Sharpen"), 0.45f);

	if (State.Upscaler == EAshlineUpscaler::FSR3)
	{
		SetCVarFloat(TEXT("r.ScreenPercentage"), bUltra ? 77.f : 59.f);
	}
	else if (State.Upscaler == EAshlineUpscaler::TSR)
	{
		SetCVarFloat(TEXT("r.ScreenPercentage"), bUltra ? 85.f : 70.f);
	}
	else if (State.Upscaler == EAshlineUpscaler::DLSS)
	{
		SetCVarFloat(TEXT("r.ScreenPercentage"), bUltra ? 67.f : 50.f);
	}
	else
	{
		SetCVarFloat(TEXT("r.ScreenPercentage"), bUltra ? 100.f : 80.f);
	}
}

void UAshlineGraphicsSettings::ApplyUpscalerCVars()
{
	// Reset optional scalers; TSR remains the engine default AA path.
	SetCVarInt(TEXT("r.MetalFX.Enabled"), 0);
	if (HasCVar(TEXT("r.FidelityFX.FSR3.Enabled")))
	{
		SetCVarInt(TEXT("r.FidelityFX.FSR3.Enabled"), 0);
	}
	if (HasCVar(TEXT("r.FidelityFX.FI.Enabled")))
	{
		SetCVarInt(TEXT("r.FidelityFX.FI.Enabled"), 0);
	}
	if (HasCVar(TEXT("r.NGX.DLSS.Enable")))
	{
		SetCVarInt(TEXT("r.NGX.DLSS.Enable"), 0);
	}

	SetCVarInt(TEXT("r.AntiAliasingMethod"), 4);
	SetCVarInt(TEXT("r.TemporalAA.Upsampling"), 1);
	SetCVarInt(TEXT("r.TSR.History.ScreenPercentage"), 100);

	switch (State.Upscaler)
	{
	case EAshlineUpscaler::MetalFXSpatial:
	case EAshlineUpscaler::MetalFXTemporal:
		if (IAshlineMetalFX* MetalFX = IAshlineMetalFX::Get())
		{
			MetalFX->ApplyUpscaler(State.Upscaler == EAshlineUpscaler::MetalFXTemporal ? 2 : 1);
		}
		break;
	case EAshlineUpscaler::FSR3:
		SetCVarInt(TEXT("r.FidelityFX.FSR3.Enabled"), 1);
		SetCVarInt(TEXT("r.FidelityFX.FSR.Enabled"), 1);
		const int32 FSRQuality = (State.Preset == EAshlineGraphicsPreset::SteamDeck) ? 3
			: (State.Preset == EAshlineGraphicsPreset::PC_Balanced ? 2 : 1);
		SetCVarInt(TEXT("r.FidelityFX.FSR3.QualityMode"), FSRQuality);
		SetCVarInt(TEXT("r.FidelityFX.FI.Enabled"), bFrameGeneration ? 1 : 0);
		break;
	case EAshlineUpscaler::DLSS:
		SetCVarInt(TEXT("r.NGX.DLSS.Enable"), 1);
		SetCVarInt(TEXT("r.NGX.DLSS.Quality"), State.Preset == EAshlineGraphicsPreset::PC_Balanced ? 2 : 1);
		break;
	case EAshlineUpscaler::TSR:
	case EAshlineUpscaler::Off:
	default:
		break;
	}
}

void UAshlineGraphicsSettings::ApplyRayTracingCVars()
{
	const int32 On = State.bRayTracingEnabled ? 1 : 0;
	SetCVarInt(TEXT("r.RayTracing"), On);
	SetCVarInt(TEXT("r.Lumen.HardwareRayTracing"), On);
	SetCVarInt(TEXT("r.Lumen.Reflections.HardwareRayTracing"), On);
	SetCVarInt(TEXT("r.Lumen.HardwareRayTracing.LightingMode"), On ? 2 : 0);
	if (On)
	{
		SetCVarInt(TEXT("r.RayTracing.Shadows"), (State.Preset == EAshlineGraphicsPreset::PC_Ultra) ? 1 : 0);
		SetCVarInt(TEXT("r.RayTracing.Skylight"), 1);
	}
}

void UAshlineGraphicsSettings::SetCVarInt(const TCHAR* Name, int32 Value)
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
	{
		CVar->Set(*FString::FromInt(Value), ECVF_SetByCode);
	}
}

void UAshlineGraphicsSettings::SetCVarFloat(const TCHAR* Name, float Value)
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
	{
		CVar->Set(*FString::SanitizeFloat(Value), ECVF_SetByCode);
	}
}

bool UAshlineGraphicsSettings::HasCVar(const TCHAR* Name)
{
	return IConsoleManager::Get().FindConsoleVariable(Name) != nullptr;
}

void UAshlineGraphicsSettings::ApplyUltraPreset()
{
	ApplyPreset(EAshlineGraphicsPreset::PC_Ultra);
}

void UAshlineGraphicsSettings::ApplyBalancedPreset()
{
	ApplyPreset(EAshlineGraphicsPreset::PC_Balanced);
}

void UAshlineGraphicsSettings::ApplySteamDeckPreset()
{
	ApplyPreset(EAshlineGraphicsPreset::SteamDeck);
}

void UAshlineGraphicsSettings::ApplyHandheldPreset()
{
	State.bRayTracingEnabled = false;
	SetCVarInt(TEXT("r.VSync"), 1);
	SetCVarFloat(TEXT("t.MaxFPS"), 60.f);
	SetCVarInt(TEXT("r.FinishCurrentFrame"), 0);
	SetCVarInt(TEXT("r.MaxAnisotropy"), 8);
	SetCVarInt(TEXT("r.Streaming.PoolSize"), 1800);
	SetCVarInt(TEXT("r.Streaming.LimitPoolSizeToVRAM"), 1);
	SetCVarFloat(TEXT("r.ViewDistanceScale"), 0.8f);
	SetCVarFloat(TEXT("r.Shadow.DistanceScale"), 0.7f);
	SetCVarInt(TEXT("r.Shadow.Virtual.MaxQuality"), 1);
	SetCVarInt(TEXT("r.Shadow.Virtual.SMRT.RayCountDirectional"), 4);
	SetCVarInt(TEXT("r.Lumen.ScreenProbeGather.RadianceCache.ProbeResolution"), 16);
	SetCVarInt(TEXT("r.Lumen.ScreenProbeGather.DownsampleFactor"), 32);
	SetCVarInt(TEXT("r.Lumen.Reflections.DownsampleFactor"), 2);
	SetCVarInt(TEXT("r.Lumen.TraceMeshSDFs"), 0);
	SetCVarInt(TEXT("r.AmbientOcclusionLevels"), 1);
	SetCVarInt(TEXT("r.BloomQuality"), 3);
	SetCVarInt(TEXT("r.MotionBlurQuality"), 0);
	SetCVarInt(TEXT("r.DepthOfFieldQuality"), 0);
	SetCVarFloat(TEXT("foliage.DensityScale"), 0.5f);
	SetCVarFloat(TEXT("r.Tonemapper.Sharpen"), 0.35f);
	SetCVarInt(TEXT("r.RayTracing"), 0);
	SetCVarInt(TEXT("r.Lumen.HardwareRayTracing"), 0);
	if (State.Upscaler == EAshlineUpscaler::FSR3)
	{
		SetCVarFloat(TEXT("r.ScreenPercentage"), 67.f);
	}
	else
	{
		SetCVarFloat(TEXT("r.ScreenPercentage"), 77.f);
	}
}

bool UAshlineGraphicsSettings::DetectSteamDeck()
{
	return UAshlineGameUserSettings::IsSteamDeckHardware();
}

void UAshlineGraphicsSettings::RegisterConsoleCommands()
{
	IConsoleManager& CM = IConsoleManager::Get();
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshPCUltra"),
		TEXT("Apply Ashline_PC_Ultra (1440p / 9070 GRE class)."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplyUltraPreset),
		ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshPCBalanced"),
		TEXT("Apply Ashline_PC_Balanced."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplyBalancedPreset),
		ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshDeck"),
		TEXT("Apply Ashline_SteamDeck (1280x800 handheld HUD + 60 fps path)."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplySteamDeckPreset),
		ECVF_Default));
}

void UAshlineGraphicsSettings::UnregisterConsoleCommands()
{
	for (IConsoleObject* Obj : ConsoleObjects)
	{
		if (Obj)
		{
			IConsoleManager::Get().UnregisterConsoleObject(Obj);
		}
	}
	ConsoleObjects.Reset();
}
