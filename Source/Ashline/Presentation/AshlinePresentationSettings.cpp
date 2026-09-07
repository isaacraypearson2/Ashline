#include "Presentation/AshlinePresentationSettings.h"

#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Presentation/AshlineCharacterPresentation.h"
#include "UObject/SoftObjectPath.h"

UAshlinePresentationSettings::UAshlinePresentationSettings()
{
	DefaultHeroMesh = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(
		TEXT("/Game/Ashline/Characters/Hero/SK_AshlineHero.SK_AshlineHero")));
	DefaultAIMesh = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(
		TEXT("/Game/Ashline/Characters/AI/SK_AI_Rifleman.SK_AI_Rifleman")));
	HeroPresentation = TSoftObjectPtr<UAshlineCharacterPresentation>(FSoftObjectPath(
		TEXT("/Game/Ashline/Data/Kits/DA_Hero_Operator.DA_Hero_Operator")));
	PlaceholderWeaponMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(
		TEXT("/Game/Ashline/Weapons/Meshes/SM_WPN_AR_ASH16.SM_WPN_AR_ASH16")));
}
