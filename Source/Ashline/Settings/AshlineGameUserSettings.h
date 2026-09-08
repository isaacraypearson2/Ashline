#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "AshlineTypes.h"
#include "AshlineGameUserSettings.generated.h"

UCLASS(config = GameUserSettings)
class ASHLINE_API UAshlineGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	virtual void SetToDefaults() override;
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	static UAshlineGameUserSettings* GetAshlineSettings();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graphics")
	void ApplyTargetPCResolution();

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ashline|Graphics")
	EAshlineGraphicsPreset NamedPreset = EAshlineGraphicsPreset::PC_Ultra;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ashline|Graphics")
	EAshlineUpscaler PreferredUpscaler = EAshlineUpscaler::FSR3;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ashline|Graphics")
	int32 TargetResX = 2560;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ashline|Graphics")
	int32 TargetResY = 1440;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ashline|Graphics")
	bool bAutoDetectPreset = true;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ashline|Graphics")
	EAshlineFrameTarget FrameTarget = EAshlineFrameTarget::Unlimited;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ashline|Graphics")
	float SafeZoneScale = 0.f;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ashline|Graphics")
	bool bHandheldLayout = false;
};
