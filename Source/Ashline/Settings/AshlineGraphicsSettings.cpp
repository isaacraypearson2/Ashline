#include "Settings/AshlineGraphicsSettings.h"

#include "Ashline.h"
#include "AshlineMetalFX.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMisc.h"
#include "GenericPlatform/GenericPlatformMemory.h"
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
	State.RecommendedPreset = DetectRecommendedPreset();

	bool bUseSaved = false;
	if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
	{
		State.bAutoDetect = User->bAutoDetectPreset;
		if (!User->bAutoDetectPreset)
		{
			State.Preset = User->NamedPreset;
			State.Upscaler = User->PreferredUpscaler;
			State.FrameTarget = User->FrameTarget;
			bUseSaved = true;
		}
	}

	if (!bUseSaved)
	{
		State.Preset = State.RecommendedPreset;
#if PLATFORM_WINDOWS
		if (State.bFSR3Available)
		{
			State.Upscaler = EAshlineUpscaler::FSR3;
		}
		else
		{
			State.Upscaler = EAshlineUpscaler::TSR;
		}
		State.bRayTracingEnabled = State.bHardwareRayTracingAvailable
			&& (State.Preset == EAshlineGraphicsPreset::PC_Ultra || State.Preset == EAshlineGraphicsPreset::PC_High);
#elif PLATFORM_MAC || PLATFORM_IOS || PLATFORM_TVOS
		State.Upscaler = State.bMetalFXAvailable ? EAshlineUpscaler::MetalFXTemporal : EAshlineUpscaler::TSR;
		if (State.Preset == EAshlineGraphicsPreset::PC_Ultra || State.Preset == EAshlineGraphicsPreset::PC_Balanced)
		{
			State.Preset = EAshlineGraphicsPreset::High;
		}
		State.bRayTracingEnabled = false;
#else
		State.Upscaler = State.bFSR3Available ? EAshlineUpscaler::FSR3 : EAshlineUpscaler::TSR;
		State.bRayTracingEnabled = false;
#endif
		if (State.Preset == EAshlineGraphicsPreset::SteamDeck)
		{
			State.FrameTarget = EAshlineFrameTarget::FPS_40;
			State.Upscaler = State.bFSR3Available ? EAshlineUpscaler::FSR3 : EAshlineUpscaler::TSR;
			State.bRayTracingEnabled = false;
		}
		else if (State.Preset == EAshlineGraphicsPreset::Laptop || State.Preset == EAshlineGraphicsPreset::PC_Performance)
		{
			State.FrameTarget = EAshlineFrameTarget::FPS_60;
			State.bRayTracingEnabled = false;
		}
		else
		{
			State.FrameTarget = EAshlineFrameTarget::Unlimited;
		}
	}

	ApplyCVars();
}

void UAshlineGraphicsSettings::ApplyPreset(EAshlineGraphicsPreset Preset)
{
	State.Preset = Preset;
	State.bAutoDetect = false;

	const bool bDesktopQuality =
		Preset == EAshlineGraphicsPreset::PC_Ultra
		|| Preset == EAshlineGraphicsPreset::PC_High
		|| Preset == EAshlineGraphicsPreset::PC_Balanced;

	if (bDesktopQuality && State.bHardwareRayTracingAvailable)
	{
		State.bRayTracingEnabled = Preset != EAshlineGraphicsPreset::PC_Balanced || State.bHardwareRayTracingAvailable;
		if (Preset == EAshlineGraphicsPreset::PC_Balanced)
		{
			State.bRayTracingEnabled = State.bHardwareRayTracingAvailable;
		}
	}
	if (Preset == EAshlineGraphicsPreset::PC_Performance
		|| Preset == EAshlineGraphicsPreset::SteamDeck
		|| Preset == EAshlineGraphicsPreset::Laptop
		|| Preset == EAshlineGraphicsPreset::Low
		|| Preset == EAshlineGraphicsPreset::Medium)
	{
		State.bRayTracingEnabled = false;
	}

	if (State.Upscaler == EAshlineUpscaler::Off || State.Upscaler == EAshlineUpscaler::MetalFXSpatial || State.Upscaler == EAshlineUpscaler::MetalFXTemporal)
	{
		State.Upscaler = State.bFSR3Available ? EAshlineUpscaler::FSR3 : EAshlineUpscaler::TSR;
	}

	if (Preset == EAshlineGraphicsPreset::SteamDeck)
	{
		State.FrameTarget = EAshlineFrameTarget::FPS_40;
		State.Upscaler = State.bFSR3Available ? EAshlineUpscaler::FSR3 : EAshlineUpscaler::TSR;
		if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
		{
			User->Feel.bForceHandheldHUD = true;
			User->Feel.HUDScale = FMath::Max(User->Feel.HUDScale, 1.2f);
			User->Feel.SafeZone = FMath::Max(User->Feel.SafeZone, 0.08f);
		}
	}
	else if (Preset == EAshlineGraphicsPreset::Laptop || Preset == EAshlineGraphicsPreset::PC_Performance)
	{
		if (State.FrameTarget == EAshlineFrameTarget::Unlimited)
		{
			State.FrameTarget = EAshlineFrameTarget::FPS_60;
		}
	}

	ApplyCVars();
}

