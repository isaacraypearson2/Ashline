#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.h"
#include "AshlineWeaponCatalog.generated.h"

UCLASS()
class ASHLINE_API UAshlineWeaponCatalog : public UObject
{
	GENERATED_BODY()

public:
	static constexpr int32 MaxUpgradeTier = 5;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static TArray<FAshlineWeaponDefinition> BuildRoster();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static TArray<FAshlineAttachmentDefinition> BuildAttachments();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static bool FindWeapon(FName WeaponId, FAshlineWeaponDefinition& OutWeapon);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static bool FindAttachment(FName AttachmentId, FAshlineAttachmentDefinition& OutAttachment);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static FAshlineWeaponStats ComposeStats(const FAshlineWeaponDefinition& Weapon, const TMap<EAshlineAttachmentSlot, FName>& Attachments);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static TArray<FAshlineWeaponDefinition> WeaponsOfClass(EAshlineWeaponClass Class);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static TArray<FName> StarterWeaponIds();

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static int32 UpgradeCostForClass(EAshlineWeaponClass Class, int32 CurrentTier);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static int32 AttachmentUnlockCost(const FAshlineAttachmentDefinition& Attachment);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static FString FireModeName(EAshlineFireMode Mode);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static FString AmmoTypeName(EAshlineAmmoType Type);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static EAshlineFireMode CycleFireMode(const FAshlineWeaponDefinition& Weapon, EAshlineFireMode Current);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static void ApplyAmmoType(FAshlineWeaponStats& Stats, EAshlineAmmoType Type);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static float DamageAtRange(const FAshlineWeaponStats& Stats, float DistanceMeters);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static FVector2D RecoilKick(const FAshlineWeaponStats& Stats, int32 ShotIndex, bool bADS);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static void ApplyClassGunfeel(FAshlineWeaponDefinition& Weapon);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Weapons")
	static void ApplyWeaponIdGunfeel(FAshlineWeaponDefinition& Weapon);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static TArray<FName> DefaultKit(EAshlineWeaponClass Class);
};
