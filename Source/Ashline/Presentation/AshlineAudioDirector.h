#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AshlineTypes.h"
#include "AshlineAudioDirector.generated.h"

class USoundBase;

/** Sound-cue slots for fire / reload / hit / footsteps / music. Silent unless assets are assigned. */
UCLASS()
class ASHLINE_API UAshlineAudioDirector : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void PlayFire(UObject* WorldContext, FName WeaponId, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void PlayReload(UObject* WorldContext, FName WeaponId, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void PlayHit(UObject* WorldContext, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void PlayFootstep(UObject* WorldContext, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void StartMusicBed(UObject* WorldContext, EAshlineMissionId MissionId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void StopMusicBed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> DefaultFireCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> DefaultReloadCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> DefaultHitCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> DefaultFootstepCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> FrontendMusic;

private:
	USoundBase* Resolve(const TSoftObjectPtr<USoundBase>& Soft, const TArray<FString>& FallbackPaths) const;

	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComp;
};