void UAshlineGraphicsSettings::ApplyDetectedPreset()
{
	if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
	{
		User->bAutoDetectPreset = true;
	}
	State.bAutoDetect = true;
	ApplySavedOrDetect();
}

void UAshlineGraphicsSettings::CycleNamedPreset(int32 Direction)
{
	const TArray<EAshlineGraphicsPreset> Cycle = NamedPresetCycle();
	int32 Index = Cycle.IndexOfByKey(State.Preset);
	if (Index == INDEX_NONE)
	{
		Index = 0;
	}
	const int32 N = Cycle.Num();
	Index = (Index + (Direction >= 0 ? 1 : -1) + N) % N;
	ApplyPreset(Cycle[Index]);
}

TArray<EAshlineGraphicsPreset> UAshlineGraphicsSettings::NamedPresetCycle()
{
	return {
		EAshlineGraphicsPreset::PC_Ultra,
		EAshlineGraphicsPreset::PC_High,
		EAshlineGraphicsPreset::PC_Balanced,
		EAshlineGraphicsPreset::PC_Performance,
		EAshlineGraphicsPreset::SteamDeck,
		EAshlineGraphicsPreset::Laptop
	};
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

void UAshlineGraphicsSettings::SetFrameTarget(EAshlineFrameTarget Target)
{
	State.FrameTarget = Target;
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
	case EAshlineGraphicsPreset::PC_High: return TEXT("Ashline_PC_High");
	case EAshlineGraphicsPreset::PC_Performance: return TEXT("Ashline_PC_Performance");
	case EAshlineGraphicsPreset::SteamDeck: return TEXT("Ashline_SteamDeck");
	case EAshlineGraphicsPreset::Laptop: return TEXT("Ashline_Laptop");
	default: return TEXT("Unknown");
	}
}

FString UAshlineGraphicsSettings::DescribeTargetHardware()
{
	return TEXT("Desktop: Ryzen 5 7500X3D / 32 GB / RX 9070 GRE @ 1440p Ultra. Handheld: Steam Deck 800p FSR 40 fps. DX12 + Nanite + Lumen + VSM. Upscale: FSR3 (TSR fallback).");
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
	State.AdapterName = FPlatformMisc::GetPrimaryGPUBrand();
	State.bSteamDeck = IsSteamDeckHardware();
	State.bIntegratedGpu = IsIntegratedGpu(State.AdapterName);

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

	State.RecommendedPreset = DetectRecommendedPreset();

	if (State.bWindows || State.bSteamDeck)
	{
		State.CapabilityNotes = FString::Printf(
			TEXT("RHI=%s GPU=%s DX12=%d HW-RT=%d FSR3=%d DLSS=%d Deck=%d iGPU=%d Recommend=%s. %s"),
			*State.RHIName,
			*State.AdapterName,
			State.bDX12 ? 1 : 0,
			State.bHardwareRayTracingAvailable ? 1 : 0,
			State.bFSR3Available ? 1 : 0,
			State.bDLSSAvailable ? 1 : 0,
			State.bSteamDeck ? 1 : 0,
			State.bIntegratedGpu ? 1 : 0,
			*GetPresetDisplayName(State.RecommendedPreset),
			*DescribeTargetHardware());
	}
	else if (State.CapabilityNotes.IsEmpty())
	{
		State.CapabilityNotes = FString::Printf(TEXT("RHI=%s GPU=%s HW-RT=%d MetalFX=%d"), *State.RHIName, *State.AdapterName,
			State.bHardwareRayTracingAvailable ? 1 : 0, State.bMetalFXAvailable ? 1 : 0);
	}

	UE_LOG(LogAshline, Log, TEXT("Graphics probe: %s"), *State.CapabilityNotes);
}

