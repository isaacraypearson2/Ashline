#include "Meta/AshlineMetaCatalog.h"

#include "Meta/AshlineEquipmentCatalog.h"
#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlineCosmeticVisual.h"
#include "Weapons/AshlineWeaponCatalog.h"
#include "Presentation/AshlineLoad.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
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
		const FLinearColor& Tint,
		const TCHAR* Collection = TEXT("SET_FIELD"),
		const TCHAR* Desc = TEXT(""))
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
		D.PreviewAccent = Tint * 1.25f;
		D.CollectionId = Collection;
		D.Description = FText::FromString(Desc);
		D.PreviewIconPath = FString::Printf(TEXT("/Game/Ashline/UI/Icons/T_%s.T_%s"), Id, Id);
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
		const FLinearColor& Tint,
		const TCHAR* Collection = TEXT("SET_FACTORY"),
		const TCHAR* Desc = TEXT(""),
		bool bAnimated = false)
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
		D.PreviewAccent = Tint * 1.2f;
		D.CollectionId = Collection;
		D.Description = FText::FromString(Desc);
		D.bAnimatedPreview = bAnimated;
		D.PreviewIconPath = FString::Printf(TEXT("/Game/Ashline/UI/Icons/T_%s.T_%s"), Id, Id);
		UAshlineMetaCatalog::BindSkinContentPaths(D);
		return D;
	}
}

