#include "Progression/AshlineSaveGame.h"
#include "Campaign/AshlineMissionCatalog.h"
#include "Meta/AshlineMetaCatalog.h"
#include "Weapons/AshlineWeaponCatalog.h"

void UAshlineSaveGame::SeedNewCampaign()
{
	Operator = FAshlineOperatorProfile();
	Difficulty = EAshlineDifficulty::Regular;
	PreferredCamera = EAshlineCameraMode::FirstPerson;
	Credits = UAshlineMetaCatalog::StartingCredits;
	CrateTokens = 0;
	CratesOpened = 0;
	PrestigeLevel = 0;
	MissionsCompleted = 0;
	CollectedCosmetics.Reset();
	OwnedCosmeticIds = UAshlineMetaCatalog::StarterCosmeticIds();
	OwnedSkinIds = UAshlineMetaCatalog::StarterSkinIds();
	Missions.Reset();
	Armory.Reset();
	SlotVersion = TEXT("2.0.0");

	Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Camo, TEXT("CAMO_FIELD"));
	Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Helmet, TEXT("HELM_PATROL"));
	Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Vest, TEXT("VEST_PLATE"));
	Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Pants, TEXT("PANT_FATIGUE"));
	Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Gloves, TEXT("GLOVE_NOMEX"));
	Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Boots, TEXT("BOOT_COMBAT"));
	Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Face, TEXT("FACE_00"));
	Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Voice, TEXT("VOICE_NEUTRAL"));
	Operator.CamoId = TEXT("CAMO_FIELD");
	Operator.VoicePack = TEXT("VOICE_NEUTRAL");

	const TArray<FAshlineMissionDefinition> Campaign = UAshlineMissionCatalog::BuildCampaign();
	for (const FAshlineMissionDefinition& Mission : Campaign)
	{
		FAshlineMissionProgress Progress;
		Progress.MissionId = Mission.MissionId;
		Progress.State = (Mission.MissionId == EAshlineMissionId::ASH_01_WireCut)
			? EAshlineMissionState::Available
			: EAshlineMissionState::Locked;
		Missions.Add(Progress);
	}

	auto Own = [this](const TCHAR* Id)
	{
		FAshlineOwnedWeapon W;
		W.WeaponId = Id;
		W.UpgradeTier = 0;
		W.EquippedSkinId = TEXT("SKIN_FACTORY");
		W.UnlockedSkins.Add(TEXT("SKIN_FACTORY"));
		W.UnlockedAttachments = UAshlineWeaponCatalog::StarterAttachmentIds(Id);
		Armory.Add(W);
	};
	Own(TEXT("WPN_AR_ASH16"));
	Own(TEXT("WPN_PIS_M17A"));

	Primary.WeaponId = TEXT("WPN_AR_ASH16");
	Primary.SkinId = TEXT("SKIN_FACTORY");
	Secondary.WeaponId = TEXT("WPN_PIS_M17A");
	Secondary.SkinId = TEXT("SKIN_FACTORY");
}

