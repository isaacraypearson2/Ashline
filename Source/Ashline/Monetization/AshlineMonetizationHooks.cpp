#include "Monetization/AshlineMonetizationHooks.h"

#include "Ashline.h"

void UAshlineMonetizationHooks::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bAdsEnabled = false;
	Provider = TEXT("None");
	UE_LOG(LogAshline, Log, TEXT("MonetizationHooks initialized (ads disabled, stub only)."));
}

void UAshlineMonetizationHooks::SetAdsEnabled(bool bEnabled)
{
	bAdsEnabled = bEnabled;
	UE_LOG(LogAshline, Log, TEXT("MonetizationHooks ads flag=%s (still a stub)."), bEnabled ? TEXT("on") : TEXT("off"));
}

bool UAshlineMonetizationHooks::RequestRewardedPlacement(FName PlacementId)
{
	UE_LOG(LogAshline, Verbose, TEXT("Rewarded placement '%s' ignored — stub."), *PlacementId.ToString());
	return false;
}

bool UAshlineMonetizationHooks::RequestInterstitialPlacement(FName PlacementId)
{
	UE_LOG(LogAshline, Verbose, TEXT("Interstitial placement '%s' ignored — stub."), *PlacementId.ToString());
	return false;
}
