#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AshlineTypes.h"
#include "AshlineGraphicsSettings.generated.h"

USTRUCT(BlueprintType)
struct FAshlineGraphicsState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	EAshlineGraphicsPreset Preset = EAshlineGraphicsPreset::PC_Ultra;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	EAshlineGraphicsPreset RecommendedPreset = EAshlineGraphicsPreset::PC_Ultra;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	EAshlineUpscaler Upscaler = EAshlineUpscaler::FSR3;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	EAshlineFrameTarget FrameTarget = EAshlineFrameTarget::Unlimited;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bMetalFXAvailable = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bFSR3Available = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bDLSSAvailable = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bHardwareRayTracingAvailable = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bRayTracingEnabled = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bDX12 = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bWindows = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bSteamDeck = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bIntegratedGpu = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bHandheldLayout = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bAutoDetect = true;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	float SafeZoneScale = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	FString RHIName;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	FString AdapterName;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	FString CapabilityNotes;
};

/**
 * Cross-platform graphics path.
 * Windows DX12: Nanite / Lumen / VSM / HW RT / FSR3 (TSR fallback, DLSS optional).
 * Steam Deck / Proton: 800p FSR, aggressive Nanite/Lumen/VSM cuts, 30/40/60 caps.
 * Apple: MetalFX + capability-gated RT via AshlineApple.
 */
UCLASS()
class ASHLINE_API UAshlineGraphicsSettings : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void ApplySavedOrDetect();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void ApplyPreset(EAshlineGraphicsPreset Preset);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void ApplyDetectedPreset();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void CycleNamedPreset(int32 Direction = 1);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void SetUpscaler(EAshlineUpscaler Upscaler);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void SetFrameTarget(EAshlineFrameTarget Target);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	bool TryEnableRayTracing(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void SetFrameGeneration(bool bEnable);

	UFUNCTION(BlueprintPure, Category = "Ashline|Graphics")
	FAshlineGraphicsState GetState() const { return State; }

	UFUNCTION(BlueprintPure, Category = "Ashline|Graphics")
	static FString GetPresetDisplayName(EAshlineGraphicsPreset Preset);

	UFUNCTION(BlueprintPure, Category = "Ashline|Graphics")
	static FString DescribeTargetHardware();

	UFUNCTION(BlueprintPure, Category = "Ashline|Graphics")
	EAshlineGraphicsPreset DetectRecommendedPreset() const;

	UFUNCTION(BlueprintPure, Category = "Ashline|Graphics")
	static TArray<EAshlineGraphicsPreset> NamedPresetCycle();

private:
	void ProbeCapabilities();
	void ApplyCVars();
	void ApplyNamedMachinePreset(EAshlineGraphicsPreset Preset);
	void ApplyUpscalerCVars();
	void ApplyRayTracingCVars();
	void ApplyFrameTargetCVars();
	void ApplyResolutionForPreset(EAshlineGraphicsPreset Preset);
	void PersistToUserSettings();
	static void SetCVarInt(const TCHAR* Name, int32 Value);
	static void SetCVarFloat(const TCHAR* Name, float Value);
	static bool HasCVar(const TCHAR* Name);
	static bool IsSteamDeckHardware();
	static bool IsIntegratedGpu(const FString& AdapterName);
	static bool LooksLikeUltraGpu(const FString& AdapterName);
	static bool LooksLikeMidGpu(const FString& AdapterName);

	void ApplyUltraPreset();
	void ApplyHighPreset();
	void ApplyBalancedPreset();
	void ApplyPerformancePreset();
	void ApplySteamDeckPreset();
	void ApplyLaptopPreset();
	void RegisterConsoleCommands();
	void UnregisterConsoleCommands();

	UPROPERTY()
	FAshlineGraphicsState State;

	bool bFrameGeneration = false;

	TArray<IConsoleObject*> ConsoleObjects;
};
