#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AshlineTypes.h"
#include "Presentation/AshlineMaterialTypes.h"
#include "AshlineMaterialFactory.generated.h"

class UTexture;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UMeshComponent;
class UObject;

/**
 * Runtime material-instance factory.
 *
 * Authored masters / Megascans MIs first. Engine DefaultMaterial + Engine textures always
 * succeed so missing Quixel/Fab never breaks PIE or Shipping.
 */
UCLASS()
class ASHLINE_API UAshlineMaterialFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Ashline|Material")
	static FString MasterMaterialPath(EAshlineMasterMaterial Master);

	UFUNCTION(BlueprintPure, Category = "Ashline|Material")
	static FString SurfaceInstancePath(EAshlineSurface Surface);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UMaterialInterface* LoadMaster(EAshlineMasterMaterial Master);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UMaterialInterface* LoadSurfaceInstance(EAshlineSurface Surface);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UTexture* LoadEngineFallbackTexture(FName Slot);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UTexture* ResolveTexture(const TSoftObjectPtr<UTexture>& Authored, FName Slot);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static FAshlineTextureSet DefaultsForSurface(EAshlineSurface Surface, const FLinearColor& Tint);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static EAshlineMasterMaterial MasterForSurface(EAshlineSurface Surface);

	/** Stamp every Quixel / Engine / Ashline alias onto an MID. Missing textures use Engine fallbacks. */
	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static void StampTextureSet(UMaterialInstanceDynamic* MID, const FAshlineTextureSet& Textures);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UMaterialInstanceDynamic* CreateFromMaster(
		UObject* Outer,
		EAshlineMasterMaterial Master,
		const FAshlineTextureSet& Textures);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UMaterialInstanceDynamic* CreateSurfaceInstance(
		UObject* Outer,
		EAshlineSurface Surface,
		const FLinearColor& Tint,
		const FAshlineTextureSet& Textures);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UMaterialInstanceDynamic* CreateWeaponInstance(UObject* Outer, const FLinearColor& Tint, const FAshlineTextureSet& Textures);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UMaterialInstanceDynamic* CreateCharacterInstance(UObject* Outer, const FLinearColor& Tint, const FAshlineTextureSet& Textures);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UMaterialInstanceDynamic* CreateSkinInstance(UObject* Outer, const FLinearColor& Tint, const FAshlineTextureSet& Textures);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UMaterialInstanceDynamic* CreateGlassInstance(UObject* Outer, const FLinearColor& Tint, float Opacity = 0.28f);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static UMaterialInstanceDynamic* CreateDecalInstance(UObject* Outer, const FLinearColor& Tint);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Material")
	static bool ApplyToMesh(
		UMeshComponent* Mesh,
		UObject* Outer,
		EAshlineSurface Surface,
		const FLinearColor& Tint,
		const FAshlineTextureSet& Textures);

	UFUNCTION(BlueprintPure, Category = "Ashline|Material")
	static FString DescribePipeline();
};
