#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AshlineTypes.h"
#include "Progression/AshlineSaveGame.h"
#include "AshlineProgressionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshlineSaveLoaded, UAshlineSaveGame*, Save);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshlineCrateOpened, const FAshlineCrateGrant&, Grant);

UCLASS()
class ASHLINE_API UAshlineProgressionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Save")
	UAshlineSaveGame* GetSave() const { return Save; }

	UFUNCTION(BlueprintCallable, Category = "Ashline|Save")
	void LoadOrCreate();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Save")
	void SaveCampaign();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void CompleteMission(EAshlineMissionId MissionId, int32 Stars, bool bOptionalComplete);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Operator")
	void ApplyOperator(const FAshlineOperatorProfile& Profile);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Armory")
	bool EquipWeapon(bool bPrimary, FName WeaponId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Armory")
	bool EquipAttachment(bool bPrimary, EAshlineAttachmentSlot Slot, FName AttachmentId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Armory")
	bool UpgradeWeapon(FName WeaponId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Prestige")
	bool PrestigeReset();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Loot")
	FAshlineCrateGrant OpenPlayEarnedCrate();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Economy")
	bool GrantCredits(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Economy")
	bool SpendCredits(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	bool PurchaseCosmetic(FName CosmeticId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	bool EquipCosmetic(EAshlineCosmeticSlot Slot, FName CosmeticId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	bool PurchaseSkin(FName SkinId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	bool EquipSkin(FName WeaponId, FName SkinId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Economy")
	int32 GetCredits() const;

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	TArray<FName> GetOwnedCosmetics() const;

	UFUNCTION(BlueprintPure, Category = "Ashline|Meta")
	TArray<FName> GetOwnedSkins() const;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	bool SetRank(int32 Rank);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Meta")
	void UnlockAllMeta();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Difficulty")
	void SetDifficulty(EAshlineDifficulty Difficulty);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Economy")
	int32 LastAwardedCredits = 0;

	UPROPERTY(BlueprintAssignable, Category = "Ashline|Save")
	FAshlineSaveLoaded OnSaveLoaded;

	UPROPERTY(BlueprintAssignable, Category = "Ashline|Loot")
	FAshlineCrateOpened OnCrateOpened;

private:
	UPROPERTY()
	TObjectPtr<UAshlineSaveGame> Save;

	FAshlineOwnedWeapon* FindOwned(FName WeaponId);
	static EAshlineLootRarity RollRarity();
};
