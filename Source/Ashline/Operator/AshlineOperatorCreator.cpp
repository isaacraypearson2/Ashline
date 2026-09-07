#include "Operator/AshlineOperatorCreator.h"

#include "Meta/AshlineMetaCatalog.h"

TArray<FName> UAshlineOperatorCreator::GetVoicePacks()
{
	TArray<FName> Voices;
	for (const FAshlineCosmeticDefinition& Item : UAshlineMetaCatalog::BuildCosmetics())
	{
		if (Item.Slot == EAshlineCosmeticSlot::Voice)
		{
			Voices.Add(Item.CosmeticId);
		}
	}
	return Voices;
}

TArray<FName> UAshlineOperatorCreator::GetCamos()
{
	TArray<FName> Camos;
	for (const FAshlineCosmeticDefinition& Item : UAshlineMetaCatalog::BuildCosmetics())
	{
		if (Item.Slot == EAshlineCosmeticSlot::Camo)
		{
			Camos.Add(Item.CosmeticId);
		}
	}
	return Camos;
}

TArray<FName> UAshlineOperatorCreator::GetCosmeticsForSlot(EAshlineCosmeticSlot Slot)
{
	TArray<FName> Ids;
	for (const FAshlineCosmeticDefinition& Item : UAshlineMetaCatalog::BuildCosmetics())
	{
		if (Item.Slot == Slot)
		{
			Ids.Add(Item.CosmeticId);
		}
	}
	return Ids;
}

FAshlineOperatorProfile UAshlineOperatorCreator::MakeDefault()
{
	FAshlineOperatorProfile Profile;
	Profile.CamoId = TEXT("CAMO_FIELD");
	Profile.VoicePack = TEXT("VOICE_NEUTRAL");
	Profile.EquippedCosmetics.Add(EAshlineCosmeticSlot::Camo, TEXT("CAMO_FIELD"));
	Profile.EquippedCosmetics.Add(EAshlineCosmeticSlot::Helmet, TEXT("HELM_PATROL"));
	Profile.EquippedCosmetics.Add(EAshlineCosmeticSlot::Vest, TEXT("VEST_PLATE"));
	Profile.EquippedCosmetics.Add(EAshlineCosmeticSlot::Pants, TEXT("PANT_FATIGUE"));
	Profile.EquippedCosmetics.Add(EAshlineCosmeticSlot::Gloves, TEXT("GLOVE_NOMEX"));
	Profile.EquippedCosmetics.Add(EAshlineCosmeticSlot::Face, TEXT("FACE_00"));
	Profile.EquippedCosmetics.Add(EAshlineCosmeticSlot::Voice, TEXT("VOICE_NEUTRAL"));
	Profile.EquippedCosmetics.Add(EAshlineCosmeticSlot::Boots, TEXT("BOOT_COMBAT"));
	return Profile;
}

bool UAshlineOperatorCreator::Validate(const FAshlineOperatorProfile& Profile, FText& OutError)
{
	if (Profile.Callsign.Len() < 2 || Profile.Callsign.Len() > 16)
	{
		OutError = FText::FromString(TEXT("Callsign must be 2–16 characters."));
		return false;
	}
	if (!GetVoicePacks().Contains(Profile.VoicePack) && !Profile.VoicePack.IsEqual(TEXT("Neutral")))
	{
		OutError = FText::FromString(TEXT("Unknown voice pack."));
		return false;
	}
	const bool bLegacyCamo = Profile.CamoId == TEXT("AshlineField") || Profile.CamoId == TEXT("NightGlass")
		|| Profile.CamoId == TEXT("DustMarket") || Profile.CamoId == TEXT("Whiteout") || Profile.CamoId == TEXT("AshlineTerminus");
	if (!GetCamos().Contains(Profile.CamoId) && !bLegacyCamo)
	{
		OutError = FText::FromString(TEXT("Unknown camo."));
		return false;
	}
	if (Profile.FaceIndex < 0 || Profile.FaceIndex > 7)
	{
		OutError = FText::FromString(TEXT("Face index out of range (0–7)."));
		return false;
	}
	return true;
}
