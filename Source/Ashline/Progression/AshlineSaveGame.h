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
	FString SlotVersion = TEXT("1.0.0");

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
	FAshlineMissionProgress* FindMissionProgress(EAshlineMissionId MissionId);
	void UnlockNext(EAshlineMissionId Completed);
	void GrantXP(int32 Amount);
};
