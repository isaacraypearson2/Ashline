#include "Progression/AshlineProgressionSubsystem.h"

#include "Ashline.h"
#include "Campaign/AshlineMissionCatalog.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Meta/AshlineMetaCatalog.h"
#include "Meta/AshlineEquipmentCatalog.h"
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
		if (Save)
		{
			Save->MigrateIfNeeded();
			SaveCampaign();
		}
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
	LastAwardedCredits = UAshlineMetaCatalog::MissionCreditReward(Def.XPReward, Stars, Save->Difficulty);
	Save->Credits += LastAwardedCredits;

	// Unlock free service weapons that have become available by rank.
	for (const FAshlineWeaponDefinition& Weapon : UAshlineWeaponCatalog::BuildRoster())
	{
		if (Weapon.UnlockLevel <= Save->Operator.Rank
			&& Weapon.RequiredPrestige <= Save->PrestigeLevel
			&& Weapon.CreditCost == 0
			&& !FindOwned(Weapon.WeaponId))
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
		Slot.SkinId = Owned->EquippedSkinId;
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

bool UAshlineProgressionSubsystem::PurchaseWeapon(FName WeaponId)
{
	if (!Save)
	{
		return false;
	}
	if (FindOwned(WeaponId))
	{
		return true;
	}
	FAshlineWeaponDefinition Def;
	if (!UAshlineWeaponCatalog::FindWeapon(WeaponId, Def))
	{
		return false;
	}
	if (Save->Operator.Rank < Def.UnlockLevel || Save->PrestigeLevel < Def.RequiredPrestige)
	{
		return false;
	}
	if (!SpendCredits(Def.CreditCost))
	{
		return false;
	}
	FAshlineOwnedWeapon Owned;
	Owned.WeaponId = WeaponId;
	Owned.EquippedSkinId = TEXT("SKIN_FACTORY");
	Owned.UnlockedSkins.Add(TEXT("SKIN_FACTORY"));
	Save->Armory.Add(Owned);
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::PurchaseAttachment(FName WeaponId, FName AttachmentId)
{
	FAshlineOwnedWeapon* Owned = FindOwned(WeaponId);
	FAshlineWeaponDefinition Weapon;
	FAshlineAttachmentDefinition Attachment;
	if (!Save || !Owned || !UAshlineWeaponCatalog::FindWeapon(WeaponId, Weapon) || !UAshlineWeaponCatalog::FindAttachment(AttachmentId, Attachment))
	{
		return false;
	}
	if (!Weapon.CompatibleAttachments.Contains(AttachmentId))
	{
		return false;
	}
	if (Owned->UnlockedAttachments.Contains(AttachmentId))
	{
		return true;
	}
	if (Save->Operator.Rank < Attachment.UnlockLevel)
	{
		return false;
	}
	const int32 Cost = UAshlineWeaponCatalog::AttachmentUnlockCost(Attachment);
	if (!SpendCredits(Cost))
	{
		return false;
	}
	Owned->UnlockedAttachments.AddUnique(AttachmentId);
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::PurchaseEquipment(FName EquipmentId)
{
	if (!Save)
	{
		return false;
	}
	FAshlineEquipmentDefinition Def;
	if (!UAshlineEquipmentCatalog::FindEquipment(EquipmentId, Def))
	{
		return false;
	}
	if (Save->OwnsEquipment(EquipmentId))
	{
		return true;
	}
	if (Save->Operator.Rank < Def.UnlockRank || Save->PrestigeLevel < Def.RequiredPrestige)
	{
		return false;
	}
	if (!SpendCredits(Def.CreditCost))
	{
		return false;
	}
	Save->OwnedEquipmentIds.AddUnique(EquipmentId);
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::EquipEquipment(EAshlineEquipmentSlot Slot, FName EquipmentId)
{
	if (!Save || !Save->OwnsEquipment(EquipmentId))
	{
		return false;
	}
	FAshlineEquipmentDefinition Def;
	if (!UAshlineEquipmentCatalog::FindEquipment(EquipmentId, Def) || Def.Slot != Slot)
	{
		return false;
	}
	if (Slot == EAshlineEquipmentSlot::Lethal)
	{
		Save->Operator.LethalId = EquipmentId;
	}
	else if (Slot == EAshlineEquipmentSlot::Tactical)
	{
		Save->Operator.TacticalId = EquipmentId;
	}
	else if (Slot == EAshlineEquipmentSlot::Field)
	{
		Save->Operator.FieldId = EquipmentId;
	}
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::UpgradeWeapon(FName WeaponId)
{
	FAshlineOwnedWeapon* Owned = FindOwned(WeaponId);
	if (!Owned || Owned->UpgradeTier >= UAshlineWeaponCatalog::MaxUpgradeTier)
	{
		return false;
	}
	const int32 Cost = [&]()
	{
		FAshlineWeaponDefinition Def;
		if (UAshlineWeaponCatalog::FindWeapon(WeaponId, Def))
		{
			return UAshlineMetaCatalog::WeaponUpgradeCostForClass(Def.Class, Owned->UpgradeTier);
		}
		return UAshlineMetaCatalog::WeaponUpgradeCost(Owned->UpgradeTier);
	}();
	if (!SpendCredits(Cost))
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
	Save->OwnedCosmeticIds.AddUnique(TEXT("CAMO_PRESTIGE"));
	Save->OwnedSkinIds.AddUnique(TEXT("SKIN_GOLD"));
	Save->OwnedCosmeticIds.AddUnique(TEXT("CHARM_SPINE"));
	if (Save->PrestigeLevel >= 2)
	{
		Save->OwnedCosmeticIds.AddUnique(TEXT("CAMO_SPECTRE"));
		Save->OwnedCosmeticIds.AddUnique(TEXT("CAMO_DIAMOND"));
		Save->OwnedCosmeticIds.AddUnique(TEXT("CHARM_DIAMOND"));
		Save->OwnedSkinIds.AddUnique(TEXT("SKIN_VOID"));
		Save->OwnedSkinIds.AddUnique(TEXT("SKIN_DIAMOND"));
		Save->OwnedEquipmentIds.AddUnique(TEXT("EQ_HEARTBEAT"));
	}
	Save->Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Camo, TEXT("CAMO_PRESTIGE"));
	Save->Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Charm, TEXT("CHARM_SPINE"));
	Save->Operator.CamoId = TEXT("CAMO_PRESTIGE");
	Save->Operator.EquippedCharm = TEXT("CHARM_SPINE");

	// Keep cosmetics and crate history; reset weapon upgrades except starters.
	for (FAshlineOwnedWeapon& Owned : Save->Armory)
	{
		Owned.UpgradeTier = 0;
		Owned.UnlockedSkins.AddUnique(TEXT("SKIN_GOLD"));
		Owned.EquippedSkinId = TEXT("SKIN_GOLD");
		if (Owned.WeaponId != FName(TEXT("WPN_AR_ASH16")) && Owned.WeaponId != FName(TEXT("WPN_PIS_M17A")))
		{
			Owned.UnlockedAttachments.Reset();
			Owned.EquippedAttachments.Reset();
		}
	}
	Save->Primary.SkinId = TEXT("SKIN_GOLD");
	Save->Secondary.SkinId = TEXT("SKIN_GOLD");

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
	TArray<FName> Pool = UAshlineMetaCatalog::CratePoolIds(Grant.Rarity);
	if (Pool.Num() == 0)
	{
		Pool = UAshlineMetaCatalog::CratePoolIds(EAshlineLootRarity::Uncommon);
	}
	if (Pool.Num() > 0)
	{
		Grant.ItemId = Pool[FMath::RandHelper(Pool.Num())];
		FAshlineCosmeticDefinition Cosmetic;
		FAshlineWeaponSkinDefinition Skin;
		if (UAshlineMetaCatalog::FindCosmetic(Grant.ItemId, Cosmetic))
		{
			Grant.DisplayName = Cosmetic.DisplayName;
			Save->OwnedCosmeticIds.AddUnique(Grant.ItemId);
		}
		else if (UAshlineMetaCatalog::FindSkin(Grant.ItemId, Skin))
		{
			Grant.DisplayName = Skin.DisplayName;
			Save->OwnedSkinIds.AddUnique(Grant.ItemId);
		}
		else
		{
			FAshlineEquipmentDefinition Equipment;
			if (UAshlineEquipmentCatalog::FindEquipment(Grant.ItemId, Equipment))
			{
				Grant.DisplayName = Equipment.DisplayName;
				Save->OwnedEquipmentIds.AddUnique(Grant.ItemId);
			}
			else
			{
				Grant.DisplayName = FText::FromName(Grant.ItemId);
			}
		}
	}
	else
	{
		Grant.ItemId = TEXT("CAMO_FIELD");
		Grant.DisplayName = FText::FromString(TEXT("Field Ash camo"));
	}

	Save->CollectedCosmetics.Add(Grant);
	SaveCampaign();
	OnCrateOpened.Broadcast(Grant);
	return Grant;
}

bool UAshlineProgressionSubsystem::GrantCredits(int32 Amount)
{
	if (!Save || Amount <= 0)
	{
		return false;
	}
	Save->Credits += Amount;
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::SpendCredits(int32 Amount)
{
	if (!Save || Amount < 0 || Save->Credits < Amount)
	{
		return false;
	}
	Save->Credits -= Amount;
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::PurchaseCosmetic(FName CosmeticId)
{
	if (!Save)
	{
		return false;
	}
	FAshlineCosmeticDefinition Def;
	if (!UAshlineMetaCatalog::FindCosmetic(CosmeticId, Def))
	{
		return false;
	}
	if (Save->OwnsCosmetic(CosmeticId))
	{
		return true;
	}
	if (Save->Operator.Rank < Def.UnlockRank || Save->PrestigeLevel < Def.RequiredPrestige)
	{
		return false;
	}
	if (!SpendCredits(Def.CreditCost))
	{
		return false;
	}
	Save->OwnedCosmeticIds.AddUnique(CosmeticId);
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::EquipCosmetic(EAshlineCosmeticSlot Slot, FName CosmeticId)
{
	if (!Save || !Save->OwnsCosmetic(CosmeticId))
	{
		return false;
	}
	FAshlineCosmeticDefinition Def;
	if (!UAshlineMetaCatalog::FindCosmetic(CosmeticId, Def) || Def.Slot != Slot)
	{
		return false;
	}
	Save->Operator.EquippedCosmetics.Add(Slot, CosmeticId);
	if (Slot == EAshlineCosmeticSlot::Camo)
	{
		Save->Operator.CamoId = CosmeticId;
	}
	if (Slot == EAshlineCosmeticSlot::Voice)
	{
		Save->Operator.VoicePack = CosmeticId;
	}
	if (Slot == EAshlineCosmeticSlot::Charm)
	{
		Save->Operator.EquippedCharm = CosmeticId;
	}
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::PurchaseSkin(FName SkinId)
{
	if (!Save)
	{
		return false;
	}
	FAshlineWeaponSkinDefinition Def;
	if (!UAshlineMetaCatalog::FindSkin(SkinId, Def))
	{
		return false;
	}
	if (Save->OwnsSkin(SkinId))
	{
		return true;
	}
	if (Save->Operator.Rank < Def.UnlockRank || Save->PrestigeLevel < Def.RequiredPrestige)
	{
		return false;
	}
	if (!SpendCredits(Def.CreditCost))
	{
		return false;
	}
	Save->OwnedSkinIds.AddUnique(SkinId);
	if (!Def.WeaponId.IsNone())
	{
		if (FAshlineOwnedWeapon* Owned = FindOwned(Def.WeaponId))
		{
			Owned->UnlockedSkins.AddUnique(SkinId);
		}
	}
	else
	{
		for (FAshlineOwnedWeapon& Owned : Save->Armory)
		{
			Owned.UnlockedSkins.AddUnique(SkinId);
		}
	}
	SaveCampaign();
	return true;
}

bool UAshlineProgressionSubsystem::EquipSkin(FName WeaponId, FName SkinId)
{
	FAshlineOwnedWeapon* Owned = FindOwned(WeaponId);
	FAshlineWeaponSkinDefinition Def;
	if (!Owned || !Save || !Save->OwnsSkin(SkinId) || !UAshlineMetaCatalog::FindSkin(SkinId, Def))
	{
		return false;
	}
	if (!UAshlineMetaCatalog::SkinFitsWeapon(Def, WeaponId))
	{
		return false;
	}
	Owned->EquippedSkinId = SkinId;
	Owned->UnlockedSkins.AddUnique(SkinId);
	if (Save->Primary.WeaponId == WeaponId)
	{
		Save->Primary.SkinId = SkinId;
	}
	if (Save->Secondary.WeaponId == WeaponId)
	{
		Save->Secondary.SkinId = SkinId;
	}
	SaveCampaign();
	return true;
}

int32 UAshlineProgressionSubsystem::GetCredits() const
{
	return Save ? Save->Credits : 0;
}

TArray<FName> UAshlineProgressionSubsystem::GetOwnedCosmetics() const
{
	return Save ? Save->OwnedCosmeticIds : TArray<FName>();
}

TArray<FName> UAshlineProgressionSubsystem::GetOwnedSkins() const
{
	return Save ? Save->OwnedSkinIds : TArray<FName>();
}

bool UAshlineProgressionSubsystem::SetRank(int32 Rank)
{
	if (!Save)
	{
		return false;
	}
	Save->Operator.Rank = FMath::Clamp(Rank, 1, UAshlineMetaCatalog::MaxRank);
	SaveCampaign();
	return true;
}

void UAshlineProgressionSubsystem::UnlockAllMeta()
{
	if (!Save)
	{
		return;
	}
	Save->Credits = FMath::Max(Save->Credits, 50000);
	Save->CrateTokens = FMath::Max(Save->CrateTokens, 8);
	Save->Operator.Rank = UAshlineMetaCatalog::MaxRank;
	for (const FAshlineCosmeticDefinition& Item : UAshlineMetaCatalog::BuildCosmetics())
	{
		if (Item.RequiredPrestige <= Save->PrestigeLevel)
		{
			Save->OwnedCosmeticIds.AddUnique(Item.CosmeticId);
		}
	}
	for (const FAshlineWeaponSkinDefinition& Item : UAshlineMetaCatalog::BuildWeaponSkins())
	{
		if (Item.RequiredPrestige <= Save->PrestigeLevel)
		{
			Save->OwnedSkinIds.AddUnique(Item.SkinId);
			if (Item.WeaponId.IsNone())
			{
				for (FAshlineOwnedWeapon& Owned : Save->Armory)
				{
					Owned.UnlockedSkins.AddUnique(Item.SkinId);
				}
			}
			else if (FAshlineOwnedWeapon* Owned = FindOwned(Item.WeaponId))
			{
				Owned->UnlockedSkins.AddUnique(Item.SkinId);
			}
		}
	}
	for (const FAshlineWeaponDefinition& Weapon : UAshlineWeaponCatalog::BuildRoster())
	{
		if (Weapon.RequiredPrestige <= Save->PrestigeLevel && !FindOwned(Weapon.WeaponId))
		{
			FAshlineOwnedWeapon Owned;
			Owned.WeaponId = Weapon.WeaponId;
			Owned.EquippedSkinId = TEXT("SKIN_FACTORY");
			Owned.UnlockedSkins.Add(TEXT("SKIN_FACTORY"));
			Save->Armory.Add(Owned);
		}
	}
	for (const FAshlineEquipmentDefinition& Item : UAshlineEquipmentCatalog::BuildRoster())
	{
		if (Item.RequiredPrestige <= Save->PrestigeLevel)
		{
			Save->OwnedEquipmentIds.AddUnique(Item.EquipmentId);
		}
	}
	SaveCampaign();
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
