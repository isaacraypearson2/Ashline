#include "Presentation/AshlineMaterialFactory.h"

#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlinePresentationLibrary.h"
#include "Components/MeshComponent.h"
#include "Engine/Texture.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace AshlineMat
{
	template <typename T>
	static T* LoadFirst(const TArray<FString>& Paths)
	{
		for (const FString& Path : Paths)
		{
			if (Path.IsEmpty())
			{
				continue;
			}
			if (T* Obj = LoadObject<T>(nullptr, *Path))
			{
				return Obj;
			}
		}
		return nullptr;
	}

	static void SetScalarAliases(UMaterialInstanceDynamic* MID, const TArray<FName>& Names, float Value)
	{
		if (!MID)
		{
			return;
		}
		for (const FName& Name : Names)
		{
			MID->SetScalarParameterValue(Name, Value);
		}
	}

	static void SetVectorAliases(UMaterialInstanceDynamic* MID, const TArray<FName>& Names, const FLinearColor& Value)
	{
		if (!MID)
		{
			return;
		}
		for (const FName& Name : Names)
		{
			MID->SetVectorParameterValue(Name, Value);
		}
	}

	static void SetTextureAliases(UMaterialInstanceDynamic* MID, const TArray<FName>& Names, UTexture* Texture)
	{
		if (!MID || !Texture)
		{
			return;
		}
		for (const FName& Name : Names)
		{
			MID->SetTextureParameterValue(Name, Texture);
		}
	}
}

FString UAshlineMaterialFactory::MasterMaterialPath(EAshlineMasterMaterial Master)
{
	return UAshlineContentManifest::MasterMaterialPath(Master);
}

FString UAshlineMaterialFactory::SurfaceInstancePath(EAshlineSurface Surface)
{
	return UAshlineContentManifest::SurfaceInstancePath(Surface);
}

