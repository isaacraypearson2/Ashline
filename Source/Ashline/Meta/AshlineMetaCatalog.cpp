#include "Meta/AshlineMetaCatalog.h"
#include "Presentation/AshlineLoad.h"

#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlineCosmeticVisual.h"
#include "Presentation/AshlineLoad.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/SoftObjectPath.h"

namespace AshlineMeta
{
	static FAshlineCosmeticDefinition C(
		const TCHAR* Id,
		const TCHAR* Name,
		EAshlineCosmeticSlot Slot,
		EAshlineLootRarity Rarity,
		int32 Rank,
		int32 Cost,
		int32 Prestige,
		bool bStarter,
		const FLinearColor& Tint)
	{
		FAshlineCosmeticDefinition D;
		D.CosmeticId = Id;
		D.DisplayName = FText::FromString(Name);
		D.Slot = Slot;
		D.Rarity = Rarity;
		D.UnlockRank = Rank;
		D.CreditCost = Cost;
		D.RequiredPrestige = Prestige;
		D.bStarter = bStarter;
		D.PreviewTint = Tint;
		UAshlineMetaCatalog::BindCosmeticContentPaths(D);
		return D;
	}

	static FAshlineWeaponSkinDefinition S(
		const TCHAR* Id,
		const TCHAR* Name,
		const TCHAR* Weapon,
		EAshlineLootRarity Rarity,
		int32 Rank,
		int32 Cost,
		int32 Prestige,
		bool bStarter,
		const FLinearColor& Tint)
	{
		FAshlineWeaponSkinDefinition D;
		D.SkinId = Id;
		D.DisplayName = FText::FromString(Name);
		D.WeaponId = Weapon;
		D.Rarity = Rarity;
		D.UnlockRank = Rank;
		D.CreditCost = Cost;
		D.RequiredPrestige = Prestige;
		D.bStarter = bStarter;
		D.Tint = Tint;
		UAshlineMetaCatalog::BindSkinContentPaths(D);
		return D;
	}
}

