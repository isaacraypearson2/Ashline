#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.h"
#include "AshlineMetaCatalog.generated.h"

/** Single-player meta: rank curve, credits, cosmetics, weapon skins. No multiplayer. */
UCLASS()
class ASHLINE_API UAshlineMetaCatalog : public UObject
{
	GENERATED_BODY()

public:
	static constexpr int32 MaxRank = 50;
	static constexpr int32 StartingCredits = 750;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	static TArray<FAshlineCosmeticDefinition> BuildCosmetics();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	static TArray<FAshlineWeaponSkinDefinition> BuildWeaponSkins();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	static TArray<FAshlineRankTier> BuildRankTable();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	static bool FindCosmetic(FName CosmeticId, FAshlineCosmeticDefinition& OutCosmetic);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	static bool FindSkin(FName SkinId, FAshlineWeaponSkinDefinition& OutSkin);

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static int32 XPToNextRank(int32 Rank);

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static FAshlineRankTier RankTier(int32 Rank);

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static int32 MissionCreditReward(int32 BaseXP, int32 Stars, EAshlineDifficulty Difficulty);

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static int32 WeaponUpgradeCost(int32 CurrentTier);

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static bool SkinFitsWeapon(const FAshlineWeaponSkinDefinition& Skin, FName WeaponId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static TArray<FName> StarterCosmeticIds();

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static TArray<FName> StarterSkinIds();

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static FLinearColor CosmeticTint(FName CosmeticId, FLinearColor Fallback);

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static FLinearColor SkinTint(FName SkinId, FLinearColor Fallback);

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static FName EquippedCosmeticId(const FAshlineOperatorProfile& Profile, EAshlineCosmeticSlot Slot);

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	static bool SlotFromName(FName SlotName, EAshlineCosmeticSlot& OutSlot);

	/** Canonical Content paths so MeshOverride / MaterialOverride resolve when packs land. */
	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	static void BindCosmeticContentPaths(FAshlineCosmeticDefinition& Cosmetic);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	static void BindSkinContentPaths(FAshlineWeaponSkinDefinition& Skin);

	/** DataAsset override (DA_COS_*) wins over the catalog path when the asset exists. */
	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	static void OverlayCosmeticDataAsset(FAshlineCosmeticDefinition& Cosmetic);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	static void OverlaySkinDataAsset(FAshlineWeaponSkinDefinition& Skin);
};
