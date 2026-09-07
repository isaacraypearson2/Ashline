#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshlineTypes.h"
#include "AshlineWeaponComponent.generated.h"

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
};

UCLASS(ClassGroup = (Ashline), meta = (BlueprintSpawnableComponent))
class ASHLINE_API UAshlineWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAshlineWeaponComponent();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	float TraceDistance = 20000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Weapons")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

private:
	void FireShot();
	void ApplyRecoil();
	FVector GetMuzzleLocation() const;
	FRotator GetAimRotation() const;

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
};
