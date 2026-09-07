#include "Progression/AshlineProgressionSubsystem.h"

#include "Ashline.h"
#include "Campaign/AshlineMissionCatalog.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/AshlineWeaponCatalog.h"

void UAshlineProgressionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadOrCreate();
}

void UAshlineProgressionSubsystem::LoadOrCreate()
{
	if (USaveGame* Existing = UGameplayStatics::LoadGameFromSlot(UAshlineSaveGame::SlotName, UAshlineSaveGame::UserIndex))
	{
		Save = Cast<UAshlineSaveGame>(Existing);
	}

	if (!Save)
	{
		Save = Cast<UAshlineSaveGame>(UGameplayStatics::CreateSaveGameObject(UAshlineSaveGame::StaticClass()));
		if (Save)
		{
			Save->SeedNewCampaign();
			SaveCampaign();
		}
	}

	OnSaveLoaded.Broadcast(Save);
}

void UAshlineProgressionSubsystem::SaveCampaign()
{
	if (!Save)
	{
		return;
	}
	UGameplayStatics::SaveGameToSlot(Save, UAshlineSaveGame::SlotName, UAshlineSaveGame::UserIndex);
	UE_LOG(LogAshline, Log, TEXT("Campaign saved (missions complete=%d prestige=%d rank=%d)."),
		Save->MissionsCompleted, Save->PrestigeLevel, Save->Operator.Rank);
}

void UAshlineProgressionSubsystem::CompleteMission(EAshlineMissionId MissionId, int32 Stars, bool bOptionalComplete)
{
	if (!Save)
	{
		return;
	}

	FAshlineMissionDefinition Def;
	if (!UAshlineMissionCatalog::FindMission(MissionId, Def))
	{
		return;
	}

	if (FAshlineMissionProgress* Progress = Save->FindMissionProgress(MissionId))
	{
		Progress->BestStars = FMath::Max(Progress->BestStars, FMath::Clamp(Stars, 0, 3));
		Progress->bOptionalComplete = Progress->bOptionalComplete || bOptionalComplete;
	}

	Save->UnlockNext(MissionId);
	Save->GrantXP(Def.XPReward + Stars * 50);
	Save->CrateTokens += Def.CrateTokensReward;

	// Unlock weapons that have become available by rank.
	for (const FAshlineWeaponDefinition& Weapon : UAshlineWeaponCatalog::BuildRoster())
	{
		if (Weapon.UnlockLevel <= Save->Operator.Rank && !FindOwned(Weapon.WeaponId))
		{
			FAshlineOwnedWeapon Owned;
			Owned.WeaponId = Weapon.WeaponId;
			Save->Armory.Add(Owned);
		}
	}

	SaveCampaign();
}

void UAshlineProgressionSubsystem::ApplyOperator(const FAshlineOperatorProfile& Profile)
{
	if (!Save)
	{
		return;
	}
	const int32 Prestige = Save->Operator.Prestige;
	const int32 Rank = Save->Operator.Rank;
	const int32 XP = Save->Operator.XP;
	Save->Operator = Profile;
	Save->Operator.Prestige = Prestige;
	Save->Operator.Rank = Rank;
	Save->Operator.XP = XP;
	SaveCampaign();
}

