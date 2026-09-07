#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AshlineTypes.h"
#include "AshlineSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FAshlineMissionProgress
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	EAshlineMissionId MissionId = EAshlineMissionId::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	EAshlineMissionState State = EAshlineMissionState::Locked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	int32 BestStars = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	bool bOptionalComplete = false;
};

USTRUCT(BlueprintType)
struct FAshlineOwnedWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	FName WeaponId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	int32 UpgradeTier = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	TArray<FName> UnlockedAttachments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	TMap<EAshlineAttachmentSlot, FName> EquippedAttachments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	FName EquippedSkinId = TEXT("SKIN_FACTORY");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	TArray<FName> UnlockedSkins;
};

USTRUCT(BlueprintType)
struct FAshlineCrateGrant
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	FName ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	EAshlineLootRarity Rarity = EAshlineLootRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	FText DisplayName;
};

UCLASS()
class ASHLINE_API UAshlineSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Save")
	FString SlotVersion = TEXT("2.0.0");

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	FAshlineOperatorProfile Operator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	EAshlineDifficulty Difficulty = EAshlineDifficulty::Regular;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	EAshlineCameraMode PreferredCamera = EAshlineCameraMode::FirstPerson;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	TArray<FAshlineMissionProgress> Missions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	TArray<FAshlineOwnedWeapon> Armory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	FAshlineLoadoutSlot Primary;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	FAshlineLoadoutSlot Secondary;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	int32 Credits = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	TArray<FName> OwnedCosmeticIds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	TArray<FName> OwnedSkinIds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	int32 CrateTokens = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	int32 CratesOpened = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	TArray<FAshlineCrateGrant> CollectedCosmetics;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	int32 PrestigeLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ashline|Save")
	int32 MissionsCompleted = 0;

	static constexpr const TCHAR* SlotName = TEXT("AshlineCampaign");
	static constexpr int32 UserIndex = 0;

	void SeedNewCampaign();
	void MigrateIfNeeded();
	FAshlineMissionProgress* FindMissionProgress(EAshlineMissionId MissionId);
	const FAshlineMissionProgress* FindMissionProgress(EAshlineMissionId MissionId) const;
	void UnlockNext(EAshlineMissionId Completed);
	void GrantXP(int32 Amount);
	bool OwnsCosmetic(FName CosmeticId) const;
	bool OwnsSkin(FName SkinId) const;
};
