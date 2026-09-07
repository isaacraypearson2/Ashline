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
	EAshlineGraphicsPreset Preset = EAshlineGraphicsPreset::High;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	EAshlineUpscaler Upscaler = EAshlineUpscaler::Off;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bMetalFXAvailable = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bHardwareRayTracingAvailable = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	bool bRayTracingEnabled = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Graphics")
	FString CapabilityNotes;
};

UCLASS()
class ASHLINE_API UAshlineGraphicsSettings : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void ApplySavedOrDetect();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void ApplyPreset(EAshlineGraphicsPreset Preset);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void SetUpscaler(EAshlineUpscaler Upscaler);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	bool TryEnableRayTracing(bool bEnable);

	UFUNCTION(BlueprintPure, Category = "Ashline|Graphics")
	FAshlineGraphicsState GetState() const { return State; }

private:
	void ProbeAppleCapabilities();
	void ApplyCVars();

	UPROPERTY()
	FAshlineGraphicsState State;
};
