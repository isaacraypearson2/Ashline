#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.h"
#include "AshlineMaterialTypes.generated.h"

class UTexture;
class UMaterialInterface;

/**
 * Master graphs authored in-editor by Scripts/create_master_materials.py.
 * Runtime never assumes those .uassets exist — Engine textures + DefaultMaterial always work.
 *
 * Soft refs MUST match UAshlineContentManifest::MasterMaterialCandidates:
 *   /Game/Ashline/Materials/PBR/MI_Ashline{Slug}
 *   /Game/Ashline/Materials/PBR/M_Ashline{Slug}
 *   /Game/Ashline/Materials/PBR/M_{Slug}
 */
UENUM(BlueprintType)
enum class EAshlineMasterMaterial : uint8
{
	Environment UMETA(DisplayName = "M_AshlineMaster"),
	Weapon UMETA(DisplayName = "M_AshlineWeapon"),
	Character UMETA(DisplayName = "M_AshlineCharacter"),
	Decal UMETA(DisplayName = "M_AshlineDecal"),
	Glass UMETA(DisplayName = "M_AshlineGlass"),
	Skin UMETA(DisplayName = "M_AshlineSkin")
};

/**
 * Quixel / Megascans / Fab texture slots. Empty soft refs resolve to Engine fallbacks
 * (DefaultTexture / DefaultNormal / WhiteSquare) so missing packs never break PIE or Shipping.
 *
 * Packed ORM = R Ambient Occlusion, G Roughness, B Metallic (Megascans default).
 * Packed ARM = R AO, G Roughness, B Metallic (some Fab military packs).
 */
USTRUCT(BlueprintType)
struct FAshlineTextureSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	TSoftObjectPtr<UTexture> BaseColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	TSoftObjectPtr<UTexture> Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	TSoftObjectPtr<UTexture> ORM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	TSoftObjectPtr<UTexture> Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	TSoftObjectPtr<UTexture> Metallic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	TSoftObjectPtr<UTexture> AmbientOcclusion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	TSoftObjectPtr<UTexture> Emissive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	TSoftObjectPtr<UTexture> Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	TSoftObjectPtr<UTexture> Opacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	FLinearColor Tint = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	float RoughnessValue = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	float MetallicValue = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	float Specular = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	float EmissiveStrength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	float Tiling = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	float NormalStrength = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	float OpacityValue = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	float IOR = 1.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Material")
	float Subsurface = 0.f;

	bool HasAnyAuthoredTexture() const
	{
		return !BaseColor.IsNull() || !Normal.IsNull() || !ORM.IsNull()
			|| !Roughness.IsNull() || !Metallic.IsNull() || !AmbientOcclusion.IsNull()
			|| !Emissive.IsNull() || !Height.IsNull() || !Opacity.IsNull();
	}
};

/** Named parameter contract shared by C++ stampers and the editor Python masters. */
struct FAshlineMaterialParamNames
{
	static constexpr const TCHAR* Color = TEXT("Color");
	static constexpr const TCHAR* BaseColor = TEXT("BaseColor");
	static constexpr const TCHAR* BaseColorSpaced = TEXT("Base Color");
	static constexpr const TCHAR* Albedo = TEXT("Albedo");
	static constexpr const TCHAR* Diffuse = TEXT("Diffuse");
	static constexpr const TCHAR* Normal = TEXT("Normal");
	static constexpr const TCHAR* NormalMap = TEXT("NormalMap");
	static constexpr const TCHAR* ORM = TEXT("ORM");
	static constexpr const TCHAR* ARM = TEXT("ARM");
	static constexpr const TCHAR* Roughness = TEXT("Roughness");
	static constexpr const TCHAR* Metallic = TEXT("Metallic");
	static constexpr const TCHAR* AmbientOcclusion = TEXT("AmbientOcclusion");
	static constexpr const TCHAR* AO = TEXT("AO");
	static constexpr const TCHAR* Emissive = TEXT("Emissive");
	static constexpr const TCHAR* EmissiveColor = TEXT("EmissiveColor");
	static constexpr const TCHAR* EmissiveStrength = TEXT("EmissiveStrength");
	static constexpr const TCHAR* Height = TEXT("Height");
	static constexpr const TCHAR* Displacement = TEXT("Displacement");
	static constexpr const TCHAR* Opacity = TEXT("Opacity");
	static constexpr const TCHAR* OpacityMask = TEXT("OpacityMask");
	static constexpr const TCHAR* Specular = TEXT("Specular");
	static constexpr const TCHAR* Tiling = TEXT("Tiling");
	static constexpr const TCHAR* UVScale = TEXT("UVScale");
	static constexpr const TCHAR* NormalStrength = TEXT("NormalStrength");
	static constexpr const TCHAR* IOR = TEXT("IOR");
	static constexpr const TCHAR* Subsurface = TEXT("Subsurface");
	static constexpr const TCHAR* SubsurfaceColor = TEXT("SubsurfaceColor");
};
