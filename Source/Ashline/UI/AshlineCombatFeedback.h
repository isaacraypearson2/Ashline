#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AshlineTypes.h"
#include "AshlineCombatFeedback.generated.h"

USTRUCT(BlueprintType)
struct FAshlineKillFeedItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	FString Text;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	float Remaining = 0.f;
};

/** Runtime combat/UI events for the Canvas HUD. No netcode. */
UCLASS()
class ASHLINE_API UAshlineCombatFeedback : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return true; }
	virtual bool IsTickableWhenPaused() const override { return true; }

	UFUNCTION(BlueprintCallable, Category = "Ashline|HUD")
	void NotifyHitMarker(bool bKilled);

	UFUNCTION(BlueprintCallable, Category = "Ashline|HUD")
	void NotifyKillConfirm(const FString& TargetName);

	UFUNCTION(BlueprintCallable, Category = "Ashline|HUD")
	void NotifyDamageTaken(float NormalizedHealth);

	UFUNCTION(BlueprintCallable, Category = "Ashline|HUD")
	void SetInteractPrompt(const FString& Prompt);

	UFUNCTION(BlueprintCallable, Category = "Ashline|HUD")
	void ClearInteractPrompt();

	UFUNCTION(BlueprintCallable, Category = "Ashline|HUD")
	void SetSubtitle(const FString& Text, float Seconds);

	UFUNCTION(BlueprintCallable, Category = "Ashline|HUD")
	void NotifyObjective(const FString& Text);

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	float HitMarkerRemaining = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	bool bLastHitWasKill = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	float KillConfirmRemaining = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	FString KillConfirmName;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	float DamagePulse = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	FString InteractPrompt;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	float InteractRemaining = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	FString Subtitle;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	float SubtitleRemaining = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	TArray<FAshlineKillFeedItem> KillFeed;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|HUD")
	int32 MissionKills = 0;

	void ResetMission();
};