EAshlineGraphicsPreset UAshlineGraphicsSettings::DetectRecommendedPreset() const
{
	if (IsSteamDeckHardware() || State.bSteamDeck)
	{
		return EAshlineGraphicsPreset::SteamDeck;
	}

	const FString Gpu = State.AdapterName.IsEmpty() ? FPlatformMisc::GetPrimaryGPUBrand() : State.AdapterName;
	if (IsIntegratedGpu(Gpu))
	{
		return EAshlineGraphicsPreset::Laptop;
	}

	const FPlatformMemoryConstants& Mem = FPlatformMemory::GetConstants();
	const uint64 RamGB = Mem.TotalPhysical / (1024ull * 1024ull * 1024ull);

	if (LooksLikeUltraGpu(Gpu) && RamGB >= 16)
	{
		return EAshlineGraphicsPreset::PC_Ultra;
	}
	if (LooksLikeMidGpu(Gpu) || RamGB >= 16)
	{
		return EAshlineGraphicsPreset::PC_High;
	}
	if (RamGB < 12)
	{
		return EAshlineGraphicsPreset::PC_Performance;
	}

#if PLATFORM_WINDOWS
	return EAshlineGraphicsPreset::PC_Ultra;
#else
	return EAshlineGraphicsPreset::High;
#endif
}

bool UAshlineGraphicsSettings::IsSteamDeckHardware()
{
	const FString SteamDeck = FPlatformMisc::GetEnvironmentVariable(TEXT("STEAMDECK"));
	if (SteamDeck == TEXT("1") || SteamDeck.Equals(TEXT("true"), ESearchCase::IgnoreCase))
	{
		return true;
	}
	const FString Gpu = FPlatformMisc::GetPrimaryGPUBrand();
	if (Gpu.Contains(TEXT("Van Gogh")) || Gpu.Contains(TEXT("AMD Custom GPU 0405"))
		|| Gpu.Contains(TEXT("Custom GPU 13D8")) || Gpu.Contains(TEXT("Sephiroth")))
	{
		return true;
	}
	const FString Cpu = FPlatformMisc::GetCPUBrand();
	if (Cpu.Contains(TEXT("Aerith")) || Cpu.Contains(TEXT("Sephiroth")))
	{
		return true;
	}
	const FString Make = FPlatformMisc::GetDeviceMake();
	return Make.Contains(TEXT("Jupiter")) || Make.Contains(TEXT("Galileo")) || Make.Contains(TEXT("Steam Deck"));
}

bool UAshlineGraphicsSettings::IsIntegratedGpu(const FString& AdapterName)
{
	const FString G = AdapterName;
	if (G.Contains(TEXT("Intel")) || G.Contains(TEXT("UHD")) || G.Contains(TEXT("Iris")) || G.Contains(TEXT("Arc Graphics")))
	{
		if (G.Contains(TEXT("Arc A")) || G.Contains(TEXT("Arc B")))
		{
			return false;
		}
		return true;
	}
	if ((G.Contains(TEXT("Radeon Graphics")) || G.Contains(TEXT("AMD Radeon(TM) Graphics")))
		&& !G.Contains(TEXT("RX")) && !G.Contains(TEXT("Pro")))
	{
		return true;
	}
	return G.Contains(TEXT("Vega 8")) || G.Contains(TEXT("Vega 11")) || G.Contains(TEXT("Adreno"));
}

