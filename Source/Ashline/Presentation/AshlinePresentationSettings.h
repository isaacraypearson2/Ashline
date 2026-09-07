#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AshlineTypes.h"
#include "AshlinePresentationSettings.generated.h"

class USkeletalMesh;
class UStaticMesh;
class UAshlineCharacterPresentation;
class UAshlineWeaponVisual;
class UAshlineEnvironmentKit;

/** Project defaults for MetaHuman / mannequin / Fab mesh assignment. Edit in Project Settings → Ashline. */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Ashline Presentation"))
class ASHLINE_API UAshlinePresentationSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override { return TEXT("Ashline"); }

	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSoftObjectPtr<USkeletalMesh> DefaultHeroMesh;

	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSoftObjectPtr<USkeletalMesh> DefaultAIMesh;

	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSoftObjectPtr<UAshlineCharacterPresentation> HeroPresentation;

	UPROPERTY(Config, EditAnywhere, Category = "Weapons")
	TMap<FName, TSoftObjectPtr<UAshlineWeaponVisual>> WeaponVisuals;

	UPROPERTY(Config, EditAnywhere, Category = "Environments")
	TMap<EAshlineMissionId, TSoftObjectPtr<UAshlineEnvironmentKit>> MissionKits;

	UPROPERTY(Config, EditAnywhere, Category = "Weapons")
	TSoftObjectPtr<UStaticMesh> PlaceholderWeaponMesh;
};
