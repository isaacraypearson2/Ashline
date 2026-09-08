#include "Meta/AshlineEquipmentCatalog.h"
#include "Presentation/AshlineContentManifest.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/SoftObjectPath.h"

namespace AshlineEq
{
	static FAshlineEquipmentDefinition E(
		const TCHAR* Id,
		const TCHAR* Name,
		EAshlineEquipmentSlot Slot,
		EAshlineLootRarity Rarity,
		int32 Rank,
		int32 Cost,
		int32 Prestige,
		bool bStarter,
		float Damage,
		float Radius,
		float Fuse,
		int32 Carry,
		const TCHAR* Desc,
		const TCHAR* Collection = TEXT("SET_FIELD"))
	{
		FAshlineEquipmentDefinition D;
		D.EquipmentId = Id;
		D.DisplayName = FText::FromString(Name);
		D.Slot = Slot;
		D.Rarity = Rarity;
		D.UnlockRank = Rank;
		D.CreditCost = Cost;
		D.RequiredPrestige = Prestige;
		D.bStarter = bStarter;
		D.Damage = Damage;
		D.RadiusMeters = Radius;
		D.FuseSeconds = Fuse;
		D.CarryCount = Carry;
		D.Description = FText::FromString(Desc);
		D.CollectionId = Collection;
		D.PreviewIconPath = FString::Printf(TEXT("/Game/Ashline/UI/Icons/T_%s.T_%s"), Id, Id);
		UAshlineEquipmentCatalog::BindContentPaths(D);
		return D;
	}
}

TArray<FAshlineEquipmentDefinition> UAshlineEquipmentCatalog::BuildRoster()
{
	using namespace AshlineEq;
	TArray<FAshlineEquipmentDefinition> List;
	List.Reserve(16);

	List.Add(E(TEXT("EQ_FRAG"), TEXT("Frag grenade"), EAshlineEquipmentSlot::Lethal, EAshlineLootRarity::Common, 1, 0, 0, true, 140.f, 6.5f, 1.8f, 2, TEXT("Issue M67-class. Cookable. Standard SP lethal."), TEXT("SET_FIELD")));
	List.Add(E(TEXT("EQ_SEMTEX"), TEXT("Semtex"), EAshlineEquipmentSlot::Lethal, EAshlineLootRarity::Uncommon, 6, 350, 0, false, 155.f, 5.5f, 1.2f, 2, TEXT("Stick charge. Shorter fuse, smaller radius."), TEXT("SET_URBAN")));
	List.Add(E(TEXT("EQ_TKNIFE"), TEXT("Throwing knife"), EAshlineEquipmentSlot::Lethal, EAshlineLootRarity::Rare, 10, 420, 0, false, 95.f, 0.4f, 0.f, 3, TEXT("Silent. Instant, no blast."), TEXT("SET_NIGHT")));
	List.Add(E(TEXT("EQ_C4"), TEXT("C4"), EAshlineEquipmentSlot::Lethal, EAshlineLootRarity::Rare, 14, 700, 0, false, 190.f, 7.5f, 0.f, 1, TEXT("Detonate on command. Heavy vs armor."), TEXT("SET_URBAN")));
	List.Add(E(TEXT("EQ_CLAYMORE"), TEXT("Claymore"), EAshlineEquipmentSlot::Lethal, EAshlineLootRarity::Uncommon, 8, 480, 0, false, 160.f, 8.f, 0.f, 1, TEXT("Front-facing trip. Hold a door."), TEXT("SET_FIELD")));
	List.Add(E(TEXT("EQ_MOLOTOV"), TEXT("Incendiary"), EAshlineEquipmentSlot::Lethal, EAshlineLootRarity::Rare, 16, 620, 0, false, 28.f, 4.f, 0.4f, 1, TEXT("Area denial. Tick damage in radius."), TEXT("SET_DUST")));

	List.Add(E(TEXT("EQ_FLASH"), TEXT("Flashbang"), EAshlineEquipmentSlot::Tactical, EAshlineLootRarity::Common, 1, 0, 0, true, 0.f, 8.f, 1.1f, 2, TEXT("Issue stun flash. Breaks a peek."), TEXT("SET_FIELD")));
	List.Add(E(TEXT("EQ_STUN"), TEXT("Stun grenade"), EAshlineEquipmentSlot::Tactical, EAshlineLootRarity::Uncommon, 5, 280, 0, false, 0.f, 7.f, 1.0f, 2, TEXT("Concuss. Longer disable, no flash."), TEXT("SET_URBAN")));
	List.Add(E(TEXT("EQ_SMOKE"), TEXT("Smoke"), EAshlineEquipmentSlot::Tactical, EAshlineLootRarity::Common, 3, 180, 0, false, 0.f, 9.f, 1.4f, 2, TEXT("Cross a lane. Hides AI perception."), TEXT("SET_FIELD")));
	List.Add(E(TEXT("EQ_STIM"), TEXT("Stim shot"), EAshlineEquipmentSlot::Tactical, EAshlineLootRarity::Rare, 12, 540, 0, false, -40.f, 0.f, 0.f, 1, TEXT("Self-heal burst. Negative damage = heal."), TEXT("SET_FIELD")));
	List.Add(E(TEXT("EQ_DECOY"), TEXT("Decoy"), EAshlineEquipmentSlot::Tactical, EAshlineLootRarity::Uncommon, 7, 260, 0, false, 0.f, 12.f, 0.5f, 2, TEXT("Fake gunfire. Pulls AI off you."), TEXT("SET_NIGHT")));
	List.Add(E(TEXT("EQ_TROPHY"), TEXT("Trophy system"), EAshlineEquipmentSlot::Tactical, EAshlineLootRarity::Epic, 22, 1100, 0, false, 0.f, 6.f, 0.f, 1, TEXT("Eats incoming nades in a bubble."), TEXT("SET_URBAN")));

	List.Add(E(TEXT("EQ_AMMO"), TEXT("Ammo crate"), EAshlineEquipmentSlot::Field, EAshlineLootRarity::Uncommon, 9, 400, 0, false, 0.f, 2.f, 0.f, 1, TEXT("Refill mag + reserve once per deploy."), TEXT("SET_SUPPORT")));
	List.Add(E(TEXT("EQ_PLATE"), TEXT("Armor plate"), EAshlineEquipmentSlot::Field, EAshlineLootRarity::Rare, 11, 500, 0, false, 0.f, 0.f, 1.6f, 1, TEXT("Insert plate. Damage soak until broken."), TEXT("SET_URBAN")));
	List.Add(E(TEXT("EQ_COVER"), TEXT("Deployable cover"), EAshlineEquipmentSlot::Field, EAshlineLootRarity::Epic, 20, 900, 0, false, 0.f, 1.5f, 0.8f, 1, TEXT("Drop a ballistic shield panel."), TEXT("SET_FIELD")));
	List.Add(E(TEXT("EQ_HEARTBEAT"), TEXT("Heartbeat sensor"), EAshlineEquipmentSlot::Field, EAshlineLootRarity::Legendary, 28, 1600, 1, false, 0.f, 18.f, 0.f, 1, TEXT("Prestige 1. Pings hostiles in radius."), TEXT("SET_PRESTIGE")));

	return List;
}