bool UAshlineGraphicsSettings::LooksLikeUltraGpu(const FString& AdapterName)
{
	const FString G = AdapterName;
	return G.Contains(TEXT("9070")) || G.Contains(TEXT("9080")) || G.Contains(TEXT("7900"))
		|| G.Contains(TEXT("7900 XTX")) || G.Contains(TEXT("4080")) || G.Contains(TEXT("4090"))
		|| G.Contains(TEXT("5080")) || G.Contains(TEXT("5090")) || G.Contains(TEXT("6950"))
		|| G.Contains(TEXT("6900")) || G.Contains(TEXT("4070 Ti"));
}

bool UAshlineGraphicsSettings::LooksLikeMidGpu(const FString& AdapterName)
{
	const FString G = AdapterName;
	return G.Contains(TEXT("7800")) || G.Contains(TEXT("7700")) || G.Contains(TEXT("6800"))
		|| G.Contains(TEXT("6700")) || G.Contains(TEXT("4060")) || G.Contains(TEXT("4070"))
		|| G.Contains(TEXT("3070")) || G.Contains(TEXT("3080")) || G.Contains(TEXT("7600"));
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
	case EAshlineGraphicsPreset::PC_High: Scalability = 3; break;
	case EAshlineGraphicsPreset::PC_Performance: Scalability = 2; break;
	case EAshlineGraphicsPreset::SteamDeck: Scalability = 1; break;
	case EAshlineGraphicsPreset::Laptop: Scalability = 1; break;
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

	SetCVarInt(TEXT("r.Nanite"), 1);
	SetCVarInt(TEXT("r.Nanite.ProjectEnabled"), 1);
	SetCVarInt(TEXT("r.Shadow.Virtual.Enable"), 1);
	SetCVarInt(TEXT("r.DynamicGlobalIlluminationMethod"), 1);
	SetCVarInt(TEXT("r.ReflectionMethod"), 1);
	SetCVarInt(TEXT("r.Lumen.DiffuseIndirect.Allow"), 1);
	SetCVarInt(TEXT("r.Lumen.Reflections.Allow"), 1);
	SetCVarInt(TEXT("r.DefaultFeature.AntiAliasing"), 4);
	SetCVarInt(TEXT("r.SkinCache.CompileShaders"), 1);
	SetCVarInt(TEXT("r.SkinCache.Mode"), 1);

	const bool bNamed =
		State.Preset == EAshlineGraphicsPreset::PC_Ultra
		|| State.Preset == EAshlineGraphicsPreset::PC_High
		|| State.Preset == EAshlineGraphicsPreset::PC_Balanced
		|| State.Preset == EAshlineGraphicsPreset::PC_Performance
		|| State.Preset == EAshlineGraphicsPreset::SteamDeck
		|| State.Preset == EAshlineGraphicsPreset::Laptop;

	if (bNamed)
	{
		ApplyNamedMachinePreset(State.Preset);
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
		State.bHandheldLayout = false;
		State.SafeZoneScale = 0.f;
		ApplyTextureStreamingCVars(State.Preset);
	}

	ApplyRayTracingCVars();
	ApplyUpscalerCVars();
	ApplyFrameTargetCVars();
	ApplyResolutionForPreset(State.Preset);
	PersistToUserSettings();

	UE_LOG(LogAshline, Log, TEXT("Graphics applied. Preset=%s Upscaler=%d RT=%d FPS=%d Deck=%d SafeZone=%.2f"),
		*GetPresetDisplayName(State.Preset),
		static_cast<int32>(State.Upscaler),
		State.bRayTracingEnabled ? 1 : 0,
		static_cast<int32>(State.FrameTarget),
		State.bHandheldLayout ? 1 : 0,
		State.SafeZoneScale);
}

