#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AshlineTypes.h"
#include "AshlineAudioDirector.generated.h"

class USoundBase;

/** Sound-cue slots: fire / reload / hit / footsteps / music / ambience / stingers. Engine/Starter fallbacks when assigned assets are missing. */
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
	void PlayHit(UObject* WorldContext, const FVector& Location, bool bFlesh = false);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void PlayFootstep(UObject* WorldContext, const FVector& Location, EAshlineSurface Surface = EAshlineSurface::Ground);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void StartMusicBed(UObject* WorldContext, EAshlineMissionId MissionId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void StopMusicBed();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void StartAmbience(UObject* WorldContext, EAshlineMissionId MissionId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void StopAmbience();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void NotifyCombat(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void PlayStinger(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void PlayUI(UObject* WorldContext, bool bConfirm);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void PlayExplosion(UObject* WorldContext, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Audio")
	void SetBusVolumes(float Master, float SFX, float Music);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> DefaultAmbience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> CombatStingerCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> UIConfirmCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> UIBackCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Audio")
	TSoftObjectPtr<USoundBase> ExplosionCue;

private:
	USoundBase* Resolve(const TSoftObjectPtr<USoundBase>& Soft, const TArray<FString>& FallbackPaths) const;
	float SFXGain() const { return MasterVolume * SFXVolume; }
	float MusicGain() const { return MasterVolume * MusicVolume; }

	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComp;

	UPROPERTY()
	TObjectPtr<UAudioComponent> AmbienceComp;

	bool bCombatLayer = false;
	float MasterVolume = 1.f;
	float SFXVolume = 1.f;
	float MusicVolume = 0.7f;
};