void UAshlineSaveGame::MigrateIfNeeded()
{
	if (SlotVersion.StartsWith(TEXT("2.")))
	{
		return;
	}

	if (Credits <= 0)
	{
		Credits = UAshlineMetaCatalog::StartingCredits;
	}
	for (const FName& Id : UAshlineMetaCatalog::StarterCosmeticIds())
	{
		OwnedCosmeticIds.AddUnique(Id);
	}
	for (const FName& Id : UAshlineMetaCatalog::StarterSkinIds())
	{
		OwnedSkinIds.AddUnique(Id);
	}
	for (FAshlineOwnedWeapon& Weapon : Armory)
	{
		if (Weapon.EquippedSkinId.IsNone())
		{
			Weapon.EquippedSkinId = TEXT("SKIN_FACTORY");
		}
		Weapon.UnlockedSkins.AddUnique(TEXT("SKIN_FACTORY"));
		for (const FName& AttachmentId : UAshlineWeaponCatalog::StarterAttachmentIds(Weapon.WeaponId))
		{
			Weapon.UnlockedAttachments.AddUnique(AttachmentId);
		}
	}
	if (Primary.SkinId.IsNone())
	{
		Primary.SkinId = TEXT("SKIN_FACTORY");
	}
	if (Secondary.SkinId.IsNone())
	{
		Secondary.SkinId = TEXT("SKIN_FACTORY");
	}

	auto RemapLegacy = [](FName& Id)
	{
		if (Id == TEXT("AshlineField")) { Id = TEXT("CAMO_FIELD"); }
		else if (Id == TEXT("NightGlass")) { Id = TEXT("CAMO_NIGHT"); }
		else if (Id == TEXT("DustMarket")) { Id = TEXT("CAMO_DUST"); }
		else if (Id == TEXT("Whiteout")) { Id = TEXT("CAMO_WHITEOUT"); }
		else if (Id == TEXT("AshlineTerminus")) { Id = TEXT("CAMO_ASHLINE"); }
		else if (Id == TEXT("Neutral")) { Id = TEXT("VOICE_NEUTRAL"); }
	};
	RemapLegacy(Operator.CamoId);
	RemapLegacy(Operator.VoicePack);
	if (Operator.CamoId.IsNone())
	{
		Operator.CamoId = TEXT("CAMO_FIELD");
	}
	if (Operator.VoicePack.IsNone())
	{
		Operator.VoicePack = TEXT("VOICE_NEUTRAL");
	}
	if (Operator.EquippedCosmetics.Num() == 0)
	{
		Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Camo, Operator.CamoId.IsNone() ? FName(TEXT("CAMO_FIELD")) : Operator.CamoId);
		Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Helmet, TEXT("HELM_PATROL"));
		Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Vest, TEXT("VEST_PLATE"));
		Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Pants, TEXT("PANT_FATIGUE"));
		Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Gloves, TEXT("GLOVE_NOMEX"));
		Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Boots, TEXT("BOOT_COMBAT"));
		Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Face, TEXT("FACE_00"));
		Operator.EquippedCosmetics.Add(EAshlineCosmeticSlot::Voice, Operator.VoicePack);
	}

	SlotVersion = TEXT("2.0.0");
}

FAshlineMissionProgress* UAshlineSaveGame::FindMissionProgress(EAshlineMissionId MissionId)
{
	return const_cast<FAshlineMissionProgress*>(
		static_cast<const UAshlineSaveGame*>(this)->FindMissionProgress(MissionId));
}

const FAshlineMissionProgress* UAshlineSaveGame::FindMissionProgress(EAshlineMissionId MissionId) const
{
	for (const FAshlineMissionProgress& Progress : Missions)
	{
		if (Progress.MissionId == MissionId)
		{
			return &Progress;
		}
	}
	return nullptr;
}

void UAshlineSaveGame::UnlockNext(EAshlineMissionId Completed)
{
	if (FAshlineMissionProgress* Current = FindMissionProgress(Completed))
	{
		Current->State = EAshlineMissionState::Completed;
	}

	MissionsCompleted = FMath::Max(MissionsCompleted, UAshlineMissionCatalog::MissionIndex(Completed) + 1);

	const EAshlineMissionId Next = UAshlineMissionCatalog::NextMission(Completed);
	if (FAshlineMissionProgress* NextProgress = FindMissionProgress(Next))
	{
		if (NextProgress->State == EAshlineMissionState::Locked)
		{
			NextProgress->State = EAshlineMissionState::Available;
		}
	}
}

void UAshlineSaveGame::GrantXP(int32 Amount)
{
	Operator.XP += FMath::Max(0, Amount);
	while (Operator.Rank < UAshlineMetaCatalog::MaxRank)
	{
		const int32 Need = UAshlineMetaCatalog::XPToNextRank(Operator.Rank);
		if (Need <= 0 || Operator.XP < Need)
		{
			break;
		}
		Operator.XP -= Need;
		++Operator.Rank;
		const FAshlineRankTier Tier = UAshlineMetaCatalog::RankTier(Operator.Rank);
		Credits += Tier.CreditGrant;
		CrateTokens += Tier.CrateGrant;
		if (!Tier.UnlockId.IsNone())
		{
			OwnedCosmeticIds.AddUnique(Tier.UnlockId);
		}
	}
}

bool UAshlineSaveGame::OwnsCosmetic(FName CosmeticId) const
{
	return OwnedCosmeticIds.Contains(CosmeticId);
}

bool UAshlineSaveGame::OwnsSkin(FName SkinId) const
{
	return OwnedSkinIds.Contains(SkinId);
}