void UAshlineGraphicsSettings::ApplyNamedMachinePreset(EAshlineGraphicsPreset Preset)
{
	const bool bUltra = Preset == EAshlineGraphicsPreset::PC_Ultra;
	const bool bHigh = Preset == EAshlineGraphicsPreset::PC_High;
	const bool bBalanced = Preset == EAshlineGraphicsPreset::PC_Balanced;
	const bool bPerf = Preset == EAshlineGraphicsPreset::PC_Performance;
	const bool bDeck = Preset == EAshlineGraphicsPreset::SteamDeck;
	const bool bLaptop = Preset == EAshlineGraphicsPreset::Laptop;

	State.bHandheldLayout = bDeck;
	State.SafeZoneScale = bDeck ? 0.07f : (bLaptop ? 0.03f : 0.f);

	SetCVarInt(TEXT("r.VSync"), (bDeck || bLaptop) ? 1 : 0);
	SetCVarInt(TEXT("r.FinishCurrentFrame"), bDeck ? 1 : 0);
	SetCVarInt(TEXT("r.MaxAnisotropy"), bUltra ? 16 : (bHigh || bBalanced ? 8 : 4));
	SetCVarInt(TEXT("r.VT.MaxAnisotropy"), bUltra ? 8 : 4);
	SetCVarInt(TEXT("r.Streaming.PoolSize"), bUltra ? 5600 : (bHigh ? 4200 : (bBalanced ? 3800 : (bDeck ? 1800 : 1400))));
	SetCVarInt(TEXT("r.Streaming.LimitPoolSizeToVRAM"), 1);
	ApplyTextureStreamingCVars(Preset);
	SetCVarFloat(TEXT("r.ViewDistanceScale"), bUltra ? 1.15f : (bHigh ? 1.0f : (bBalanced ? 0.9f : (bDeck ? 0.65f : 0.7f))));
	SetCVarFloat(TEXT("r.Shadow.DistanceScale"), bUltra ? 1.1f : (bHigh ? 0.95f : (bDeck ? 0.55f : 0.75f)));
	SetCVarInt(TEXT("r.Shadow.Virtual.MaxQuality"), bUltra ? 3 : (bHigh ? 2 : 1));
	SetCVarInt(TEXT("r.Shadow.Virtual.SMRT.RayCountDirectional"), bUltra ? 8 : (bHigh ? 4 : 2));
	SetCVarInt(TEXT("r.Lumen.ScreenProbeGather.RadianceCache.ProbeResolution"), bUltra ? 32 : (bHigh ? 16 : 8));
	SetCVarInt(TEXT("r.Lumen.ScreenProbeGather.DownsampleFactor"), bUltra ? 16 : (bHigh ? 24 : 32));
	SetCVarInt(TEXT("r.Lumen.Reflections.DownsampleFactor"), bUltra ? 1 : 2);
	SetCVarInt(TEXT("r.Lumen.TraceMeshSDFs"), (bDeck || bLaptop || bPerf) ? 0 : 1);
	SetCVarInt(TEXT("r.Nanite.MaxPixelsPerEdge"), bUltra ? 1 : (bDeck ? 4 : 2));
	SetCVarInt(TEXT("r.AmbientOcclusionLevels"), bUltra ? 2 : (bHigh ? 1 : 0));
	SetCVarInt(TEXT("r.BloomQuality"), bUltra ? 5 : (bHigh || bBalanced ? 4 : 2));
	SetCVarInt(TEXT("r.MotionBlurQuality"), 0);
	SetCVarInt(TEXT("r.DepthOfFieldQuality"), bUltra ? 2 : 0);
	SetCVarFloat(TEXT("foliage.DensityScale"), bUltra ? 1.f : (bHigh ? 0.85f : (bBalanced ? 0.7f : (bDeck ? 0.35f : 0.45f))));
	SetCVarFloat(TEXT("r.Tonemapper.Sharpen"), bDeck ? 0.25f : 0.45f);

	float ScreenPct = 100.f;
	if (State.Upscaler == EAshlineUpscaler::FSR3)
	{
		ScreenPct = bUltra ? 77.f : (bHigh ? 67.f : (bBalanced ? 59.f : (bDeck ? 59.f : 50.f)));
	}
	else if (State.Upscaler == EAshlineUpscaler::TSR)
	{
		ScreenPct = bUltra ? 85.f : (bHigh ? 77.f : (bBalanced ? 70.f : (bDeck ? 67.f : 59.f)));
	}
	else if (State.Upscaler == EAshlineUpscaler::DLSS)
	{
		ScreenPct = bUltra ? 67.f : (bHigh ? 58.f : 50.f);
	}
	else
	{
		ScreenPct = bUltra ? 100.f : (bDeck ? 77.f : 80.f);
	}
	SetCVarFloat(TEXT("r.ScreenPercentage"), ScreenPct);
	(void)bPerf;
}

