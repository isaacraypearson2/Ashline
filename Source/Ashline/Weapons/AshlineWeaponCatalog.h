#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.h"
#include "AshlineWeaponCatalog.generated.h"

UCLASS()
class ASHLINE_API UAshlineWeaponCatalog : public UObject
{
	GENERATED_BODY()

public:
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

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static bool IsWeaponUnlockedAtRank(FName WeaponId, int32 Rank);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static bool IsAttachmentUnlockedAtRank(FName AttachmentId, int32 Rank);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static bool IsAttachmentCompatible(FName WeaponId, FName AttachmentId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static TArray<FName> StarterAttachmentIds(FName WeaponId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Weapons")
	static TArray<FName> AttachmentsUnlockedByRank(FName WeaponId, int32 Rank);
};