TArray<FAshlineCosmeticDefinition> UAshlineMetaCatalog::BuildCosmetics()
{
	using namespace AshlineMeta;
	TArray<FAshlineCosmeticDefinition> List;
	List.Reserve(48);

	List.Add(C(TEXT("CAMO_FIELD"), TEXT("Field Ash"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.22f, 0.26f, 0.16f)));
	List.Add(C(TEXT("CAMO_NIGHT"), TEXT("Night Glass"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Uncommon, 6, 400, 0, false, FLinearColor(0.08f, 0.1f, 0.16f)));
	List.Add(C(TEXT("CAMO_DUST"), TEXT("Dust Market"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Uncommon, 8, 450, 0, false, FLinearColor(0.42f, 0.3f, 0.14f)));
	List.Add(C(TEXT("CAMO_WHITEOUT"), TEXT("Whiteout"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Rare, 18, 900, 0, false, FLinearColor(0.78f, 0.82f, 0.88f)));
	List.Add(C(TEXT("CAMO_ASHLINE"), TEXT("Terminus"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Legendary, 40, 2400, 0, false, FLinearColor(0.45f, 0.08f, 0.06f)));
	List.Add(C(TEXT("CAMO_PRESTIGE"), TEXT("Prestige Ash"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Epic, 1, 0, 1, false, FLinearColor(0.72f, 0.62f, 0.22f)));
	List.Add(C(TEXT("CAMO_URBAN"), TEXT("Urban ash"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Uncommon, 10, 500, 0, false, FLinearColor(0.28f, 0.28f, 0.3f)));
	List.Add(C(TEXT("CAMO_WOODLAND"), TEXT("Woodland"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Common, 3, 200, 0, false, FLinearColor(0.18f, 0.28f, 0.12f)));
	List.Add(C(TEXT("CAMO_TIGER"), TEXT("Tiger stripe"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Rare, 14, 750, 0, false, FLinearColor(0.32f, 0.22f, 0.08f)));

	List.Add(C(TEXT("HELM_PATROL"), TEXT("Patrol cap"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.16f, 0.18f, 0.12f)));
	List.Add(C(TEXT("HELM_FAST"), TEXT("FAST helmet"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Rare, 12, 700, 0, false, FLinearColor(0.1f, 0.1f, 0.1f)));
	List.Add(C(TEXT("HELM_BOONIE"), TEXT("Boonie"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Uncommon, 5, 350, 0, false, FLinearColor(0.28f, 0.26f, 0.14f)));
	List.Add(C(TEXT("HELM_BALLISTIC"), TEXT("Ballistic visor"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Epic, 22, 1100, 0, false, FLinearColor(0.08f, 0.09f, 0.1f)));
	List.Add(C(TEXT("HELM_BEANIE"), TEXT("Watch cap"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Common, 2, 120, 0, false, FLinearColor(0.12f, 0.12f, 0.1f)));
	List.Add(C(TEXT("VEST_PLATE"), TEXT("Plate carrier"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.14f, 0.16f, 0.12f)));
	List.Add(C(TEXT("VEST_HEAVY"), TEXT("Heavy rig"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Rare, 20, 850, 0, false, FLinearColor(0.12f, 0.1f, 0.08f)));
	List.Add(C(TEXT("VEST_RECON"), TEXT("Recon vest"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Uncommon, 9, 500, 0, false, FLinearColor(0.18f, 0.16f, 0.1f)));
	List.Add(C(TEXT("VEST_CQB"), TEXT("CQB chest rig"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Rare, 15, 700, 0, false, FLinearColor(0.16f, 0.14f, 0.1f)));
	List.Add(C(TEXT("PANT_FATIGUE"), TEXT("Field fatigues"), EAshlineCosmeticSlot::Pants, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.2f, 0.22f, 0.14f)));
	List.Add(C(TEXT("PANT_CRYE"), TEXT("Crye cut"), EAshlineCosmeticSlot::Pants, EAshlineLootRarity::Uncommon, 11, 400, 0, false, FLinearColor(0.18f, 0.2f, 0.12f)));
	List.Add(C(TEXT("GLOVE_NOMEX"), TEXT("Nomex gloves"), EAshlineCosmeticSlot::Gloves, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.08f, 0.08f, 0.08f)));
	List.Add(C(TEXT("GLOVE_WINTER"), TEXT("Winter gloves"), EAshlineCosmeticSlot::Gloves, EAshlineLootRarity::Rare, 18, 450, 0, false, FLinearColor(0.22f, 0.24f, 0.26f)));
	List.Add(C(TEXT("BOOT_COMBAT"), TEXT("Combat boots"), EAshlineCosmeticSlot::Boots, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.1f, 0.08f, 0.06f)));
	List.Add(C(TEXT("BOOT_DESERT"), TEXT("Desert boots"), EAshlineCosmeticSlot::Boots, EAshlineLootRarity::Uncommon, 8, 300, 0, false, FLinearColor(0.32f, 0.24f, 0.12f)));

	List.Add(C(TEXT("FACE_00"), TEXT("Operator A"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.45f, 0.34f, 0.26f)));
	List.Add(C(TEXT("FACE_01"), TEXT("Operator B"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Common, 4, 150, 0, false, FLinearColor(0.38f, 0.28f, 0.2f)));
	List.Add(C(TEXT("FACE_02"), TEXT("Operator C"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Uncommon, 8, 220, 0, false, FLinearColor(0.42f, 0.3f, 0.22f)));
	List.Add(C(TEXT("FACE_03"), TEXT("Operator D"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Rare, 16, 400, 0, false, FLinearColor(0.3f, 0.22f, 0.18f)));
	List.Add(C(TEXT("VOICE_NEUTRAL"), TEXT("Neutral VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor::White));
	List.Add(C(TEXT("VOICE_HOLDFAST"), TEXT("Holdfast VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Epic, 16, 1200, 0, false, FLinearColor(0.8f, 0.7f, 0.4f)));
	List.Add(C(TEXT("VOICE_HARBOR"), TEXT("Harbor VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Rare, 22, 800, 0, false, FLinearColor(0.4f, 0.6f, 0.8f)));

	List.Add(C(TEXT("CHARM_WIRE"), TEXT("Wire Cut charm"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Rare, 2, 300, 0, false, FLinearColor(0.7f, 0.55f, 0.2f)));
	List.Add(C(TEXT("CHARM_SPINE"), TEXT("Spine charm"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Legendary, 50, 0, 1, false, FLinearColor(0.9f, 0.2f, 0.15f)));
	List.Add(C(TEXT("CHARM_DOG"), TEXT("Dog tag"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Uncommon, 6, 250, 0, false, FLinearColor(0.55f, 0.55f, 0.5f)));
	List.Add(C(TEXT("CHARM_COMPASS"), TEXT("Brass compass"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Rare, 12, 400, 0, false, FLinearColor(0.65f, 0.45f, 0.18f)));

	List.Add(C(TEXT("HEAD_COMTAC"), TEXT("ComTac headset"), EAshlineCosmeticSlot::Headset, EAshlineLootRarity::Uncommon, 7, 450, 0, false, FLinearColor(0.1f, 0.1f, 0.1f)));
	List.Add(C(TEXT("HEAD_PTT"), TEXT("PTT pack"), EAshlineCosmeticSlot::Headset, EAshlineLootRarity::Rare, 13, 600, 0, false, FLinearColor(0.12f, 0.14f, 0.1f)));
	List.Add(C(TEXT("PACK_ASSAULT"), TEXT("Assault pack"), EAshlineCosmeticSlot::Backpack, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.16f, 0.18f, 0.12f)));
	List.Add(C(TEXT("PACK_RADIO"), TEXT("Radio bag"), EAshlineCosmeticSlot::Backpack, EAshlineLootRarity::Rare, 18, 800, 0, false, FLinearColor(0.14f, 0.12f, 0.08f)));

	return List;
}

TArray<FAshlineWeaponSkinDefinition> UAshlineMetaCatalog::BuildWeaponSkins()
{
	using namespace AshlineMeta;
	TArray<FAshlineWeaponSkinDefinition> List;
	List.Reserve(16);

	List.Add(S(TEXT("SKIN_FACTORY"), TEXT("Factory black"), TEXT(""), EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.07f, 0.07f, 0.08f)));
	List.Add(S(TEXT("SKIN_FDE"), TEXT("FDE"), TEXT(""), EAshlineLootRarity::Uncommon, 5, 350, 0, false, FLinearColor(0.35f, 0.28f, 0.16f)));
	List.Add(S(TEXT("SKIN_OD"), TEXT("OD green"), TEXT(""), EAshlineLootRarity::Uncommon, 7, 350, 0, false, FLinearColor(0.16f, 0.2f, 0.1f)));
	List.Add(S(TEXT("SKIN_SNOW"), TEXT("Arctic"), TEXT(""), EAshlineLootRarity::Rare, 18, 800, 0, false, FLinearColor(0.75f, 0.78f, 0.82f)));
	List.Add(S(TEXT("SKIN_GOLD"), TEXT("Prestige gilt"), TEXT(""), EAshlineLootRarity::Legendary, 1, 0, 1, false, FLinearColor(0.72f, 0.58f, 0.18f)));

	List.Add(S(TEXT("SKIN_ASH16_NIGHT"), TEXT("ASH-16 Night Glass"), TEXT("WPN_AR_ASH16"), EAshlineLootRarity::Rare, 10, 600, 0, false, FLinearColor(0.05f, 0.08f, 0.14f)));
	List.Add(S(TEXT("SKIN_C9_DUST"), TEXT("C9 Dust"), TEXT("WPN_SMG_C9"), EAshlineLootRarity::Uncommon, 8, 400, 0, false, FLinearColor(0.4f, 0.28f, 0.12f)));
	List.Add(S(TEXT("SKIN_G28_GLASS"), TEXT("G28-L Glass"), TEXT("WPN_SNP_G28L"), EAshlineLootRarity::Epic, 24, 1400, 0, false, FLinearColor(0.12f, 0.14f, 0.18f)));
	List.Add(S(TEXT("SKIN_M870_BREACH"), TEXT("M870 Breacher"), TEXT("WPN_SHG_M870K"), EAshlineLootRarity::Rare, 14, 700, 0, false, FLinearColor(0.18f, 0.1f, 0.06f)));
	List.Add(S(TEXT("SKIN_M17_SIDE"), TEXT("M17 Sidearm"), TEXT("WPN_PIS_M17A"), EAshlineLootRarity::Uncommon, 3, 250, 0, false, FLinearColor(0.1f, 0.1f, 0.12f)));
	List.Add(S(TEXT("SKIN_SASS_MARK"), TEXT("SASS-7 Marks"), TEXT("WPN_DMR_SASS"), EAshlineLootRarity::Rare, 16, 750, 0, false, FLinearColor(0.14f, 0.12f, 0.08f)));
	List.Add(S(TEXT("SKIN_M250_SAW"), TEXT("M250 SAW"), TEXT("WPN_LMG_M250"), EAshlineLootRarity::Epic, 28, 1600, 0, false, FLinearColor(0.1f, 0.12f, 0.08f)));
	List.Add(S(TEXT("SKIN_M4C_TAN"), TEXT("M4-C Tan"), TEXT("WPN_AR_M4C"), EAshlineLootRarity::Uncommon, 6, 380, 0, false, FLinearColor(0.38f, 0.3f, 0.16f)));
	List.Add(S(TEXT("SKIN_P90_NIGHT"), TEXT("P90 Night"), TEXT("WPN_PDW_P90C"), EAshlineLootRarity::Rare, 12, 650, 0, false, FLinearColor(0.06f, 0.08f, 0.12f)));
	List.Add(S(TEXT("SKIN_FAL_WOOD"), TEXT("FAL Wood"), TEXT("WPN_BR_FAL"), EAshlineLootRarity::Rare, 14, 700, 0, false, FLinearColor(0.28f, 0.16f, 0.08f)));
	List.Add(S(TEXT("SKIN_AA12_URBAN"), TEXT("AA-12 Urban"), TEXT("WPN_SHG_AA12"), EAshlineLootRarity::Epic, 20, 1200, 0, false, FLinearColor(0.18f, 0.18f, 0.2f)));
	List.Add(S(TEXT("SKIN_1911_NICKEL"), TEXT("1911 Nickel"), TEXT("WPN_PIS_1911"), EAshlineLootRarity::Rare, 9, 500, 0, false, FLinearColor(0.55f, 0.55f, 0.5f)));
	List.Add(S(TEXT("SKIN_UMP_OD"), TEXT("UMP OD"), TEXT("WPN_SMG_UMP"), EAshlineLootRarity::Uncommon, 8, 420, 0, false, FLinearColor(0.14f, 0.18f, 0.1f)));

	return List;
}

TArray<FAshlineRankTier> UAshlineMetaCatalog::BuildRankTable()
{
	TArray<FAshlineRankTier> Table;
	Table.Reserve(MaxRank);
	for (int32 Rank = 1; Rank <= MaxRank; ++Rank)
	{
		FAshlineRankTier T;
		T.Rank = Rank;
		T.XPToNext = (Rank >= MaxRank) ? 0 : (700 + Rank * 70);
		T.CreditGrant = (Rank % 5 == 0) ? 200 : 50;
		T.CrateGrant = (Rank % 10 == 0) ? 1 : 0;
		if (Rank == 10)
		{
			T.UnlockId = TEXT("CAMO_NIGHT");
		}
		else if (Rank == 20)
		{
			T.UnlockId = TEXT("VEST_HEAVY");
		}
		else if (Rank == 40)
		{
			T.UnlockId = TEXT("CAMO_ASHLINE");
		}
		Table.Add(T);
	}
	return Table;
}

bool UAshlineMetaCatalog::FindCosmetic(FName CosmeticId, FAshlineCosmeticDefinition& OutCosmetic)
{
	for (const FAshlineCosmeticDefinition& Item : BuildCosmetics())
	{
		if (Item.CosmeticId == CosmeticId)
		{
			OutCosmetic = Item;
			OverlayCosmeticDataAsset(OutCosmetic);
			return true;
		}
	}
	return false;
}

bool UAshlineMetaCatalog::FindSkin(FName SkinId, FAshlineWeaponSkinDefinition& OutSkin)
{
	for (const FAshlineWeaponSkinDefinition& Item : BuildWeaponSkins())
	{
		if (Item.SkinId == SkinId)
		{
			OutSkin = Item;
			OverlaySkinDataAsset(OutSkin);
			return true;
		}
	}
	return false;
}

int32 UAshlineMetaCatalog::XPToNextRank(int32 Rank)
{
	return RankTier(Rank).XPToNext;
}

FAshlineRankTier UAshlineMetaCatalog::RankTier(int32 Rank)
{
	const int32 Clamped = FMath::Clamp(Rank, 1, MaxRank);
	const TArray<FAshlineRankTier> Table = BuildRankTable();
	if (Table.IsValidIndex(Clamped - 1))
	{
		return Table[Clamped - 1];
	}
	FAshlineRankTier Fallback;
	Fallback.Rank = Clamped;
	Fallback.XPToNext = 1000;
	return Fallback;
}

int32 UAshlineMetaCatalog::MissionCreditReward(int32 BaseXP, int32 Stars, EAshlineDifficulty Difficulty)
{
	float Mul = 1.f;
	switch (Difficulty)
	{
	case EAshlineDifficulty::Recruit: Mul = 0.75f; break;
	case EAshlineDifficulty::Veteran: Mul = 1.25f; break;
	case EAshlineDifficulty::Extreme: Mul = 1.6f; break;
	default: break;
	}
	return FMath::RoundToInt((BaseXP * 0.45f + Stars * 80.f) * Mul);
}

int32 UAshlineMetaCatalog::WeaponUpgradeCost(int32 CurrentTier)
{
	const int32 Tier = FMath::Clamp(CurrentTier, 0, 4);
	return 200 + Tier * 250;
}

bool UAshlineMetaCatalog::SkinFitsWeapon(const FAshlineWeaponSkinDefinition& Skin, FName WeaponId)
{
	return Skin.WeaponId.IsNone() || Skin.WeaponId == WeaponId;
}

TArray<FName> UAshlineMetaCatalog::StarterCosmeticIds()
{
	TArray<FName> Ids;
	for (const FAshlineCosmeticDefinition& Item : BuildCosmetics())
	{
		if (Item.bStarter)
		{
			Ids.Add(Item.CosmeticId);
		}
	}
	return Ids;
}

TArray<FName> UAshlineMetaCatalog::StarterSkinIds()
{
	TArray<FName> Ids;
	for (const FAshlineWeaponSkinDefinition& Item : BuildWeaponSkins())
	{
		if (Item.bStarter)
		{
			Ids.Add(Item.SkinId);
		}
	}
	return Ids;
}

FLinearColor UAshlineMetaCatalog::CosmeticTint(FName CosmeticId, FLinearColor Fallback)
{
	FAshlineCosmeticDefinition Def;
	return FindCosmetic(CosmeticId, Def) ? Def.PreviewTint : Fallback;
}

FLinearColor UAshlineMetaCatalog::SkinTint(FName SkinId, FLinearColor Fallback)
{
	FAshlineWeaponSkinDefinition Def;
	return FindSkin(SkinId, Def) ? Def.Tint : Fallback;
}

FName UAshlineMetaCatalog::EquippedCosmeticId(const FAshlineOperatorProfile& Profile, EAshlineCosmeticSlot Slot)
{
	if (const FName* Equipped = Profile.EquippedCosmetics.Find(Slot))
	{
		if (!Equipped->IsNone())
		{
			return *Equipped;
		}
	}
	if (Slot == EAshlineCosmeticSlot::Camo && !Profile.CamoId.IsNone())
	{
		return Profile.CamoId;
	}
	if (Slot == EAshlineCosmeticSlot::Voice && !Profile.VoicePack.IsNone())
	{
		return Profile.VoicePack;
	}
	if (Slot == EAshlineCosmeticSlot::Charm && !Profile.EquippedCharm.IsNone())
	{
		return Profile.EquippedCharm;
	}
	return NAME_None;
}

bool UAshlineMetaCatalog::SlotFromName(FName SlotName, EAshlineCosmeticSlot& OutSlot)
{
	const FString S = SlotName.ToString();
	if (S.Equals(TEXT("Helmet"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Helmet; return true; }
	if (S.Equals(TEXT("Vest"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Vest; return true; }
	if (S.Equals(TEXT("Pants"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Pants; return true; }
	if (S.Equals(TEXT("Gloves"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Gloves; return true; }
	if (S.Equals(TEXT("Boots"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Boots; return true; }
	if (S.Equals(TEXT("Camo"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Camo; return true; }
	if (S.Equals(TEXT("Face"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Face; return true; }
	if (S.Equals(TEXT("Voice"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Voice; return true; }
	if (S.Equals(TEXT("Charm"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Charm; return true; }
	if (S.Equals(TEXT("Headset"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Headset; return true; }
	if (S.Equals(TEXT("Backpack"), ESearchCase::IgnoreCase)) { OutSlot = EAshlineCosmeticSlot::Backpack; return true; }
	return false;
}

void UAshlineMetaCatalog::BindCosmeticContentPaths(FAshlineCosmeticDefinition& Cosmetic)
{
	if (Cosmetic.MeshOverride.IsNull())
	{
		Cosmetic.MeshOverride = TSoftObjectPtr<USkeletalMesh>(
			FSoftObjectPath(UAshlineContentManifest::CosmeticMeshPath(Cosmetic.CosmeticId)));
	}
	if (Cosmetic.MaterialOverride.IsNull())
	{
		Cosmetic.MaterialOverride = TSoftObjectPtr<UMaterialInterface>(
			FSoftObjectPath(UAshlineContentManifest::CosmeticMaterialPath(Cosmetic.CosmeticId)));
	}
}

void UAshlineMetaCatalog::BindSkinContentPaths(FAshlineWeaponSkinDefinition& Skin)
{
	if (Skin.MaterialOverride.IsNull())
	{
		Skin.MaterialOverride = TSoftObjectPtr<UMaterialInterface>(
			FSoftObjectPath(UAshlineContentManifest::SkinMaterialPath(Skin.SkinId)));
	}
}

void UAshlineMetaCatalog::OverlayCosmeticDataAsset(FAshlineCosmeticDefinition& Cosmetic)
{
	if (UAshlineCosmeticVisual* Visual = AshlineLoad::Object<UAshlineCosmeticVisual>(
		UAshlineContentManifest::CosmeticDataAssetPath(Cosmetic.CosmeticId)))
	{
		if (!Visual->MeshOverride.IsNull())
		{
			Cosmetic.MeshOverride = Visual->MeshOverride;
		}
		if (!Visual->MaterialOverride.IsNull())
		{
			Cosmetic.MaterialOverride = Visual->MaterialOverride;
		}
	}
}

void UAshlineMetaCatalog::OverlaySkinDataAsset(FAshlineWeaponSkinDefinition& Skin)
{
	if (UAshlineCosmeticVisual* Visual = AshlineLoad::Object<UAshlineCosmeticVisual>(
		UAshlineContentManifest::SkinDataAssetPath(Skin.SkinId)))
	{
		if (!Visual->MaterialOverride.IsNull())
		{
			Skin.MaterialOverride = Visual->MaterialOverride;
		}
	}
}
