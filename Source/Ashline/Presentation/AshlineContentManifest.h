#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.h"
#include "AshlineContentManifest.generated.h"

/**
 * Canonical Content paths. Drop Fab / MetaHuman / Megascans files on these names
 * and the runtime resolver picks them up — no architecture rewrite.
 */
UCLASS()
class ASHLINE_API UAshlineContentManifest : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString HeroMeshPath() { return TEXT("/Game/Ashline/Characters/Hero/SK_AshlineHero.SK_AshlineHero"); }

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString HeroFacePath() { return TEXT("/Game/Ashline/Characters/Hero/SK_AshlineHero_Face.SK_AshlineHero_Face"); }

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString MetaHumanRoot() { return TEXT("/Game/Ashline/Characters/MetaHuman"); }

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString AIMeshPath(EAshlineAIArchetype Archetype);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString WeaponMeshPath(FName WeaponId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString WeaponMuzzleFXPath(FName WeaponId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString KitDirectory(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString KitGroundPath(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString KitDataAssetPath(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString FireCuePath(FName WeaponId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString MusicBedPath(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static TArray<FString> HumanoidMeshCandidates();

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static TArray<FString> MuzzleFXCandidates();

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString CosmeticMeshPath(FName CosmeticId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString CosmeticStaticMeshPath(FName CosmeticId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString CosmeticMaterialPath(FName CosmeticId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString CosmeticDataAssetPath(FName CosmeticId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString SkinMaterialPath(FName SkinId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString SkinMeshPath(FName SkinId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString MasterWeaponMaterialPath();

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString MasterSkinMaterialPath();

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString MasterCharacterMaterialPath();

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString MasterEnvironmentMaterialPath();

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString SkinDataAssetPath(FName SkinId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString ClothingPartPath(EAshlineCosmeticSlot Slot, FName CosmeticId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString AIPresentationPath(EAshlineAIArchetype Archetype);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString KitWallPath(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString KitTrimPath(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString KitFoliagePath(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static TArray<FString> FoliageMeshCandidates();

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static TArray<FString> TreeMeshCandidates();

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString SlotFolderName(EAshlineCosmeticSlot Slot);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString EquipmentMeshPath(FName EquipmentId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString EquipmentMaterialPath(FName EquipmentId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString EquipmentDataAssetPath(FName EquipmentId);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString SurfaceSlug(EAshlineSurface Surface);

	/** Authored master + MI. Missing packages fail quiet — Engine fallbacks remain valid. */
	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static TArray<FString> MasterMaterialCandidates(EAshlineSurface Surface);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static FString WeaponClassSlug(EAshlineWeaponClass Class);

	/** Class MI first (`MI_WPN_AR`), then master. Quiet if missing. */
	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static TArray<FString> WeaponClassMasterCandidates(EAshlineWeaponClass Class);

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static TArray<FString> SkinMasterCandidates();

	UFUNCTION(BlueprintPure, Category = "Ashline|Content")
	static TArray<FString> CharacterMasterCandidates();
};