UMaterialInterface* UAshlineMaterialFactory::LoadMaster(EAshlineMasterMaterial Master)
{
	TArray<FString> Paths;
	Paths.Add(MasterMaterialPath(Master));
	switch (Master)
	{
	case EAshlineMasterMaterial::Glass:
		Paths.Add(TEXT("/Engine/EngineMaterials/DefaultDeferredDecalMaterial.DefaultDeferredDecalMaterial"));
		break;
	case EAshlineMasterMaterial::Decal:
		Paths.Add(TEXT("/Engine/EngineMaterials/DefaultDeferredDecalMaterial.DefaultDeferredDecalMaterial"));
		break;
	case EAshlineMasterMaterial::Skin:
		Paths.Add(TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
		break;
	default:
		break;
	}
	Paths.Add(TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
	Paths.Add(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	return AshlineMat::LoadFirst<UMaterialInterface>(Paths);
}

UMaterialInterface* UAshlineMaterialFactory::LoadSurfaceInstance(EAshlineSurface Surface)
{
	TArray<FString> Paths;
	Paths.Add(SurfaceInstancePath(Surface));
	if (UMaterialInterface* Authored = AshlineMat::LoadFirst<UMaterialInterface>(Paths))
	{
		return Authored;
	}
	return UAshlinePresentationLibrary::GetSurfaceMaterial(Surface);
}

UTexture* UAshlineMaterialFactory::LoadEngineFallbackTexture(FName Slot)
{
	const FString SlotStr = Slot.ToString();
	if (SlotStr.Contains(TEXT("Normal")))
	{
		return AshlineMat::LoadFirst<UTexture>({
			TEXT("/Engine/EngineResources/DefaultNormal.DefaultNormal"),
			TEXT("/Engine/EngineMaterials/DefaultNormal.DefaultNormal"),
			TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture"),
			TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture")
		});
	}
	if (SlotStr.Contains(TEXT("Emissive")) || SlotStr.Contains(TEXT("Black")) || SlotStr.Contains(TEXT("Height")) || SlotStr.Contains(TEXT("Opacity")))
	{
		return AshlineMat::LoadFirst<UTexture>({
			TEXT("/Engine/EngineResources/Black.Black"),
			TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture"),
			TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture")
		});
	}
	return AshlineMat::LoadFirst<UTexture>({
		TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture"),
		TEXT("/Engine/EngineResources/DefaultDiffuse.DefaultDiffuse"),
		TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"),
		TEXT("/Engine/EngineResources/Black.Black")
	});
}

UTexture* UAshlineMaterialFactory::ResolveTexture(const TSoftObjectPtr<UTexture>& Authored, FName Slot)
{
	if (UTexture* Loaded = Authored.LoadSynchronous())
	{
		return Loaded;
	}
	return LoadEngineFallbackTexture(Slot);
}

FAshlineTextureSet UAshlineMaterialFactory::DefaultsForSurface(EAshlineSurface Surface, const FLinearColor& Tint)
{
	FAshlineTextureSet Set;
	Set.Tint = Tint;
	Set.Tiling = 1.f;
	Set.NormalStrength = 1.f;
	Set.Specular = 0.5f;
	Set.OpacityValue = 1.f;
	Set.IOR = 1.45f;
	switch (Surface)
	{
	case EAshlineSurface::Metal:
		Set.RoughnessValue = 0.32f;
		Set.MetallicValue = 0.88f;
		Set.Specular = 0.7f;
		Set.Tiling = 2.f;
		break;
	case EAshlineSurface::Glass:
		Set.RoughnessValue = 0.04f;
		Set.MetallicValue = 0.f;
		Set.OpacityValue = 0.28f;
		Set.IOR = 1.52f;
		Set.Specular = 0.9f;
		break;
	case EAshlineSurface::Skin:
		Set.RoughnessValue = 0.55f;
		Set.MetallicValue = 0.f;
		Set.Subsurface = 0.45f;
		Set.Specular = 0.35f;
		break;
	case EAshlineSurface::Water:
		Set.RoughnessValue = 0.08f;
		Set.MetallicValue = 0.f;
		Set.OpacityValue = 0.55f;
		Set.IOR = 1.33f;
		break;
	case EAshlineSurface::Snow:
		Set.RoughnessValue = 0.78f;
		Set.MetallicValue = 0.f;
		Set.Tiling = 1.5f;
		break;
	case EAshlineSurface::Sand:
		Set.RoughnessValue = 0.82f;
		Set.MetallicValue = 0.f;
		Set.Tiling = 2.5f;
		break;
	case EAshlineSurface::Wood:
		Set.RoughnessValue = 0.7f;
		Set.MetallicValue = 0.02f;
		Set.Tiling = 1.4f;
		break;
	case EAshlineSurface::Foliage:
		Set.RoughnessValue = 0.68f;
		Set.MetallicValue = 0.f;
		Set.Tiling = 1.8f;
		break;
	case EAshlineSurface::Emissive:
		Set.RoughnessValue = 0.4f;
		Set.MetallicValue = 0.1f;
		Set.EmissiveStrength = 8.f;
		break;
	case EAshlineSurface::Concrete:
		Set.RoughnessValue = 0.74f;
		Set.MetallicValue = 0.04f;
		Set.Tiling = 2.f;
		break;
	case EAshlineSurface::Plastic:
		Set.RoughnessValue = 0.55f;
		Set.MetallicValue = 0.08f;
		break;
	case EAshlineSurface::Ground:
	default:
		Set.RoughnessValue = 0.78f;
		Set.MetallicValue = 0.03f;
		Set.Tiling = 2.2f;
		break;
	}
	return Set;
}

EAshlineMasterMaterial UAshlineMaterialFactory::MasterForSurface(EAshlineSurface Surface)
{
	switch (Surface)
	{
	case EAshlineSurface::Glass:
		return EAshlineMasterMaterial::Glass;
	case EAshlineSurface::Skin:
		return EAshlineMasterMaterial::Skin;
	case EAshlineSurface::Emissive:
		return EAshlineMasterMaterial::Environment;
	case EAshlineSurface::Plastic:
		return EAshlineMasterMaterial::Character;
	default:
		return EAshlineMasterMaterial::Environment;
	}
}

void UAshlineMaterialFactory::StampTextureSet(UMaterialInstanceDynamic* MID, const FAshlineTextureSet& Textures)
{
	if (!MID)
	{
		return;
	}

	UTexture* BaseColor = ResolveTexture(Textures.BaseColor, TEXT("BaseColor"));
	UTexture* Normal = ResolveTexture(Textures.Normal, TEXT("Normal"));
	UTexture* ORM = ResolveTexture(Textures.ORM, TEXT("ORM"));
	UTexture* Roughness = Textures.Roughness.LoadSynchronous();
	UTexture* Metallic = Textures.Metallic.LoadSynchronous();
	UTexture* AO = Textures.AmbientOcclusion.LoadSynchronous();
	UTexture* Emissive = ResolveTexture(Textures.Emissive, TEXT("Emissive"));
	UTexture* Height = ResolveTexture(Textures.Height, TEXT("Height"));
	UTexture* Opacity = ResolveTexture(Textures.Opacity, TEXT("Opacity"));

	AshlineMat::SetTextureAliases(MID, {
		TEXT("BaseColor"), TEXT("Base Color"), TEXT("Albedo"), TEXT("Diffuse"),
		TEXT("Color"), TEXT("BaseColorMap"), TEXT("AlbedoMap"), TEXT("DiffuseMap")
	}, BaseColor);

	AshlineMat::SetTextureAliases(MID, {
		TEXT("Normal"), TEXT("NormalMap"), TEXT("Normalmap"), TEXT("NormalTex")
	}, Normal);

	AshlineMat::SetTextureAliases(MID, {
		TEXT("ORM"), TEXT("ARM"), TEXT("Mask"), TEXT("OcclusionRoughnessMetallic"),
		TEXT("PackedORM")
	}, ORM);

	if (Roughness)
	{
		AshlineMat::SetTextureAliases(MID, { TEXT("Roughness"), TEXT("RoughnessMap") }, Roughness);
	}
	if (Metallic)
	{
		AshlineMat::SetTextureAliases(MID, { TEXT("Metallic"), TEXT("MetallicMap") }, Metallic);
	}
	if (AO)
	{
		AshlineMat::SetTextureAliases(MID, { TEXT("AmbientOcclusion"), TEXT("AO"), TEXT("Occlusion") }, AO);
	}

	AshlineMat::SetTextureAliases(MID, { TEXT("Emissive"), TEXT("EmissiveMap"), TEXT("EmissiveTexture") }, Emissive);
	AshlineMat::SetTextureAliases(MID, { TEXT("Height"), TEXT("Displacement"), TEXT("HeightMap") }, Height);
	AshlineMat::SetTextureAliases(MID, { TEXT("Opacity"), TEXT("OpacityMask"), TEXT("OpacityMap") }, Opacity);

	AshlineMat::SetVectorAliases(MID, {
		TEXT("Color"), TEXT("BaseColor"), TEXT("Base Color"), TEXT("Tint"), TEXT("Albedo")
	}, Textures.Tint);

	AshlineMat::SetScalarAliases(MID, { TEXT("Roughness"), TEXT("RoughnessValue") }, Textures.RoughnessValue);
	AshlineMat::SetScalarAliases(MID, { TEXT("Metallic"), TEXT("MetallicValue") }, Textures.MetallicValue);
	AshlineMat::SetScalarAliases(MID, { TEXT("Specular") }, Textures.Specular);
	AshlineMat::SetScalarAliases(MID, { TEXT("EmissiveStrength"), TEXT("Emissive Intensity") }, Textures.EmissiveStrength);
	AshlineMat::SetScalarAliases(MID, { TEXT("Tiling"), TEXT("UVScale"), TEXT("TextureScale") }, Textures.Tiling);
	AshlineMat::SetScalarAliases(MID, { TEXT("NormalStrength"), TEXT("Normal Intensity") }, Textures.NormalStrength);
	AshlineMat::SetScalarAliases(MID, { TEXT("Opacity"), TEXT("OpacityValue") }, Textures.OpacityValue);
	AshlineMat::SetScalarAliases(MID, { TEXT("IOR") }, Textures.IOR);
	AshlineMat::SetScalarAliases(MID, { TEXT("Subsurface"), TEXT("SubsurfaceAmount") }, Textures.Subsurface);

	if (Textures.EmissiveStrength > 0.f)
	{
		AshlineMat::SetVectorAliases(MID, { TEXT("EmissiveColor"), TEXT("Emissive") }, Textures.Tint * Textures.EmissiveStrength);
	}
	if (Textures.Subsurface > 0.f)
	{
		AshlineMat::SetVectorAliases(MID, { TEXT("SubsurfaceColor") }, Textures.Tint);
	}
}

UMaterialInstanceDynamic* UAshlineMaterialFactory::CreateFromMaster(
	UObject* Outer,
	EAshlineMasterMaterial Master,
	const FAshlineTextureSet& Textures)
{
	UMaterialInterface* Base = LoadMaster(Master);
	if (!Base)
	{
		return nullptr;
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Outer);
	if (!MID)
	{
		return nullptr;
	}
	StampTextureSet(MID, Textures);
	return MID;
}

UMaterialInstanceDynamic* UAshlineMaterialFactory::CreateSurfaceInstance(
	UObject* Outer,
	EAshlineSurface Surface,
	const FLinearColor& Tint,
	const FAshlineTextureSet& Textures)
{
	FAshlineTextureSet Merged = Textures.HasAnyAuthoredTexture() ? Textures : DefaultsForSurface(Surface, Tint);
	if (Merged.Tint.Equals(FLinearColor::White) || !Textures.HasAnyAuthoredTexture())
	{
		Merged.Tint = Tint;
	}

	if (UMaterialInterface* AuthoredMI = LoadObject<UMaterialInterface>(nullptr, *SurfaceInstancePath(Surface)))
	{
		if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(AuthoredMI, Outer))
		{
			StampTextureSet(MID, Merged);
			return MID;
		}
	}

	if (UMaterialInstanceDynamic* FromMaster = CreateFromMaster(Outer, MasterForSurface(Surface), Merged))
	{
		return FromMaster;
	}

	return UAshlinePresentationLibrary::MakeTintedMaterial(Outer, Surface, Tint);
}

UMaterialInstanceDynamic* UAshlineMaterialFactory::CreateWeaponInstance(UObject* Outer, const FLinearColor& Tint, const FAshlineTextureSet& Textures)
{
	FAshlineTextureSet Merged = Textures.HasAnyAuthoredTexture() ? Textures : DefaultsForSurface(EAshlineSurface::Metal, Tint);
	Merged.Tint = Tint;
	if (UMaterialInstanceDynamic* MID = CreateFromMaster(Outer, EAshlineMasterMaterial::Weapon, Merged))
	{
		return MID;
	}
	return CreateSurfaceInstance(Outer, EAshlineSurface::Metal, Tint, Merged);
}

UMaterialInstanceDynamic* UAshlineMaterialFactory::CreateCharacterInstance(UObject* Outer, const FLinearColor& Tint, const FAshlineTextureSet& Textures)
{
	FAshlineTextureSet Merged = Textures.HasAnyAuthoredTexture() ? Textures : DefaultsForSurface(EAshlineSurface::Plastic, Tint);
	Merged.Tint = Tint;
	if (UMaterialInstanceDynamic* MID = CreateFromMaster(Outer, EAshlineMasterMaterial::Character, Merged))
	{
		return MID;
	}
	return CreateSurfaceInstance(Outer, EAshlineSurface::Plastic, Tint, Merged);
}

UMaterialInstanceDynamic* UAshlineMaterialFactory::CreateSkinInstance(UObject* Outer, const FLinearColor& Tint, const FAshlineTextureSet& Textures)
{
	FAshlineTextureSet Merged = Textures.HasAnyAuthoredTexture() ? Textures : DefaultsForSurface(EAshlineSurface::Skin, Tint);
	Merged.Tint = Tint;
	if (UMaterialInstanceDynamic* MID = CreateFromMaster(Outer, EAshlineMasterMaterial::Skin, Merged))
	{
		return MID;
	}
	return CreateSurfaceInstance(Outer, EAshlineSurface::Skin, Tint, Merged);
}

UMaterialInstanceDynamic* UAshlineMaterialFactory::CreateGlassInstance(UObject* Outer, const FLinearColor& Tint, float Opacity)
{
	FAshlineTextureSet Set = DefaultsForSurface(EAshlineSurface::Glass, Tint);
	Set.OpacityValue = Opacity;
	if (UMaterialInstanceDynamic* MID = CreateFromMaster(Outer, EAshlineMasterMaterial::Glass, Set))
	{
		return MID;
	}
	return CreateSurfaceInstance(Outer, EAshlineSurface::Glass, Tint, Set);
}

UMaterialInstanceDynamic* UAshlineMaterialFactory::CreateDecalInstance(UObject* Outer, const FLinearColor& Tint)
{
	FAshlineTextureSet Set = DefaultsForSurface(EAshlineSurface::Concrete, Tint);
	Set.RoughnessValue = 0.9f;
	if (UMaterialInstanceDynamic* MID = CreateFromMaster(Outer, EAshlineMasterMaterial::Decal, Set))
	{
		return MID;
	}
	UMaterialInterface* Decal = UAshlinePresentationLibrary::ResolveImpactDecalMaterial();
	if (Decal)
	{
		if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Decal, Outer))
		{
			StampTextureSet(MID, Set);
			return MID;
		}
	}
	return nullptr;
}

bool UAshlineMaterialFactory::ApplyToMesh(
	UMeshComponent* Mesh,
	UObject* Outer,
	EAshlineSurface Surface,
	const FLinearColor& Tint,
	const FAshlineTextureSet& Textures)
{
	if (!Mesh)
	{
		return false;
	}
	if (UMaterialInstanceDynamic* MID = CreateSurfaceInstance(Outer, Surface, Tint, Textures))
	{
		Mesh->SetMaterial(0, MID);
		return true;
	}
	return false;
}

FString UAshlineMaterialFactory::DescribePipeline()
{
	return TEXT("Masters (/Game/Ashline/Materials/PBR/Masters) → surface MIs → Megascans/Fab textures → Engine DefaultTexture/DefaultNormal. Missing Quixel never fails PIE/Shipping.");
}
