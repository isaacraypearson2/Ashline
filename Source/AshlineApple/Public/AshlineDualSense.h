#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "AshlineDualSense.generated.h"

UENUM(BlueprintType)
enum class EAshlineDualSenseEvent : uint8
{
	Fire,
	Reload,
	Damage,
	Explosion
};

UCLASS()
class ASHLINEAPPLE_API UAshlineDualSense : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Input")
	bool IsDualSenseConnected() const;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Input")
	void TriggerWeaponFeedback(uint8 WeaponClass, float RecoilPitch);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Input")
	void Pulse(EAshlineDualSenseEvent Event, float Intensity = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Input")
	void SetAdaptiveTriggersForWeapon(uint8 WeaponClass);

	UFUNCTION(BlueprintPure, Category = "Ashline|Input")
	FString GetControllerSummary() const { return ControllerSummary; }

private:
	void RefreshControllers();

	bool bConnected = false;
	FString ControllerSummary;
};
