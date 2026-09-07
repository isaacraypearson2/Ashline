#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "AshlineMetalFX.h"
#include "AshlineMetalFXSubsystem.generated.h"

UCLASS()
class ASHLINEAPPLE_API UAshlineMetalFXSubsystem : public UEngineSubsystem, public IAshlineMetalFX
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool IsMetalFXAvailable() const override;
	virtual bool IsHardwareRayTracingAvailable() const override;
	virtual FString GetCapabilitySummary() const override;
	virtual void ApplyUpscaler(int32 Mode) override;

private:
	void Probe();

	bool bMetalFX = false;
	bool bHardwareRT = false;
	FString Summary;
};