TArray<FAshlineCosmeticDefinition> UAshlineMetaCatalog::BuildCosmetics()
{
	using namespace AshlineMeta;
	TArray<FAshlineCosmeticDefinition> List;
	List.Reserve(128);

	List.Add(C(TEXT("CAMO_FIELD"), TEXT("Field Ash"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.22f, 0.26f, 0.16f), TEXT("SET_FIELD"), TEXT("Issue woodland. Every operator starts here.")));
	List.Add(C(TEXT("CAMO_NIGHT"), TEXT("Night Glass"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Uncommon, 6, 400, 0, false, FLinearColor(0.08f, 0.1f, 0.16f), TEXT("SET_NIGHT"), TEXT("ASH-04 overwatch blue-black.")));
	List.Add(C(TEXT("CAMO_DUST"), TEXT("Dust Market"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Uncommon, 8, 450, 0, false, FLinearColor(0.42f, 0.3f, 0.14f), TEXT("SET_DUST"), TEXT("ASH-02 bazaar tan.")));
	List.Add(C(TEXT("CAMO_WHITEOUT"), TEXT("Whiteout"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Rare, 18, 900, 0, false, FLinearColor(0.78f, 0.82f, 0.88f), TEXT("SET_WHITEOUT"), TEXT("ASH-07 alpine.")));
	List.Add(C(TEXT("CAMO_ASHLINE"), TEXT("Terminus"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Legendary, 40, 2400, 0, false, FLinearColor(0.45f, 0.08f, 0.06f), TEXT("SET_TERMINUS"), TEXT("Finale red. Rank 40 grant.")));
	List.Add(C(TEXT("CAMO_PRESTIGE"), TEXT("Prestige Ash"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Epic, 1, 0, 1, false, FLinearColor(0.72f, 0.62f, 0.22f), TEXT("SET_PRESTIGE"), TEXT("Gilt woodland. Prestige 1.")));
	List.Add(C(TEXT("CAMO_URBAN"), TEXT("Urban Ash"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Uncommon, 10, 500, 0, false, FLinearColor(0.22f, 0.22f, 0.24f), TEXT("SET_URBAN"), TEXT("Campus grey. ASH-10.")));
	List.Add(C(TEXT("CAMO_WOODLAND"), TEXT("Woodland Split"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Common, 3, 200, 0, false, FLinearColor(0.16f, 0.28f, 0.12f), TEXT("SET_FIELD"), TEXT("Classic ERDL-adjacent.")));
	List.Add(C(TEXT("CAMO_DIGITAL"), TEXT("Digital Hex"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Rare, 14, 750, 0, false, FLinearColor(0.18f, 0.22f, 0.2f), TEXT("SET_URBAN"), TEXT("Pixel hex. Armory favorite.")));
	List.Add(C(TEXT("CAMO_HARBOR"), TEXT("Harbor Grey"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Uncommon, 12, 550, 0, false, FLinearColor(0.2f, 0.24f, 0.28f), TEXT("SET_HARBOR"), TEXT("ASH-06 wet steel.")));
	List.Add(C(TEXT("CAMO_CATACOMB"), TEXT("Catacomb"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Rare, 21, 850, 0, false, FLinearColor(0.18f, 0.12f, 0.08f), TEXT("SET_CATACOMB"), TEXT("ASH-08 brick dust.")));
	List.Add(C(TEXT("CAMO_RIDGE"), TEXT("Ridge Wire"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Uncommon, 15, 600, 0, false, FLinearColor(0.28f, 0.32f, 0.18f), TEXT("SET_RIDGE"), TEXT("ASH-09 alpine grass.")));
	List.Add(C(TEXT("CAMO_CONVOY"), TEXT("Convoy Amber"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Uncommon, 9, 480, 0, false, FLinearColor(0.38f, 0.26f, 0.1f), TEXT("SET_CONVOY"), TEXT("ASH-05 highway dusk.")));
	List.Add(C(TEXT("CAMO_TRAIN"), TEXT("Last Train"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Rare, 24, 950, 0, false, FLinearColor(0.16f, 0.16f, 0.14f), TEXT("SET_TRAIN"), TEXT("ASH-11 grease and gravel.")));
	List.Add(C(TEXT("CAMO_MARITIME"), TEXT("Maritime"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Rare, 19, 880, 0, false, FLinearColor(0.12f, 0.2f, 0.28f), TEXT("SET_HARBOR"), TEXT("Navy blue-grey.")));
	List.Add(C(TEXT("CAMO_ARID"), TEXT("Arid Split"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Common, 4, 250, 0, false, FLinearColor(0.46f, 0.38f, 0.22f), TEXT("SET_DUST"), TEXT("Holdfast bleach.")));
	List.Add(C(TEXT("CAMO_FROST"), TEXT("Frostbite"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Epic, 28, 1400, 0, false, FLinearColor(0.7f, 0.78f, 0.86f), TEXT("SET_WHITEOUT"), TEXT("Iridescent snow.")));
	List.Add(C(TEXT("CAMO_GHOST"), TEXT("Ghost Mesh"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Epic, 32, 1600, 0, false, FLinearColor(0.14f, 0.16f, 0.15f), TEXT("SET_NIGHT"), TEXT("Near-black multi-cam.")));
	List.Add(C(TEXT("CAMO_SPECTRE"), TEXT("Spectre"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Legendary, 45, 2800, 1, false, FLinearColor(0.55f, 0.08f, 0.42f), TEXT("SET_PRESTIGE"), TEXT("Prestige 1 night-purple.")));
	List.Add(C(TEXT("CAMO_HOLDFAST"), TEXT("Holdfast Sand"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Common, 5, 280, 0, false, FLinearColor(0.4f, 0.34f, 0.2f), TEXT("SET_DUST"), TEXT("ASH-03 berm tan.")));
	List.Add(C(TEXT("CAMO_DIAMOND"), TEXT("Diamond Ash"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Legendary, 1, 0, 2, false, FLinearColor(0.78f, 0.86f, 0.92f), TEXT("SET_PRESTIGE"), TEXT("Prestige 2 ice diamond.")));
	List.Add(C(TEXT("CAMO_MULTICAM"), TEXT("Multicam"), EAshlineCosmeticSlot::Camo, EAshlineLootRarity::Rare, 17, 820, 0, false, FLinearColor(0.32f, 0.3f, 0.18f), TEXT("SET_FIELD"), TEXT("Issue multi-environment.")));

	List.Add(C(TEXT("HELM_PATROL"), TEXT("Patrol cap"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.16f, 0.18f, 0.12f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("HELM_FAST"), TEXT("FAST helmet"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Rare, 12, 700, 0, false, FLinearColor(0.1f, 0.1f, 0.1f), TEXT("SET_URBAN")));
	List.Add(C(TEXT("HELM_BOONIE"), TEXT("Boonie"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Uncommon, 5, 350, 0, false, FLinearColor(0.28f, 0.26f, 0.14f), TEXT("SET_DUST")));
	List.Add(C(TEXT("HELM_BERET"), TEXT("Beret"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Rare, 16, 650, 0, false, FLinearColor(0.28f, 0.06f, 0.08f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("HELM_GAS"), TEXT("Gas mask"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Epic, 22, 1100, 0, false, FLinearColor(0.12f, 0.14f, 0.1f), TEXT("SET_CATACOMB")));
	List.Add(C(TEXT("HELM_SHEMAGH"), TEXT("Shemagh"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Uncommon, 7, 400, 0, false, FLinearColor(0.36f, 0.28f, 0.16f), TEXT("SET_DUST")));
	List.Add(C(TEXT("HELM_BALLISTIC"), TEXT("Ballistic full-cut"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Rare, 20, 900, 0, false, FLinearColor(0.08f, 0.09f, 0.08f), TEXT("SET_URBAN")));
	List.Add(C(TEXT("HELM_BEANIE"), TEXT("Watch cap"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Common, 3, 180, 0, false, FLinearColor(0.12f, 0.12f, 0.14f), TEXT("SET_WHITEOUT")));
	List.Add(C(TEXT("HELM_NVG"), TEXT("NVG mount"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Epic, 26, 1300, 0, false, FLinearColor(0.1f, 0.12f, 0.1f), TEXT("SET_NIGHT")));
	List.Add(C(TEXT("HELM_CAP"), TEXT("Contractor cap"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Common, 2, 120, 0, false, FLinearColor(0.2f, 0.18f, 0.12f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("HELM_MICH"), TEXT("MICH 2000"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Rare, 14, 760, 0, false, FLinearColor(0.12f, 0.14f, 0.1f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("HELM_PILOT"), TEXT("Pilot visor"), EAshlineCosmeticSlot::Helmet, EAshlineLootRarity::Epic, 28, 1400, 0, false, FLinearColor(0.08f, 0.1f, 0.14f), TEXT("SET_HARBOR")));

	List.Add(C(TEXT("VEST_PLATE"), TEXT("Plate carrier"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.14f, 0.16f, 0.12f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("VEST_HEAVY"), TEXT("Heavy rig"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Rare, 20, 850, 0, false, FLinearColor(0.12f, 0.1f, 0.08f), TEXT("SET_URBAN")));
	List.Add(C(TEXT("VEST_RECON"), TEXT("Recon vest"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Uncommon, 9, 500, 0, false, FLinearColor(0.18f, 0.16f, 0.1f), TEXT("SET_RIDGE")));
	List.Add(C(TEXT("VEST_SLICK"), TEXT("Slick carrier"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Uncommon, 6, 420, 0, false, FLinearColor(0.16f, 0.16f, 0.14f), TEXT("SET_URBAN")));
	List.Add(C(TEXT("VEST_RAID"), TEXT("Raid rig"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Rare, 17, 780, 0, false, FLinearColor(0.2f, 0.14f, 0.1f), TEXT("SET_NIGHT")));
	List.Add(C(TEXT("VEST_CHEST"), TEXT("Chest rig"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Common, 4, 260, 0, false, FLinearColor(0.22f, 0.2f, 0.12f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("VEST_AVIATOR"), TEXT("Aviator harness"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Epic, 30, 1200, 0, false, FLinearColor(0.28f, 0.22f, 0.12f), TEXT("SET_HARBOR")));
	List.Add(C(TEXT("VEST_RADIO"), TEXT("Radio pack"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Rare, 13, 640, 0, false, FLinearColor(0.15f, 0.16f, 0.14f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("VEST_JUMP"), TEXT("Jump plate"), EAshlineCosmeticSlot::Vest, EAshlineLootRarity::Uncommon, 8, 460, 0, false, FLinearColor(0.18f, 0.16f, 0.12f), TEXT("SET_RIDGE")));

	List.Add(C(TEXT("PANT_FATIGUE"), TEXT("Field fatigues"), EAshlineCosmeticSlot::Pants, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.2f, 0.22f, 0.14f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("PANT_CRYE"), TEXT("Crye cut"), EAshlineCosmeticSlot::Pants, EAshlineLootRarity::Uncommon, 11, 400, 0, false, FLinearColor(0.18f, 0.2f, 0.12f), TEXT("SET_URBAN")));
	List.Add(C(TEXT("PANT_CARGO"), TEXT("Cargo"), EAshlineCosmeticSlot::Pants, EAshlineLootRarity::Common, 3, 200, 0, false, FLinearColor(0.24f, 0.22f, 0.14f), TEXT("SET_DUST")));
	List.Add(C(TEXT("PANT_ALPINE"), TEXT("Alpine"), EAshlineCosmeticSlot::Pants, EAshlineLootRarity::Rare, 18, 520, 0, false, FLinearColor(0.7f, 0.74f, 0.78f), TEXT("SET_WHITEOUT")));
	List.Add(C(TEXT("PANT_URBAN"), TEXT("Urban combat"), EAshlineCosmeticSlot::Pants, EAshlineLootRarity::Uncommon, 10, 380, 0, false, FLinearColor(0.18f, 0.18f, 0.2f), TEXT("SET_URBAN")));
	List.Add(C(TEXT("PANT_G3"), TEXT("G3 combat"), EAshlineCosmeticSlot::Pants, EAshlineLootRarity::Rare, 15, 480, 0, false, FLinearColor(0.16f, 0.18f, 0.12f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("PANT_JEANS"), TEXT("Contractor jeans"), EAshlineCosmeticSlot::Pants, EAshlineLootRarity::Common, 2, 140, 0, false, FLinearColor(0.12f, 0.16f, 0.22f), TEXT("SET_URBAN")));

	List.Add(C(TEXT("GLOVE_NOMEX"), TEXT("Nomex gloves"), EAshlineCosmeticSlot::Gloves, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.08f, 0.08f, 0.08f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("GLOVE_WINTER"), TEXT("Winter gloves"), EAshlineCosmeticSlot::Gloves, EAshlineLootRarity::Rare, 18, 450, 0, false, FLinearColor(0.22f, 0.24f, 0.26f), TEXT("SET_WHITEOUT")));
	List.Add(C(TEXT("GLOVE_TACTICAL"), TEXT("Tactical hard-knuckle"), EAshlineCosmeticSlot::Gloves, EAshlineLootRarity::Uncommon, 8, 280, 0, false, FLinearColor(0.1f, 0.1f, 0.1f), TEXT("SET_URBAN")));
	List.Add(C(TEXT("GLOVE_CUT"), TEXT("Shooter's cut"), EAshlineCosmeticSlot::Gloves, EAshlineLootRarity::Uncommon, 6, 240, 0, false, FLinearColor(0.12f, 0.1f, 0.08f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("GLOVE_WOOL"), TEXT("Wool inserts"), EAshlineCosmeticSlot::Gloves, EAshlineLootRarity::Common, 4, 160, 0, false, FLinearColor(0.28f, 0.24f, 0.2f), TEXT("SET_WHITEOUT")));
	List.Add(C(TEXT("GLOVE_HALF"), TEXT("Half-finger"), EAshlineCosmeticSlot::Gloves, EAshlineLootRarity::Uncommon, 5, 200, 0, false, FLinearColor(0.14f, 0.1f, 0.08f), TEXT("SET_FIELD")));

	List.Add(C(TEXT("BOOT_COMBAT"), TEXT("Combat boots"), EAshlineCosmeticSlot::Boots, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.1f, 0.08f, 0.06f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("BOOT_DESERT"), TEXT("Desert boots"), EAshlineCosmeticSlot::Boots, EAshlineLootRarity::Uncommon, 8, 300, 0, false, FLinearColor(0.32f, 0.24f, 0.12f), TEXT("SET_DUST")));
	List.Add(C(TEXT("BOOT_ALPINE"), TEXT("Alpine boots"), EAshlineCosmeticSlot::Boots, EAshlineLootRarity::Rare, 18, 420, 0, false, FLinearColor(0.2f, 0.22f, 0.24f), TEXT("SET_WHITEOUT")));
	List.Add(C(TEXT("BOOT_MARITIME"), TEXT("Maritime boots"), EAshlineCosmeticSlot::Boots, EAshlineLootRarity::Uncommon, 12, 340, 0, false, FLinearColor(0.08f, 0.1f, 0.12f), TEXT("SET_HARBOR")));
	List.Add(C(TEXT("BOOT_HIKE"), TEXT("Patrol hikers"), EAshlineCosmeticSlot::Boots, EAshlineLootRarity::Common, 5, 180, 0, false, FLinearColor(0.18f, 0.14f, 0.08f), TEXT("SET_RIDGE")));
	List.Add(C(TEXT("BOOT_TANKER"), TEXT("Tanker boots"), EAshlineCosmeticSlot::Boots, EAshlineLootRarity::Rare, 16, 380, 0, false, FLinearColor(0.08f, 0.08f, 0.07f), TEXT("SET_CONVOY")));

	List.Add(C(TEXT("FACE_00"), TEXT("Operator A"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.45f, 0.34f, 0.26f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("FACE_01"), TEXT("Operator B"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Common, 4, 150, 0, false, FLinearColor(0.38f, 0.28f, 0.2f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("FACE_02"), TEXT("Operator C"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Uncommon, 8, 220, 0, false, FLinearColor(0.5f, 0.36f, 0.24f), TEXT("SET_DUST")));
	List.Add(C(TEXT("FACE_03"), TEXT("Operator D"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Uncommon, 12, 220, 0, false, FLinearColor(0.32f, 0.24f, 0.18f), TEXT("SET_NIGHT")));
	List.Add(C(TEXT("FACE_04"), TEXT("Operator E"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Rare, 16, 350, 0, false, FLinearColor(0.42f, 0.3f, 0.22f), TEXT("SET_HARBOR")));
	List.Add(C(TEXT("FACE_05"), TEXT("Operator F"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Rare, 22, 350, 0, false, FLinearColor(0.28f, 0.22f, 0.18f), TEXT("SET_WHITEOUT")));
	List.Add(C(TEXT("FACE_06"), TEXT("Operator G"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Epic, 26, 480, 0, false, FLinearColor(0.36f, 0.26f, 0.2f), TEXT("SET_HARBOR")));
	List.Add(C(TEXT("FACE_07"), TEXT("Operator H"), EAshlineCosmeticSlot::Face, EAshlineLootRarity::Epic, 30, 520, 0, false, FLinearColor(0.24f, 0.18f, 0.14f), TEXT("SET_TERMINUS")));

	List.Add(C(TEXT("VOICE_NEUTRAL"), TEXT("Neutral VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor::White, TEXT("SET_FIELD")));
	List.Add(C(TEXT("VOICE_HOLDFAST"), TEXT("Holdfast VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Epic, 16, 1200, 0, false, FLinearColor(0.8f, 0.7f, 0.4f), TEXT("SET_DUST")));
	List.Add(C(TEXT("VOICE_HARBOR"), TEXT("Harbor VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Rare, 22, 800, 0, false, FLinearColor(0.4f, 0.6f, 0.8f), TEXT("SET_HARBOR")));
	List.Add(C(TEXT("VOICE_RIDGE"), TEXT("Ridge VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Rare, 18, 750, 0, false, FLinearColor(0.5f, 0.7f, 0.5f), TEXT("SET_RIDGE")));
	List.Add(C(TEXT("VOICE_WHITEOUT"), TEXT("Whiteout VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Epic, 25, 1100, 0, false, FLinearColor(0.75f, 0.82f, 0.9f), TEXT("SET_WHITEOUT")));
	List.Add(C(TEXT("VOICE_NIGHT"), TEXT("Night Glass VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Rare, 14, 700, 0, false, FLinearColor(0.3f, 0.4f, 0.7f), TEXT("SET_NIGHT")));
	List.Add(C(TEXT("VOICE_RADIO"), TEXT("Radio grit VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Uncommon, 10, 500, 0, false, FLinearColor(0.6f, 0.6f, 0.5f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("VOICE_TERMINUS"), TEXT("Terminus VO"), EAshlineCosmeticSlot::Voice, EAshlineLootRarity::Legendary, 40, 1600, 0, false, FLinearColor(0.7f, 0.2f, 0.12f), TEXT("SET_TERMINUS")));

	List.Add(C(TEXT("CHARM_WIRE"), TEXT("Wire Cut charm"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Rare, 2, 300, 0, false, FLinearColor(0.7f, 0.55f, 0.2f), TEXT("SET_FIELD"), TEXT("ASH-01.")));
	List.Add(C(TEXT("CHARM_SPINE"), TEXT("Spine charm"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Legendary, 50, 0, 1, false, FLinearColor(0.9f, 0.2f, 0.15f), TEXT("SET_PRESTIGE"), TEXT("Prestige 1.")));
	List.Add(C(TEXT("CHARM_DUST"), TEXT("Market coin"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Uncommon, 8, 220, 0, false, FLinearColor(0.72f, 0.5f, 0.18f), TEXT("SET_DUST")));
	List.Add(C(TEXT("CHARM_HOLDFAST"), TEXT("Sandbag tag"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Uncommon, 10, 240, 0, false, FLinearColor(0.55f, 0.45f, 0.2f), TEXT("SET_DUST")));
	List.Add(C(TEXT("CHARM_GLASS"), TEXT("Night glass shard"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Rare, 12, 360, 0, false, FLinearColor(0.3f, 0.4f, 0.7f), TEXT("SET_NIGHT")));
	List.Add(C(TEXT("CHARM_CONVOY"), TEXT("Convoy key"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Uncommon, 11, 260, 0, false, FLinearColor(0.7f, 0.4f, 0.1f), TEXT("SET_CONVOY")));
	List.Add(C(TEXT("CHARM_HARBOR"), TEXT("Harbor hook"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Rare, 14, 340, 0, false, FLinearColor(0.4f, 0.55f, 0.7f), TEXT("SET_HARBOR")));
	List.Add(C(TEXT("CHARM_WHITEOUT"), TEXT("Ice tag"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Rare, 18, 380, 0, false, FLinearColor(0.8f, 0.85f, 0.9f), TEXT("SET_WHITEOUT")));
	List.Add(C(TEXT("CHARM_CATACOMB"), TEXT("Catacomb key"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Rare, 21, 400, 0, false, FLinearColor(0.45f, 0.28f, 0.12f), TEXT("SET_CATACOMB")));
	List.Add(C(TEXT("CHARM_RIDGE"), TEXT("Ridge pin"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Uncommon, 16, 280, 0, false, FLinearColor(0.4f, 0.5f, 0.3f), TEXT("SET_RIDGE")));
	List.Add(C(TEXT("CHARM_FLAG"), TEXT("False flag chip"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Epic, 24, 700, 0, false, FLinearColor(0.7f, 0.15f, 0.12f), TEXT("SET_URBAN")));
	List.Add(C(TEXT("CHARM_TRAIN"), TEXT("Rail spike"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Rare, 26, 420, 0, false, FLinearColor(0.35f, 0.32f, 0.28f), TEXT("SET_TRAIN")));
	List.Add(C(TEXT("CHARM_TERMINUS"), TEXT("Terminus token"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Legendary, 40, 900, 0, false, FLinearColor(0.7f, 0.12f, 0.08f), TEXT("SET_TERMINUS")));
	List.Add(C(TEXT("CHARM_DOGTAG"), TEXT("Dog tag"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Common, 2, 80, 0, false, FLinearColor(0.55f, 0.55f, 0.5f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("CHARM_CASING"), TEXT("Spent casing"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Common, 3, 90, 0, false, FLinearColor(0.62f, 0.5f, 0.22f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("CHARM_KNIFE"), TEXT("Folding knife"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Uncommon, 4, 140, 0, false, FLinearColor(0.4f, 0.4f, 0.38f), TEXT("SET_FIELD")));
	List.Add(C(TEXT("CHARM_DIAMOND"), TEXT("Diamond chip"), EAshlineCosmeticSlot::Charm, EAshlineLootRarity::Legendary, 1, 0, 2, false, FLinearColor(0.8f, 0.88f, 0.95f), TEXT("SET_PRESTIGE"), TEXT("Prestige 2.")));

	List.Add(C(TEXT("HEAD_COMTAC"), TEXT("ComTac headset"), EAshlineCosmeticSlot::Headset, EAshlineLootRarity::Uncommon, 7, 450, 0, false, FLinearColor(0.1f, 0.1f, 0.1f), TEXT("SET_FIELD"), TEXT("Issue comms. PTT + earpro.")));
	List.Add(C(TEXT("HEAD_PTT"), TEXT("PTT pack"), EAshlineCosmeticSlot::Headset, EAshlineLootRarity::Rare, 13, 600, 0, false, FLinearColor(0.12f, 0.14f, 0.1f), TEXT("SET_FIELD"), TEXT("Radio PTT + boom mic.")));
	List.Add(C(TEXT("HEAD_WALKER"), TEXT("Walker electronic"), EAshlineCosmeticSlot::Headset, EAshlineLootRarity::Rare, 16, 720, 0, false, FLinearColor(0.16f, 0.16f, 0.14f), TEXT("SET_URBAN"), TEXT("Electronic earpro. Sporting cut.")));
	List.Add(C(TEXT("HEAD_AMP"), TEXT("AMP headset"), EAshlineCosmeticSlot::Headset, EAshlineLootRarity::Epic, 24, 980, 0, false, FLinearColor(0.08f, 0.08f, 0.09f), TEXT("SET_NIGHT"), TEXT("High-cut AMP. NVG-friendly.")));
	List.Add(C(TEXT("HEAD_BONE"), TEXT("Bone mic"), EAshlineCosmeticSlot::Headset, EAshlineLootRarity::Uncommon, 9, 380, 0, false, FLinearColor(0.14f, 0.12f, 0.1f), TEXT("SET_URBAN"), TEXT("Covert throat/bone set.")));

	List.Add(C(TEXT("PACK_ASSAULT"), TEXT("Assault pack"), EAshlineCosmeticSlot::Backpack, EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.16f, 0.18f, 0.12f), TEXT("SET_FIELD"), TEXT("Issue 3-day. Every operator starts here.")));
	List.Add(C(TEXT("PACK_RADIO"), TEXT("Radio bag"), EAshlineCosmeticSlot::Backpack, EAshlineLootRarity::Rare, 18, 800, 0, false, FLinearColor(0.14f, 0.12f, 0.08f), TEXT("SET_FIELD"), TEXT("Manpack radio. Radio Op silhouette.")));
	List.Add(C(TEXT("PACK_FAST"), TEXT("Fast-rope pack"), EAshlineCosmeticSlot::Backpack, EAshlineLootRarity::Uncommon, 11, 520, 0, false, FLinearColor(0.12f, 0.12f, 0.1f), TEXT("SET_URBAN"), TEXT("Low-profile CQB bag.")));
	List.Add(C(TEXT("PACK_HYDRA"), TEXT("Hydration pack"), EAshlineCosmeticSlot::Backpack, EAshlineLootRarity::Uncommon, 8, 360, 0, false, FLinearColor(0.18f, 0.2f, 0.12f), TEXT("SET_RIDGE"), TEXT("Bladder + tube. Ridge / Whiteout.")));
	List.Add(C(TEXT("PACK_MED"), TEXT("Medic pack"), EAshlineCosmeticSlot::Backpack, EAshlineLootRarity::Rare, 20, 740, 0, false, FLinearColor(0.22f, 0.1f, 0.08f), TEXT("SET_FIELD"), TEXT("IFAK expansion. Red cross tab.")));
	List.Add(C(TEXT("PACK_BREACH"), TEXT("Breach bag"), EAshlineCosmeticSlot::Backpack, EAshlineLootRarity::Epic, 26, 1100, 0, false, FLinearColor(0.1f, 0.08f, 0.06f), TEXT("SET_BREACH"), TEXT("Charges + shotgun shells.")));

	return List;
}

TArray<FAshlineWeaponSkinDefinition> UAshlineMetaCatalog::BuildWeaponSkins()
{
	using namespace AshlineMeta;
	TArray<FAshlineWeaponSkinDefinition> List;
	List.Reserve(80);

	List.Add(S(TEXT("SKIN_FACTORY"), TEXT("Factory black"), TEXT(""), EAshlineLootRarity::Common, 1, 0, 0, true, FLinearColor(0.07f, 0.07f, 0.08f), TEXT("SET_FACTORY"), TEXT("Issue Cerakote.")));
	List.Add(S(TEXT("SKIN_FDE"), TEXT("FDE"), TEXT(""), EAshlineLootRarity::Uncommon, 5, 350, 0, false, FLinearColor(0.35f, 0.28f, 0.16f), TEXT("SET_FACTORY"), TEXT("Flat dark earth.")));
	List.Add(S(TEXT("SKIN_OD"), TEXT("OD green"), TEXT(""), EAshlineLootRarity::Uncommon, 7, 350, 0, false, FLinearColor(0.16f, 0.2f, 0.1f), TEXT("SET_FACTORY")));
	List.Add(S(TEXT("SKIN_SNOW"), TEXT("Arctic"), TEXT(""), EAshlineLootRarity::Rare, 18, 800, 0, false, FLinearColor(0.75f, 0.78f, 0.82f), TEXT("SET_WHITEOUT")));
	List.Add(S(TEXT("SKIN_GOLD"), TEXT("Prestige gilt"), TEXT(""), EAshlineLootRarity::Legendary, 1, 0, 1, false, FLinearColor(0.72f, 0.58f, 0.18f), TEXT("SET_PRESTIGE"), TEXT("Prestige 1 gilt."), true));
	List.Add(S(TEXT("SKIN_URBAN"), TEXT("Urban grey"), TEXT(""), EAshlineLootRarity::Uncommon, 9, 400, 0, false, FLinearColor(0.28f, 0.28f, 0.3f), TEXT("SET_URBAN")));
	List.Add(S(TEXT("SKIN_RUST"), TEXT("Harbor rust"), TEXT(""), EAshlineLootRarity::Rare, 14, 700, 0, false, FLinearColor(0.38f, 0.16f, 0.08f), TEXT("SET_HARBOR")));
	List.Add(S(TEXT("SKIN_DIGITAL"), TEXT("Digital hex"), TEXT(""), EAshlineLootRarity::Rare, 16, 750, 0, false, FLinearColor(0.18f, 0.22f, 0.2f), TEXT("SET_URBAN")));
	List.Add(S(TEXT("SKIN_CHROME"), TEXT("Chrome"), TEXT(""), EAshlineLootRarity::Epic, 30, 1600, 0, false, FLinearColor(0.72f, 0.74f, 0.78f), TEXT("SET_PRESTIGE"), TEXT("Polished receiver."), true));
	List.Add(S(TEXT("SKIN_NIGHTWIRE"), TEXT("Nightwire"), TEXT(""), EAshlineLootRarity::Rare, 12, 650, 0, false, FLinearColor(0.06f, 0.1f, 0.18f), TEXT("SET_NIGHT")));
	List.Add(S(TEXT("SKIN_BLOODLINE"), TEXT("Bloodline"), TEXT(""), EAshlineLootRarity::Epic, 34, 1800, 0, false, FLinearColor(0.42f, 0.06f, 0.06f), TEXT("SET_TERMINUS")));
	List.Add(S(TEXT("SKIN_GLACIER"), TEXT("Glacier"), TEXT(""), EAshlineLootRarity::Epic, 28, 1500, 0, false, FLinearColor(0.62f, 0.78f, 0.88f), TEXT("SET_WHITEOUT"), TEXT(""), true));
	List.Add(S(TEXT("SKIN_COPPER"), TEXT("Copperhead"), TEXT(""), EAshlineLootRarity::Rare, 20, 850, 0, false, FLinearColor(0.55f, 0.28f, 0.1f), TEXT("SET_DUST")));
	List.Add(S(TEXT("SKIN_GHOST"), TEXT("Ghost"), TEXT(""), EAshlineLootRarity::Epic, 32, 1700, 0, false, FLinearColor(0.12f, 0.13f, 0.14f), TEXT("SET_NIGHT")));
	List.Add(S(TEXT("SKIN_CARBON"), TEXT("Carbon"), TEXT(""), EAshlineLootRarity::Rare, 22, 900, 0, false, FLinearColor(0.08f, 0.08f, 0.09f), TEXT("SET_FACTORY")));
	List.Add(S(TEXT("SKIN_TIGER"), TEXT("Tiger stripe"), TEXT(""), EAshlineLootRarity::Rare, 15, 720, 0, false, FLinearColor(0.28f, 0.22f, 0.08f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_HEX"), TEXT("Hex black"), TEXT(""), EAshlineLootRarity::Uncommon, 11, 480, 0, false, FLinearColor(0.1f, 0.1f, 0.12f), TEXT("SET_URBAN")));
	List.Add(S(TEXT("SKIN_VOID"), TEXT("Void"), TEXT(""), EAshlineLootRarity::Legendary, 48, 2600, 1, false, FLinearColor(0.04f, 0.02f, 0.08f), TEXT("SET_PRESTIGE"), TEXT("Prestige 1 void."), true));
	List.Add(S(TEXT("SKIN_WIRECUT"), TEXT("Wire Cut set"), TEXT(""), EAshlineLootRarity::Rare, 6, 500, 0, false, FLinearColor(0.12f, 0.16f, 0.1f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_TERMINUS_SET"), TEXT("Terminus set"), TEXT(""), EAshlineLootRarity::Legendary, 42, 2200, 0, false, FLinearColor(0.5f, 0.08f, 0.06f), TEXT("SET_TERMINUS"), TEXT(""), true));
	List.Add(S(TEXT("SKIN_HARBOR_SET"), TEXT("Harbor set"), TEXT(""), EAshlineLootRarity::Rare, 14, 680, 0, false, FLinearColor(0.18f, 0.24f, 0.28f), TEXT("SET_HARBOR")));
	List.Add(S(TEXT("SKIN_WHITEOUT_SET"), TEXT("Whiteout set"), TEXT(""), EAshlineLootRarity::Rare, 18, 780, 0, false, FLinearColor(0.8f, 0.84f, 0.9f), TEXT("SET_WHITEOUT")));

	List.Add(S(TEXT("SKIN_ASH16_NIGHT"), TEXT("ASH-16 Night Glass"), TEXT("WPN_AR_ASH16"), EAshlineLootRarity::Rare, 10, 600, 0, false, FLinearColor(0.05f, 0.08f, 0.14f), TEXT("SET_NIGHT")));
	List.Add(S(TEXT("SKIN_M4K_RANGER"), TEXT("M4-K Ranger"), TEXT("WPN_AR_M4K"), EAshlineLootRarity::Uncommon, 6, 420, 0, false, FLinearColor(0.2f, 0.24f, 0.12f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_AK74_BIRCH"), TEXT("AK-74 Birch"), TEXT("WPN_AR_AK74"), EAshlineLootRarity::Rare, 9, 560, 0, false, FLinearColor(0.42f, 0.3f, 0.14f), TEXT("SET_DUST")));
	List.Add(S(TEXT("SKIN_SCARH_TAN"), TEXT("SCAR-H Tan"), TEXT("WPN_AR_SCARH"), EAshlineLootRarity::Rare, 14, 700, 0, false, FLinearColor(0.4f, 0.32f, 0.18f), TEXT("SET_DUST")));
	List.Add(S(TEXT("SKIN_416C_BLACK"), TEXT("416C Blackout"), TEXT("WPN_AR_416C"), EAshlineLootRarity::Uncommon, 10, 480, 0, false, FLinearColor(0.06f, 0.06f, 0.07f), TEXT("SET_NIGHT")));
	List.Add(S(TEXT("SKIN_C9_DUST"), TEXT("C9 Dust"), TEXT("WPN_SMG_C9"), EAshlineLootRarity::Uncommon, 8, 400, 0, false, FLinearColor(0.4f, 0.28f, 0.12f), TEXT("SET_DUST")));
	List.Add(S(TEXT("SKIN_MPX_GRAPHITE"), TEXT("MPX Graphite"), TEXT("WPN_SMG_MPX"), EAshlineLootRarity::Uncommon, 8, 440, 0, false, FLinearColor(0.18f, 0.18f, 0.2f), TEXT("SET_URBAN")));
	List.Add(S(TEXT("SKIN_VEC_STRIPE"), TEXT("Vector Stripe"), TEXT("WPN_SMG_VEC"), EAshlineLootRarity::Rare, 16, 720, 0, false, FLinearColor(0.12f, 0.12f, 0.14f), TEXT("SET_URBAN")));
	List.Add(S(TEXT("SKIN_P90_FROST"), TEXT("P90 Frost"), TEXT("WPN_SMG_P90"), EAshlineLootRarity::Rare, 14, 680, 0, false, FLinearColor(0.7f, 0.78f, 0.85f), TEXT("SET_WHITEOUT")));
	List.Add(S(TEXT("SKIN_M870_BREACH"), TEXT("M870 Breacher"), TEXT("WPN_SHG_M870K"), EAshlineLootRarity::Rare, 14, 700, 0, false, FLinearColor(0.18f, 0.1f, 0.06f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_M1014_MARINE"), TEXT("M1014 Marine"), TEXT("WPN_SHG_M1014"), EAshlineLootRarity::Uncommon, 11, 500, 0, false, FLinearColor(0.15f, 0.2f, 0.22f), TEXT("SET_HARBOR")));
	List.Add(S(TEXT("SKIN_AA12_DRUM"), TEXT("AA-12 Drum"), TEXT("WPN_SHG_AA12"), EAshlineLootRarity::Epic, 22, 1400, 1, false, FLinearColor(0.1f, 0.1f, 0.1f), TEXT("SET_PRESTIGE")));
	List.Add(S(TEXT("SKIN_G28_GLASS"), TEXT("G28-L Glass"), TEXT("WPN_SNP_G28L"), EAshlineLootRarity::Epic, 24, 1400, 0, false, FLinearColor(0.12f, 0.14f, 0.18f), TEXT("SET_NIGHT")));
	List.Add(S(TEXT("SKIN_M2010_GHILLIE"), TEXT("M2010 Ghillie"), TEXT("WPN_SNP_M2010"), EAshlineLootRarity::Rare, 18, 900, 0, false, FLinearColor(0.22f, 0.28f, 0.12f), TEXT("SET_RIDGE")));
	List.Add(S(TEXT("SKIN_AWM_MAGNUM"), TEXT("AWM Magnum"), TEXT("WPN_SNP_AWM"), EAshlineLootRarity::Epic, 26, 1600, 0, false, FLinearColor(0.08f, 0.08f, 0.1f), TEXT("SET_OVERWATCH")));
	List.Add(S(TEXT("SKIN_SASS_MARK"), TEXT("SASS-7 Marks"), TEXT("WPN_DMR_SASS"), EAshlineLootRarity::Rare, 16, 750, 0, false, FLinearColor(0.14f, 0.12f, 0.08f), TEXT("SET_MARKSMAN")));
	List.Add(S(TEXT("SKIN_MK14_WOOD"), TEXT("MK14 Wood"), TEXT("WPN_DMR_MK14"), EAshlineLootRarity::Rare, 17, 780, 0, false, FLinearColor(0.32f, 0.2f, 0.1f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_SVD_DRAGON"), TEXT("SVD Dragon"), TEXT("WPN_DMR_SVD"), EAshlineLootRarity::Epic, 20, 1200, 0, false, FLinearColor(0.28f, 0.08f, 0.06f), TEXT("SET_TERMINUS")));
	List.Add(S(TEXT("SKIN_M250_SAW"), TEXT("M250 SAW"), TEXT("WPN_LMG_M250"), EAshlineLootRarity::Epic, 28, 1600, 0, false, FLinearColor(0.1f, 0.12f, 0.08f), TEXT("SET_SUPPORT")));
	List.Add(S(TEXT("SKIN_M240_OLIVE"), TEXT("M240 Olive"), TEXT("WPN_LMG_M240"), EAshlineLootRarity::Rare, 19, 820, 0, false, FLinearColor(0.18f, 0.22f, 0.1f), TEXT("SET_SUPPORT")));
	List.Add(S(TEXT("SKIN_PKM_RUST"), TEXT("PKM Rust"), TEXT("WPN_LMG_PKM"), EAshlineLootRarity::Rare, 22, 860, 0, false, FLinearColor(0.32f, 0.16f, 0.08f), TEXT("SET_HARBOR")));
	List.Add(S(TEXT("SKIN_M17_SIDE"), TEXT("M17 Sidearm"), TEXT("WPN_PIS_M17A"), EAshlineLootRarity::Uncommon, 3, 250, 0, false, FLinearColor(0.1f, 0.1f, 0.12f), TEXT("SET_SIDEARM")));
	List.Add(S(TEXT("SKIN_G19_URBAN"), TEXT("G19 Urban"), TEXT("WPN_PIS_G19"), EAshlineLootRarity::Uncommon, 6, 280, 0, false, FLinearColor(0.16f, 0.16f, 0.18f), TEXT("SET_URBAN")));
	List.Add(S(TEXT("SKIN_DEAG_GOLD"), TEXT("D50 Gilt"), TEXT("WPN_PIS_DEAG"), EAshlineLootRarity::Legendary, 20, 1800, 1, false, FLinearColor(0.7f, 0.55f, 0.16f), TEXT("SET_PRESTIGE"), TEXT(""), true));
	List.Add(S(TEXT("SKIN_MP443_FIELD"), TEXT("MP-443 Field"), TEXT("WPN_PIS_MP443"), EAshlineLootRarity::Uncommon, 8, 260, 0, false, FLinearColor(0.14f, 0.16f, 0.1f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_AUG_OD"), TEXT("AUG OD"), TEXT("WPN_AR_AUG"), EAshlineLootRarity::Rare, 13, 640, 0, false, FLinearColor(0.18f, 0.22f, 0.1f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_FAMAS_TRI"), TEXT("FAMAS Tricolor"), TEXT("WPN_AR_FAMAS"), EAshlineLootRarity::Rare, 15, 720, 0, false, FLinearColor(0.12f, 0.18f, 0.28f), TEXT("SET_URBAN")));
	List.Add(S(TEXT("SKIN_UMP_TAN"), TEXT("UMP Tan"), TEXT("WPN_SMG_UMP"), EAshlineLootRarity::Uncommon, 11, 480, 0, false, FLinearColor(0.36f, 0.28f, 0.16f), TEXT("SET_DUST")));
	List.Add(S(TEXT("SKIN_UZI_NIGHT"), TEXT("UZI Night"), TEXT("WPN_SMG_UZI"), EAshlineLootRarity::Uncommon, 7, 400, 0, false, FLinearColor(0.08f, 0.09f, 0.1f), TEXT("SET_NIGHT")));
	List.Add(S(TEXT("SKIN_SPAS_BREACH"), TEXT("SPAS Breacher"), TEXT("WPN_SHG_SPAS"), EAshlineLootRarity::Rare, 14, 700, 0, false, FLinearColor(0.16f, 0.12f, 0.08f), TEXT("SET_BREACH")));
	List.Add(S(TEXT("SKIN_1911_IVORY"), TEXT("1911 Ivory"), TEXT("WPN_PIS_1911"), EAshlineLootRarity::Rare, 10, 520, 0, false, FLinearColor(0.78f, 0.74f, 0.62f), TEXT("SET_SIDEARM")));
	List.Add(S(TEXT("SKIN_REV_SNAKE"), TEXT("Revolver Snake"), TEXT("WPN_PIS_REV"), EAshlineLootRarity::Epic, 18, 1100, 0, false, FLinearColor(0.22f, 0.12f, 0.06f), TEXT("SET_SIDEARM")));
	List.Add(S(TEXT("SKIN_RPG_WOOD"), TEXT("RPG Wood"), TEXT("WPN_LCH_RPG"), EAshlineLootRarity::Rare, 16, 800, 0, false, FLinearColor(0.32f, 0.22f, 0.1f), TEXT("SET_DEMO")));
	List.Add(S(TEXT("SKIN_AT4_OD"), TEXT("AT4 OD"), TEXT("WPN_LCH_AT4"), EAshlineLootRarity::Rare, 21, 860, 0, false, FLinearColor(0.16f, 0.2f, 0.1f), TEXT("SET_DEMO")));
	List.Add(S(TEXT("SKIN_KNIFE_BLACK"), TEXT("Knife Blackout"), TEXT("WPN_MEL_KNIFE"), EAshlineLootRarity::Uncommon, 4, 220, 0, false, FLinearColor(0.06f, 0.06f, 0.07f), TEXT("SET_MELEE")));
	List.Add(S(TEXT("SKIN_TOMA_RUST"), TEXT("Tomahawk Rust"), TEXT("WPN_MEL_TOMA"), EAshlineLootRarity::Rare, 23, 640, 0, false, FLinearColor(0.38f, 0.18f, 0.08f), TEXT("SET_MELEE")));
	List.Add(S(TEXT("SKIN_M4C_FDE"), TEXT("M4-C FDE"), TEXT("WPN_AR_M4C"), EAshlineLootRarity::Uncommon, 5, 400, 0, false, FLinearColor(0.34f, 0.28f, 0.16f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_G36_FLECK"), TEXT("G36 Flecktarn"), TEXT("WPN_AR_G36"), EAshlineLootRarity::Rare, 12, 680, 0, false, FLinearColor(0.22f, 0.26f, 0.12f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_M16_A4"), TEXT("M16 Black"), TEXT("WPN_AR_M16"), EAshlineLootRarity::Uncommon, 11, 520, 0, false, FLinearColor(0.07f, 0.07f, 0.08f), TEXT("SET_SERVICE")));
	List.Add(S(TEXT("SKIN_FAL_WOOD"), TEXT("FAL Wood"), TEXT("WPN_BR_FAL"), EAshlineLootRarity::Rare, 13, 760, 0, false, FLinearColor(0.34f, 0.2f, 0.1f), TEXT("SET_BATTLE")));
	List.Add(S(TEXT("SKIN_G3_OD"), TEXT("G3 OD"), TEXT("WPN_BR_G3"), EAshlineLootRarity::Rare, 17, 820, 0, false, FLinearColor(0.16f, 0.2f, 0.1f), TEXT("SET_BATTLE")));
	List.Add(S(TEXT("SKIN_P90C_NATO"), TEXT("P90-C NATO"), TEXT("WPN_PDW_P90C"), EAshlineLootRarity::Rare, 10, 700, 0, false, FLinearColor(0.12f, 0.14f, 0.12f), TEXT("SET_CQB")));
	List.Add(S(TEXT("SKIN_MP7_GRAPHITE"), TEXT("MP7 Graphite"), TEXT("WPN_SMG_MP7"), EAshlineLootRarity::Uncommon, 12, 540, 0, false, FLinearColor(0.16f, 0.16f, 0.18f), TEXT("SET_URBAN")));
	List.Add(S(TEXT("SKIN_M82_SAND"), TEXT("M82 Sand"), TEXT("WPN_SNP_M82"), EAshlineLootRarity::Epic, 28, 1800, 0, false, FLinearColor(0.42f, 0.34f, 0.18f), TEXT("SET_OVERWATCH")));
	List.Add(S(TEXT("SKIN_M9_ISSUE"), TEXT("M9 Issue"), TEXT("WPN_PIS_M9"), EAshlineLootRarity::Uncommon, 3, 220, 0, false, FLinearColor(0.1f, 0.11f, 0.12f), TEXT("SET_SIDEARM")));
	List.Add(S(TEXT("SKIN_KRYPTEK"), TEXT("Kryptek"), TEXT(""), EAshlineLootRarity::Epic, 27, 1500, 0, false, FLinearColor(0.18f, 0.16f, 0.12f), TEXT("SET_FIELD")));
	List.Add(S(TEXT("SKIN_AOR1"), TEXT("AOR1"), TEXT(""), EAshlineLootRarity::Rare, 19, 860, 0, false, FLinearColor(0.4f, 0.34f, 0.2f), TEXT("SET_DUST")));
	List.Add(S(TEXT("SKIN_NAVY"), TEXT("Navy"), TEXT(""), EAshlineLootRarity::Rare, 21, 880, 0, false, FLinearColor(0.08f, 0.12f, 0.2f), TEXT("SET_HARBOR")));
	List.Add(S(TEXT("SKIN_DIAMOND"), TEXT("Prestige diamond"), TEXT(""), EAshlineLootRarity::Legendary, 1, 0, 2, false, FLinearColor(0.82f, 0.9f, 0.96f), TEXT("SET_PRESTIGE"), TEXT("Prestige 2 diamond."), true));

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
		if (Rank == 5)
		{
			T.UnlockId = TEXT("CAMO_WOODLAND");
		}
		else if (Rank == 10)
		{
			T.UnlockId = TEXT("CAMO_NIGHT");
		}
		else if (Rank == 15)
		{
			T.UnlockId = TEXT("HELM_FAST");
		}
		else if (Rank == 20)
		{
			T.UnlockId = TEXT("VEST_HEAVY");
		}
		else if (Rank == 25)
		{
			T.UnlockId = TEXT("SKIN_CARBON");
		}
		else if (Rank == 30)
		{
			T.UnlockId = TEXT("CHARM_FLAG");
		}
		else if (Rank == 35)
		{
			T.UnlockId = TEXT("SKIN_CHROME");
		}
		else if (Rank == 40)
		{
			T.UnlockId = TEXT("CAMO_ASHLINE");
		}
		else if (Rank == 45)
		{
			T.UnlockId = TEXT("CAMO_GHOST");
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
	return WeaponUpgradeCostForClass(EAshlineWeaponClass::AssaultRifle, CurrentTier);
}

int32 UAshlineMetaCatalog::WeaponUpgradeCostForClass(EAshlineWeaponClass Class, int32 CurrentTier)
{
	return UAshlineWeaponCatalog::UpgradeCostForClass(Class, CurrentTier);
}

int32 UAshlineMetaCatalog::MaxUpgradeTier()
{
	return UAshlineWeaponCatalog::MaxUpgradeTier;
}

TArray<FAshlineCosmeticDefinition> UAshlineMetaCatalog::CosmeticsForSlot(EAshlineCosmeticSlot Slot)
{
	TArray<FAshlineCosmeticDefinition> Out;
	for (const FAshlineCosmeticDefinition& Item : BuildCosmetics())
	{
		if (Item.Slot == Slot)
		{
			Out.Add(Item);
		}
	}
	return Out;
}

TArray<FAshlineWeaponSkinDefinition> UAshlineMetaCatalog::SkinsForWeapon(FName WeaponId)
{
	TArray<FAshlineWeaponSkinDefinition> Out;
	for (const FAshlineWeaponSkinDefinition& Item : BuildWeaponSkins())
	{
		if (SkinFitsWeapon(Item, WeaponId))
		{
			Out.Add(Item);
		}
	}
	return Out;
}

TArray<FName> UAshlineMetaCatalog::CollectionIds()
{
	TArray<FName> Ids;
	auto Add = [&Ids](FName Id)
	{
		Ids.AddUnique(Id);
	};
	for (const FAshlineCosmeticDefinition& Item : BuildCosmetics())
	{
		Add(Item.CollectionId);
	}
	for (const FAshlineWeaponSkinDefinition& Item : BuildWeaponSkins())
	{
		Add(Item.CollectionId);
	}
	return Ids;
}

TArray<FName> UAshlineMetaCatalog::CratePoolIds(EAshlineLootRarity Rarity)
{
	TArray<FName> Ids;
	for (const FAshlineCosmeticDefinition& Item : BuildCosmetics())
	{
		if (Item.Rarity == Rarity && !Item.bStarter && Item.RequiredPrestige == 0)
		{
			Ids.Add(Item.CosmeticId);
		}
	}
	for (const FAshlineWeaponSkinDefinition& Item : BuildWeaponSkins())
	{
		if (Item.Rarity == Rarity && !Item.bStarter && Item.RequiredPrestige == 0)
		{
			Ids.Add(Item.SkinId);
		}
	}
	for (const FAshlineEquipmentDefinition& Item : UAshlineEquipmentCatalog::BuildRoster())
	{
		if (Item.Rarity == Rarity && !Item.bStarter && Item.RequiredPrestige == 0)
		{
			Ids.Add(Item.EquipmentId);
		}
	}
	return Ids;
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
	if (Skin.MeshOverride.IsNull())
	{
		Skin.MeshOverride = TSoftObjectPtr<UStaticMesh>(
			FSoftObjectPath(UAshlineContentManifest::SkinMeshPath(Skin.SkinId)));
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
		if (!Visual->PartMesh.IsNull())
		{
			Skin.MeshOverride = Visual->PartMesh;
		}
	}
}
