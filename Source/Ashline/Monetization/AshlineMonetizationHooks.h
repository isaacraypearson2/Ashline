#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AshlineMonetizationHooks.generated.h"

/**
 * Stub only. Ashline is a premium single-player campaign.
 * These hooks exist so a later Apple ads / IAP experiment can plug in
 * without rewriting frontend flow. No ads are shown.
 */
UCLASS()
class ASHLINE_API UAshlineMonetizationHooks : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Monetization")
	bool AreAdsEnabled() const { return bAdsEnabled; }

	UFUNCTION(BlueprintCallable, Category = "Ashline|Monetization")
	void SetAdsEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Monetization")
	bool RequestRewardedPlacement(FName PlacementId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Monetization")
	bool RequestInterstitialPlacement(FName PlacementId);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Monetization")
	FString Provider = TEXT("None");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Monetization")
	FString Notes = TEXT("Stub. Wire a future ads SDK here. Campaign loot is play-earned only.");

private:
	bool bAdsEnabled = false;
};
