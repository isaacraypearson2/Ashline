#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshlineTypes.h"
#include "AshlineWeaponComponent.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class UAshlineWeaponVisual;
class UMaterialInterface;
class USoundBase;

USTRUCT(BlueprintType)
struct FAshlineRuntimeWeapon
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Weapons")
	FAshlineWeaponDefinition Definition;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Weapons")
	FAshlineWeaponStats Stats;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Weapons")
	int32 AmmoInMag = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Weapons")
	int32 Reserve = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Weapons")
	int32 UpgradeTier = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Ashline|Weapons")
	FName SkinId = TEXT("SKIN_FACTORY");
};

UCLASS(ClassGroup = (Ashline), meta = (BlueprintSpawnableComponent))
class ASHLINE_API UAshlineWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAshlineWeaponComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	void LoadFromLoadout(const FAshlineLoadoutSlot& Primary, const FAshlineLoadoutSlot& Secondary, int32 PrimaryUpgrade, int32 SecondaryUpgrade);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	void SwapWeapon();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	void SetAiming(bool bAiming);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	const FAshlineRuntimeWeapon& GetActiveWeapon() const;

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	bool IsFiring() const { return bWantsFire; }

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	bool IsReloading() const { return bReloading; }

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	bool IsAiming() const { return bAiming; }

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	float GetReloadAlpha() const;

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	FString GetFireModeLabel() const;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	void RefreshVisuals();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	void ApplyEquippedSkin();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	void ApplyCharm(FName CharmId);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float TraceDistance = 20000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Weapons")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Weapons")
	TObjectPtr<UStaticMeshComponent> BarrelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Weapons")
	TObjectPtr<UStaticMeshComponent> StockMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Weapons")
	TObjectPtr<UStaticMeshComponent> MagMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Weapons")
	TObjectPtr<UPointLightComponent> MuzzleLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Weapons")
	TObjectPtr<UStaticMeshComponent> CharmMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	TSoftObjectPtr<UAshlineWeaponVisual> VisualOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float ImpactDecalSize = 12.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float MuzzleFlashSeconds = 0.045f;

private:
	void FireShot();
	void ApplyRecoil();
	FVector GetMuzzleLocation() const;
	FRotator GetAimRotation() const;
	void SpawnMuzzleFX();
	void SpawnImpact(const FHitResult& Hit);
	void AttachVisuals();
	void BuildCompoundPlaceholder();
	void ApplyVisualAsset(UAshlineWeaponVisual* Visual);
	void ApplyTintToWeaponMeshes(const FLinearColor& Tint, UMaterialInterface* Override);
	void PlayFireAudio();
	void PlayReloadAudio();

	FName EquippedCharmId;

	UPROPERTY()
	FAshlineRuntimeWeapon PrimaryWeapon;

	UPROPERTY()
	FAshlineRuntimeWeapon SecondaryWeapon;

	bool bUsingPrimary = true;
	bool bWantsFire = false;
	bool bAiming = false;
	bool bReloading = false;
	float FireCooldown = 0.f;
	float ReloadRemaining = 0.f;
	float MuzzleFlashRemaining = 0.f;
};