void UAshlineGraphicsSettings::ApplyTextureStreamingCVars(EAshlineGraphicsPreset Preset)
{
	const bool bUltra = Preset == EAshlineGraphicsPreset::PC_Ultra;
	const bool bHigh = Preset == EAshlineGraphicsPreset::PC_High;
	const bool bBalanced = Preset == EAshlineGraphicsPreset::PC_Balanced;
	const bool bDeck = Preset == EAshlineGraphicsPreset::SteamDeck;
	const bool bLaptop = Preset == EAshlineGraphicsPreset::Laptop;
	const bool bPerf = Preset == EAshlineGraphicsPreset::PC_Performance;

	SetCVarInt(TEXT("r.TextureStreaming"), 1);
	SetCVarInt(TEXT("r.Streaming.LimitPoolSizeToVRAM"), 1);
	SetCVarInt(TEXT("r.Streaming.UseAllMips"), 0);
	SetCVarInt(TEXT("r.Streaming.AmortizeCPUToGPUCopy"), 1);
	SetCVarInt(TEXT("r.Streaming.MaxNumTexturesToStreamPerFrame"), bDeck ? 8 : (bLaptop || bPerf ? 10 : 16));
	SetCVarInt(TEXT("r.Streaming.FramesForFullUpdate"), bDeck ? 7 : (bLaptop ? 6 : 5));
	SetCVarFloat(TEXT("r.Streaming.Boost"), bUltra ? 1.f : (bHigh ? 0.85f : 0.7f));
	SetCVarFloat(TEXT("r.Streaming.MipBias"), bDeck ? 0.5f : (bLaptop || bPerf ? 0.25f : 0.f));
	SetCVarInt(TEXT("r.VT.Enable"), 1);
	SetCVarFloat(TEXT("r.VT.PoolSizeScale"), bUltra ? 1.15f : (bHigh ? 1.0f : (bBalanced ? 0.8f : (bDeck ? 0.45f : 0.55f))));
	SetCVarInt(TEXT("r.VT.MaxUploadsPerFrame"), bUltra ? 24 : (bHigh ? 18 : (bDeck ? 8 : 12)));
	SetCVarInt(TEXT("r.VT.MaxTilesProducedPerFrame"), bDeck ? 16 : (bUltra ? 48 : 32));
	SetCVarFloat(TEXT("r.Streaming.HiddenPrimitiveScale"), bDeck ? 0.4f : 0.5f);
	SetCVarInt(TEXT("r.Streaming.UseNewMetrics"), 1);
}

void UAshlineGraphicsSettings::ApplyUpscalerCVars()
{
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

	int32 FsrQuality = 1;
	if (State.Preset == EAshlineGraphicsPreset::PC_Balanced || State.Preset == EAshlineGraphicsPreset::SteamDeck)
	{
		FsrQuality = 2;
	}
	else if (State.Preset == EAshlineGraphicsPreset::PC_Performance || State.Preset == EAshlineGraphicsPreset::Laptop)
	{
		FsrQuality = 3;
	}

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
		SetCVarInt(TEXT("r.FidelityFX.FSR3.QualityMode"), FsrQuality);
		SetCVarInt(TEXT("r.FidelityFX.FI.Enabled"), bFrameGeneration ? 1 : 0);
		break;
	case EAshlineUpscaler::DLSS:
		SetCVarInt(TEXT("r.NGX.DLSS.Enable"), 1);
		SetCVarInt(TEXT("r.NGX.DLSS.Quality"), State.Preset == EAshlineGraphicsPreset::PC_Ultra ? 1 : 2);
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
	else
	{
		SetCVarInt(TEXT("r.RayTracing.Shadows"), 0);
	}
}

