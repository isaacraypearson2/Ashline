#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AshlineTypes.h"
#include "AshlineWeaponVisual.generated.h"

class UStaticMesh;
class USkeletalMesh;
class UNiagaraSystem;
class UMaterialInterface;
class USoundBase;
class UParticleSystem;

/** Visual / FX / audio slots for one catalog weapon. Placeholder meshes are valid; swap via Fab. */
UCLASS(BlueprintType)
class ASHLINE_API UAshlineWeaponVisual : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	FName WeaponId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<UStaticMesh> WorldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<UNiagaraSystem> MuzzleFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<UParticleSystem> MuzzleCascadeFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<UNiagaraSystem> TracerFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<UNiagaraSystem> ImpactFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<UNiagaraSystem> BloodFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<UParticleSystem> SparksCascade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<UParticleSystem> ExplosionCascade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<UMaterialInterface> ImpactDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<USoundBase> FireCue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<USoundBase> ReloadCue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	TSoftObjectPtr<USoundBase> HitCue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	FName MuzzleSocket = TEXT("Muzzle");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	FVector FPSOffset = FVector(28.f, 14.f, -10.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	FRotator FPSRotation = FRotator(0.f, 0.f, 8.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	FVector MeshScale = FVector(0.35f, 0.12f, 0.12f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	FLinearColor WeaponTint = FLinearColor(0.08f, 0.08f, 0.09f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Weapon")
	FString FabSwapNotes;
};
