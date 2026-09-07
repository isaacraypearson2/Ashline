#include "Presentation/AshlineContentManifest.h"

namespace AshlineManifest
{
	static const TCHAR* MissionSlug(EAshlineMissionId Id)
	{
		switch (Id)
		{
		case EAshlineMissionId::ASH_01_WireCut: return TEXT("ASH01_WireCut");
		case EAshlineMissionId::ASH_02_DustMarket: return TEXT("ASH02_DustMarket");
		case EAshlineMissionId::ASH_03_Holdfast: return TEXT("ASH03_Holdfast");
		case EAshlineMissionId::ASH_04_NightGlass: return TEXT("ASH04_NightGlass");
		case EAshlineMissionId::ASH_05_ConvoyGhost: return TEXT("ASH05_ConvoyGhost");
		case EAshlineMissionId::ASH_06_AshHarbor: return TEXT("ASH06_AshHarbor");
		case EAshlineMissionId::ASH_07_Whiteout: return TEXT("ASH07_Whiteout");
		case EAshlineMissionId::ASH_08_Catacomb: return TEXT("ASH08_Catacomb");
		case EAshlineMissionId::ASH_09_RidgeWire: return TEXT("ASH09_RidgeWire");
		case EAshlineMissionId::ASH_10_FalseFlag: return TEXT("ASH10_FalseFlag");
		case EAshlineMissionId::ASH_11_LastTrain: return TEXT("ASH11_LastTrain");
		case EAshlineMissionId::ASH_12_Ashline: return TEXT("ASH12_Ashline");
		default: return TEXT("Shared");
		}
	}

	static const TCHAR* AISlug(EAshlineAIArchetype Archetype)
	{
		switch (Archetype)
		{
		case EAshlineAIArchetype::Breacher: return TEXT("Breacher");
		case EAshlineAIArchetype::Marksman: return TEXT("Marksman");
		case EAshlineAIArchetype::MachineGunner: return TEXT("Gunner");
		case EAshlineAIArchetype::Officer: return TEXT("Officer");
		case EAshlineAIArchetype::Scout: return TEXT("Scout");
		case EAshlineAIArchetype::Heavy: return TEXT("Heavy");
		case EAshlineAIArchetype::CivilianIrregular: return TEXT("Irregular");
		default: return TEXT("Rifleman");
		}
	}
}

FString UAshlineContentManifest::AIMeshPath(EAshlineAIArchetype Archetype)
{
	return FString::Printf(TEXT("/Game/Ashline/Characters/AI/SK_AI_%s.SK_AI_%s"),
		AshlineManifest::AISlug(Archetype), AshlineManifest::AISlug(Archetype));
}

FString UAshlineContentManifest::WeaponMeshPath(FName WeaponId)
{
	return FString::Printf(TEXT("/Game/Ashline/Weapons/Meshes/SM_%s.SM_%s"),
		*WeaponId.ToString(), *WeaponId.ToString());
}

FString UAshlineContentManifest::WeaponMuzzleFXPath(FName WeaponId)
{
	return FString::Printf(TEXT("/Game/Ashline/FX/Muzzle/NS_Muzzle_%s.NS_Muzzle_%s"),
		*WeaponId.ToString(), *WeaponId.ToString());
}

FString UAshlineContentManifest::KitDirectory(EAshlineMissionId MissionId)
{
	return FString::Printf(TEXT("/Game/Ashline/Environments/%s"), AshlineManifest::MissionSlug(MissionId));
}

FString UAshlineContentManifest::KitGroundPath(EAshlineMissionId MissionId)
{
	const TCHAR* Slug = AshlineManifest::MissionSlug(MissionId);
	return FString::Printf(TEXT("/Game/Ashline/Environments/%s/M_Ground_%s.M_Ground_%s"), Slug, Slug, Slug);
}

FString UAshlineContentManifest::KitDataAssetPath(EAshlineMissionId MissionId)
{
	const int32 Index = static_cast<int32>(MissionId);
	return FString::Printf(TEXT("/Game/Ashline/Data/Kits/DA_Kit_ASH%02d.DA_Kit_ASH%02d"), Index, Index);
}

FString UAshlineContentManifest::FireCuePath(FName WeaponId)
{
	return FString::Printf(TEXT("/Game/Ashline/Audio/Weapons/SC_Fire_%s.SC_Fire_%s"),
		*WeaponId.ToString(), *WeaponId.ToString());
}

FString UAshlineContentManifest::MusicBedPath(EAshlineMissionId MissionId)
{
	const int32 Index = static_cast<int32>(MissionId);
	return FString::Printf(TEXT("/Game/Ashline/Audio/Music/SC_Bed_ASH%02d.SC_Bed_ASH%02d"), Index, Index);
}

TArray<FString> UAshlineContentManifest::HumanoidMeshCandidates()
{
	return {
		HeroMeshPath(),
		TEXT("/Game/Ashline/Characters/MetaHuman/SK_MetaHumanBody.SK_MetaHumanBody"),
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"),
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"),
		TEXT("/Game/Characters/Mannequins/Meshes/SK_Mannequin.SK_Mannequin"),
		TEXT("/Game/GameAnimationSample/Characters/UEFN_Mannequin/Meshes/SKM_UEFN_Mannequin.SKM_UEFN_Mannequin"),
		TEXT("/Engine/EditorMeshes/AssetViewer/SkeletalMesh/DefaultSkeletalMesh.DefaultSkeletalMesh"),
		TEXT("/Engine/EngineMeshes/SkeletalMesh/DefaultSkeletalMesh.DefaultSkeletalMesh"),
		TEXT("/Engine/EngineMeshes/SkeletalMesh/SK_Mannequin.SK_Mannequin"),
		TEXT("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP.TutorialTPP")
	};
}

TArray<FString> UAshlineContentManifest::MuzzleFXCandidates()
{
	return {
		TEXT("/Game/Ashline/FX/Muzzle/NS_Muzzle_Default.NS_Muzzle_Default"),
		TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"),
		TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire")
	};
}