bool UAshlineEquipmentCatalog::FindEquipment(FName EquipmentId, FAshlineEquipmentDefinition& OutEquipment)
{
	for (const FAshlineEquipmentDefinition& Item : BuildRoster())
	{
		if (Item.EquipmentId == EquipmentId)
		{
			OutEquipment = Item;
			return true;
		}
	}
	return false;
}

TArray<FAshlineEquipmentDefinition> UAshlineEquipmentCatalog::OfSlot(EAshlineEquipmentSlot Slot)
{
	TArray<FAshlineEquipmentDefinition> Out;
	for (const FAshlineEquipmentDefinition& Item : BuildRoster())
	{
		if (Item.Slot == Slot)
		{
			Out.Add(Item);
		}
	}
	return Out;
}

TArray<FName> UAshlineEquipmentCatalog::StarterIds()
{
	TArray<FName> Ids;
	for (const FAshlineEquipmentDefinition& Item : BuildRoster())
	{
		if (Item.bStarter)
		{
			Ids.Add(Item.EquipmentId);
		}
	}
	return Ids;
}

FString UAshlineEquipmentCatalog::SlotName(EAshlineEquipmentSlot Slot)
{
	switch (Slot)
	{
	case EAshlineEquipmentSlot::Lethal: return TEXT("Lethal");
	case EAshlineEquipmentSlot::Tactical: return TEXT("Tactical");
	case EAshlineEquipmentSlot::Field: return TEXT("Field");
	default: return TEXT("Lethal");
	}
}

void UAshlineEquipmentCatalog::BindContentPaths(FAshlineEquipmentDefinition& Equipment)
{
	if (Equipment.MeshOverride.IsNull())
	{
		Equipment.MeshOverride = TSoftObjectPtr<UStaticMesh>(
			FSoftObjectPath(UAshlineContentManifest::EquipmentMeshPath(Equipment.EquipmentId)));
	}
	if (Equipment.MaterialOverride.IsNull())
	{
		Equipment.MaterialOverride = TSoftObjectPtr<UMaterialInterface>(
			FSoftObjectPath(UAshlineContentManifest::EquipmentMaterialPath(Equipment.EquipmentId)));
	}
}