void UAshlineGraphicsSettings::ApplyFrameTargetCVars()
{
	float MaxFps = 0.f;
	switch (State.FrameTarget)
	{
	case EAshlineFrameTarget::FPS_30: MaxFps = 30.f; break;
	case EAshlineFrameTarget::FPS_40: MaxFps = 40.f; break;
	case EAshlineFrameTarget::FPS_60: MaxFps = 60.f; break;
	default: MaxFps = 0.f; break;
	}
	SetCVarFloat(TEXT("t.MaxFPS"), MaxFps);
	if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
	{
		User->SetFrameRateLimit(MaxFps);
	}
}

void UAshlineGraphicsSettings::ApplyResolutionForPreset(EAshlineGraphicsPreset Preset)
{
	UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings();
	if (!User)
	{
		return;
	}
	if (Preset == EAshlineGraphicsPreset::SteamDeck)
	{
		User->TargetResX = 1280;
		User->TargetResY = 800;
		User->SetScreenResolution(FIntPoint(1280, 800));
		User->SetVSyncEnabled(true);
		User->SetFrameRateLimit(40.f);
		User->ApplySettings(false);
	}
}

void UAshlineGraphicsSettings::PersistToUserSettings()
{
	if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
	{
		User->NamedPreset = State.Preset;
		User->PreferredUpscaler = State.Upscaler;
		User->FrameTarget = State.FrameTarget;
		User->SafeZoneScale = State.SafeZoneScale;
		User->bHandheldLayout = State.bHandheldLayout;
		User->bAutoDetectPreset = State.bAutoDetect;
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

void UAshlineGraphicsSettings::ApplyHighPreset()
{
	ApplyPreset(EAshlineGraphicsPreset::PC_High);
}

void UAshlineGraphicsSettings::ApplyBalancedPreset()
{
	ApplyPreset(EAshlineGraphicsPreset::PC_Balanced);
}

void UAshlineGraphicsSettings::ApplyPerformancePreset()
{
	ApplyPreset(EAshlineGraphicsPreset::PC_Performance);
}

void UAshlineGraphicsSettings::ApplySteamDeckPreset()
{
	ApplyPreset(EAshlineGraphicsPreset::SteamDeck);
}

void UAshlineGraphicsSettings::ApplyLaptopPreset()
{
	ApplyPreset(EAshlineGraphicsPreset::Laptop);
}

void UAshlineGraphicsSettings::ToggleFSR3()
{
	SetUpscaler(EAshlineUpscaler::FSR3);
}

void UAshlineGraphicsSettings::ToggleTSR()
{
	SetUpscaler(EAshlineUpscaler::TSR);
}

void UAshlineGraphicsSettings::RegisterConsoleCommands()
{
	IConsoleManager& CM = IConsoleManager::Get();
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshPCUltra"), TEXT("Apply Ashline_PC_Ultra (1440p / 9070 GRE class)."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplyUltraPreset), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshPCHigh"), TEXT("Apply Ashline_PC_High (mid discrete GPU)."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplyHighPreset), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshPCBalanced"), TEXT("Apply Ashline_PC_Balanced."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplyBalancedPreset), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshPCPerf"), TEXT("Apply Ashline_PC_Performance."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplyPerformancePreset), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshSteamDeck"), TEXT("Apply Ashline_SteamDeck (800p / FSR / 40 fps)."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplySteamDeckPreset), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshDeck"), TEXT("Alias for AshSteamDeck (handheld HUD + FSR)."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplySteamDeckPreset), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshLaptop"), TEXT("Apply Ashline_Laptop (iGPU fallback)."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplyLaptopPreset), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshGfxAuto"), TEXT("Re-detect GPU and apply the recommended preset."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ApplyDetectedPreset), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshGfxCycle"), TEXT("Cycle Ultra / High / Balanced / Perf / Deck / Laptop."),
		FConsoleCommandDelegate::CreateLambda([this]() { CycleNamedPreset(1); }), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshFSR"), TEXT("Switch upscaler to FSR3 (TSR if plugin CVars are missing)."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ToggleFSR3), ECVF_Default));
	ConsoleObjects.Add(CM.RegisterConsoleCommand(
		TEXT("AshTSR"), TEXT("Switch upscaler to Unreal TSR."),
		FConsoleCommandDelegate::CreateUObject(this, &UAshlineGraphicsSettings::ToggleTSR), ECVF_Default));
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
