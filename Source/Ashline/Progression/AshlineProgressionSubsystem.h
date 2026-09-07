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

	UFUNCTION(BlueprintCallable, Category = "Ashline|Difficulty")
	void SetDifficulty(EAshlineDifficulty Difficulty);

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
