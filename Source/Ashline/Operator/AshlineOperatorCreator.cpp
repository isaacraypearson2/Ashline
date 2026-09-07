#include "Operator/AshlineOperatorCreator.h"

TArray<FName> UAshlineOperatorCreator::GetVoicePacks()
{
	return {
		TEXT("Neutral"),
		TEXT("Holdfast"),
		TEXT("Harbor"),
		TEXT("Whiteout")
	};
}

TArray<FName> UAshlineOperatorCreator::GetCamos()
{
	return {
		TEXT("AshlineField"),
		TEXT("NightGlass"),
		TEXT("DustMarket"),
		TEXT("Whiteout"),
		TEXT("AshlineTerminus")
	};
}

FAshlineOperatorProfile UAshlineOperatorCreator::MakeDefault()
{
	return FAshlineOperatorProfile();
}

bool UAshlineOperatorCreator::Validate(const FAshlineOperatorProfile& Profile, FText& OutError)
{
	if (Profile.Callsign.Len() < 2 || Profile.Callsign.Len() > 16)
	{
		OutError = FText::FromString(TEXT("Callsign must be 2–16 characters."));
		return false;
	}
	if (!GetVoicePacks().Contains(Profile.VoicePack))
	{
		OutError = FText::FromString(TEXT("Unknown voice pack."));
		return false;
	}
	if (!GetCamos().Contains(Profile.CamoId))
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
