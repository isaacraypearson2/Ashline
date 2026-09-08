#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AshlineTypes.h"
#include "Presentation/AshlinePresentationTypes.h"
#include "AshlinePresentationLibrary.generated.h"

class UStaticMesh;
class USkeletalMesh;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UAshlineEnvironmentKit;
class UAshlineCharacterPresentation;
class UAshlineWeaponVisual;
class UObject;
class AActor;
class ACharacter;

/** Runtime resolver: Engine materials/meshes first, then authored DataAssets / StarterContent only if the package exists. Never assumes Fab binaries exist. */
UCLASS()
class ASHLINE_API UAshlinePresentationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UMaterialInterface* LoadMaterial(const TArray<FString>& Paths);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UStaticMesh* LoadStaticMesh(const TArray<FString>& Paths);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static USkeletalMesh* LoadSkeletalMesh(const TArray<FString>& Paths);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UMaterialInterface* GetSurfaceMaterial(EAshlineSurface Surface);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UMaterialInstanceDynamic* MakeTintedMaterial(UObject* Outer, EAshlineSurface Surface, const FLinearColor& Tint);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static EAshlineSurface InferSurface(const FLinearColor& Color);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static FAshlineLightingMood MoodForMission(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UAshlineEnvironmentKit* FindEnvironmentKit(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UAshlineCharacterPresentation* FindCharacterPresentation(bool bHero, EAshlineAIArchetype Archetype);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UAshlineWeaponVisual* FindWeaponVisual(FName WeaponId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static USkeletalMesh* ResolveHumanoidMesh();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UStaticMesh* ResolveWeaponPlaceholderMesh();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UMaterialInterface* ResolveImpactDecalMaterial();

	UFUNCTION(BlueprintPure, Category = "Ashline|Presentation")
	static FString DescribeFallbackChain();

	/** AAA-style mannequin blockout (head/torso/limbs) when no skeletal mesh is available. */
	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static void ApplyHumanoidBlockout(ACharacter* Character, const FLinearColor& Tint);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static void TintNamedStaticMesh(AActor* Actor, FName ComponentName, const FLinearColor& Tint, EAshlineSurface Surface);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static USkeletalMesh* ResolveCosmeticMesh(const FAshlineCosmeticDefinition& Cosmetic);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UStaticMesh* ResolveCosmeticPartMesh(const FAshlineCosmeticDefinition& Cosmetic);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UMaterialInterface* ResolveCosmeticMaterial(const FAshlineCosmeticDefinition& Cosmetic);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UMaterialInterface* ResolveSkinMaterial(const FAshlineWeaponSkinDefinition& Skin);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UStaticMesh* ResolveFoliageMesh();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static UStaticMesh* ResolveTreeMesh();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static void ApplyClothingPart(ACharacter* Character, EAshlineCosmeticSlot Slot, FName CosmeticId, const FLinearColor& Tint);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Presentation")
	static void HideCapsuleVisual(ACharacter* Character);

	UFUNCTION(BlueprintPure, Category = "Ashline|Presentation")
	static FName ClothingComponentName(EAshlineCosmeticSlot Slot);
};