bool UAshlineProgressionSubsystem::EquipWeapon(bool bPrimary, FName WeaponId)
{
	if (!Save || !FindOwned(WeaponId))
	{
		return false;
	}
	FAshlineLoadoutSlot& Slot = bPrimary ? Save->Primary : Save->Secondary;
	Slot.WeaponId = WeaponId;
	if (const FAshlineOwnedWeapon* Owned = FindOwned(WeaponId))
	{
		Slot.Attachments = Owned->EquippedAttachments;
	}
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::EquipAttachment(bool bPrimary, EAshlineAttachmentSlot Slot, FName AttachmentId)
{
	if (!Save)
	{
		return false;
	}

	FAshlineLoadoutSlot& Loadout = bPrimary ? Save->Primary : Save->Secondary;
	FAshlineWeaponDefinition Weapon;
	if (!UAshlineWeaponCatalog::FindWeapon(Loadout.WeaponId, Weapon))
	{
		return false;
	}
	if (!Weapon.CompatibleAttachments.Contains(AttachmentId))
	{
		return false;
	}
	if (FAshlineOwnedWeapon* Owned = FindOwned(Loadout.WeaponId))
	{
		if (!Owned->UnlockedAttachments.Contains(AttachmentId))
		{
			Owned->UnlockedAttachments.Add(AttachmentId);
		}
		Owned->EquippedAttachments.Add(Slot, AttachmentId);
	}
	Loadout.Attachments.Add(Slot, AttachmentId);
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::UpgradeWeapon(FName WeaponId)
{
	FAshlineOwnedWeapon* Owned = FindOwned(WeaponId);
	if (!Owned || Owned->UpgradeTier >= 5)
	{
		return false;
	}
	++Owned->UpgradeTier;
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::PrestigeReset()
{
	if (!Save || Save->Operator.Rank < 50)
	{
		return false;
	}

	++Save->PrestigeLevel;
	Save->Operator.Prestige = Save->PrestigeLevel;
	Save->Operator.Rank = 1;
	Save->Operator.XP = 0;

	// Keep cosmetics and crate history; reset weapon upgrades except starters.
	for (FAshlineOwnedWeapon& Owned : Save->Armory)
	{
		Owned.UpgradeTier = 0;
		if (Owned.WeaponId != FName(TEXT("WPN_AR_ASH16")) && Owned.WeaponId != FName(TEXT("WPN_PIS_M17A")))
		{
			Owned.UnlockedAttachments.Reset();
			Owned.EquippedAttachments.Reset();
		}
	}

	SaveCampaign();
	return true;
}

FAshlineCrateGrant UAshlineProgressionSubsystem::OpenPlayEarnedCrate()
{
	FAshlineCrateGrant Grant;
	if (!Save || Save->CrateTokens <= 0)
	{
		Grant.DisplayName = FText::FromString(TEXT("No crate tokens"));
		return Grant;
	}

	--Save->CrateTokens;
	++Save->CratesOpened;

	Grant.Rarity = RollRarity();
	switch (Grant.Rarity)
	{
	case EAshlineLootRarity::Common:
		Grant.ItemId = TEXT("CAMO_FIELD");
		Grant.DisplayName = FText::FromString(TEXT("Field Ash camo"));
		break;
	case EAshlineLootRarity::Uncommon:
		Grant.ItemId = TEXT("CAMO_NIGHT");
		Grant.DisplayName = FText::FromString(TEXT("Night Glass camo"));
		break;
	case EAshlineLootRarity::Rare:
		Grant.ItemId = TEXT("CHARM_WIRE");
		Grant.DisplayName = FText::FromString(TEXT("Wire Cut charm"));
		break;
	case EAshlineLootRarity::Epic:
		Grant.ItemId = TEXT("VOICE_HOLDFAST");
		Grant.DisplayName = FText::FromString(TEXT("Holdfast voice pack"));
		break;
	case EAshlineLootRarity::Legendary:
		Grant.ItemId = TEXT("CAMO_ASHLINE");
		Grant.DisplayName = FText::FromString(TEXT("Ashline terminus camo"));
		break;
	}

	Save->CollectedCosmetics.Add(Grant);
	SaveCampaign();
	OnCrateOpened.Broadcast(Grant);
	return Grant;
}

void UAshlineProgressionSubsystem::SetDifficulty(EAshlineDifficulty Difficulty)
{
	if (!Save)
	{
		return;
	}
	Save->Difficulty = Difficulty;
	SaveCampaign();
}

FAshlineOwnedWeapon* UAshlineProgressionSubsystem::FindOwned(FName WeaponId)
{
	if (!Save)
	{
		return nullptr;
	}
	for (FAshlineOwnedWeapon& Owned : Save->Armory)
	{
		if (Owned.WeaponId == WeaponId)
		{
			return &Owned;
		}
	}
	return nullptr;
}

EAshlineLootRarity UAshlineProgressionSubsystem::RollRarity()
{
	const float Roll = FMath::FRand();
	if (Roll < 0.02f)
	{
		return EAshlineLootRarity::Legendary;
	}
	if (Roll < 0.08f)
	{
		return EAshlineLootRarity::Epic;
	}
	if (Roll < 0.22f)
	{
		return EAshlineLootRarity::Rare;
	}
	if (Roll < 0.50f)
	{
		return EAshlineLootRarity::Uncommon;
	}
	return EAshlineLootRarity::Common;
}
