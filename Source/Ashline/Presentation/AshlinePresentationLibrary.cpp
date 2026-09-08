#include "Presentation/AshlinePresentationLibrary.h"
#include "Presentation/AshlineLoad.h"

#include "Presentation/AshlineCharacterPresentation.h"
#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlineCosmeticVisual.h"
#include "Presentation/AshlineEnvironmentKit.h"
#include "Presentation/AshlinePresentationSettings.h"
#include "Presentation/AshlineWeaponVisual.h"
#include "Meta/AshlineMetaCatalog.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/SoftObjectPath.h"

namespace AshlinePres
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
			if (T* Obj = AshlineLoad::Object<T>(Path))
			{
				return Obj;
			}
		}
		return nullptr;
	}

}

UMaterialInterface* UAshlinePresentationLibrary::LoadMaterial(const TArray<FString>& Paths)
{
	return AshlinePres::LoadFirst<UMaterialInterface>(Paths);
}

UStaticMesh* UAshlinePresentationLibrary::LoadStaticMesh(const TArray<FString>& Paths)
{
	return AshlinePres::LoadFirst<UStaticMesh>(Paths);
}

USkeletalMesh* UAshlinePresentationLibrary::LoadSkeletalMesh(const TArray<FString>& Paths)
{
	return AshlinePres::LoadFirst<USkeletalMesh>(Paths);
}

UMaterialInterface* UAshlinePresentationLibrary::GetSurfaceMaterial(EAshlineSurface Surface)
{
	switch (Surface)
	{
	case EAshlineSurface::Ground:
		return LoadMaterial({
			TEXT("/Game/StarterContent/Materials/M_Ground_Grass.M_Ground_Grass"),
			TEXT("/Game/StarterContent/Materials/M_Ground_Moss.M_Ground_Moss"),
			TEXT("/Engine/EngineMaterials/WorldGridMaterial.WorldGridMaterial"),
			TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	case EAshlineSurface::Concrete:
		return LoadMaterial({
			TEXT("/Game/StarterContent/Materials/M_Concrete_Tiles.M_Concrete_Tiles"),
			TEXT("/Game/StarterContent/Materials/M_Concrete_Poured.M_Concrete_Poured"),
			TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	case EAshlineSurface::Metal:
		return LoadMaterial({
			TEXT("/Game/StarterContent/Materials/M_Metal_Steel.M_Metal_Steel"),
			TEXT("/Game/StarterContent/Materials/M_Metal_Brushed.M_Metal_Brushed"),
			TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	case EAshlineSurface::Wood:
		return LoadMaterial({
			TEXT("/Game/StarterContent/Materials/M_Wood_Floor_Walnut_Polished.M_Wood_Floor_Walnut_Polished"),
			TEXT("/Game/StarterContent/Materials/M_Wood_Oak.M_Wood_Oak"),
			TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	case EAshlineSurface::Sand:
		return LoadMaterial({
			TEXT("/Game/StarterContent/Materials/M_Ground_Gravel.M_Ground_Gravel"),
			TEXT("/Engine/EngineMaterials/WorldGridMaterial.WorldGridMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	case EAshlineSurface::Snow:
		return LoadMaterial({
			TEXT("/Game/StarterContent/Materials/M_Rock_Marble.M_Rock_Marble"),
			TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	case EAshlineSurface::Water:
		return LoadMaterial({
			TEXT("/Game/StarterContent/Materials/M_Water_Ocean.M_Water_Ocean"),
			TEXT("/Game/StarterContent/Materials/M_Water_Lake.M_Water_Lake"),
			TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	case EAshlineSurface::Foliage:
		return LoadMaterial({
			TEXT("/Game/StarterContent/Materials/M_Ground_Grass.M_Ground_Grass"),
			TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	case EAshlineSurface::Emissive:
		return LoadMaterial({
			TEXT("/Engine/EngineMaterials/EmissiveMeshMaterial.EmissiveMeshMaterial"),
			TEXT("/Engine/EngineMaterials/DefaultUnlitMaterial.DefaultUnlitMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	case EAshlineSurface::Plastic:
	case EAshlineSurface::Auto:
	default:
		return LoadMaterial({
			TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"),
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
		});
	}
}

UMaterialInstanceDynamic* UAshlinePresentationLibrary::MakeTintedMaterial(UObject* Outer, EAshlineSurface Surface, const FLinearColor& Tint)
{
	UMaterialInterface* Base = GetSurfaceMaterial(Surface);
	if (!Base)
	{
		return nullptr;
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Outer);
	if (!MID)
	{
		return nullptr;
	}

	// Engine / StarterContent materials expose different parameter names. Set all common ones.
	MID->SetVectorParameterValue(TEXT("Color"), Tint);
	MID->SetVectorParameterValue(TEXT("BaseColor"), Tint);
	MID->SetVectorParameterValue(TEXT("Base Color"), Tint);
	MID->SetScalarParameterValue(TEXT("Roughness"), Surface == EAshlineSurface::Metal ? 0.35f : 0.72f);
	MID->SetScalarParameterValue(TEXT("Metallic"), Surface == EAshlineSurface::Metal ? 0.85f : 0.05f);
	if (Surface == EAshlineSurface::Emissive)
	{
		MID->SetVectorParameterValue(TEXT("EmissiveColor"), Tint);
		MID->SetScalarParameterValue(TEXT("EmissiveStrength"), 8.f);
	}
	return MID;
}

EAshlineSurface UAshlinePresentationLibrary::InferSurface(const FLinearColor& Color)
{
	const float Lum = Color.GetLuminance();
	if (Color.B > 0.28f && Color.B > Color.R && Lum < 0.28f)
	{
		return EAshlineSurface::Water;
	}
	if (Lum > 0.7f && Color.B >= Color.R)
	{
		return EAshlineSurface::Snow;
	}
	if (Color.R > 0.35f && Color.G > 0.22f && Color.B < 0.2f && Lum > 0.25f)
	{
		return EAshlineSurface::Sand;
	}
	if (Color.G > Color.R && Color.G > Color.B && Color.G > 0.2f)
	{
		return EAshlineSurface::Foliage;
	}
	if (Lum < 0.16f)
	{
		return EAshlineSurface::Metal;
	}
	if (Color.R > 0.25f && Color.G > 0.16f && Color.B < 0.16f)
	{
		return EAshlineSurface::Wood;
	}
	if (Lum < 0.28f)
	{
		return EAshlineSurface::Concrete;
	}
	return EAshlineSurface::Ground;
}

FAshlineLightingMood UAshlinePresentationLibrary::MoodForMission(EAshlineMissionId MissionId)
{
	FAshlineLightingMood Mood;
	switch (MissionId)
	{
	case EAshlineMissionId::ASH_01_WireCut:
		Mood.SunColor = FLinearColor(0.28f, 0.38f, 0.72f);
		Mood.SunIntensity = 2.4f;
		Mood.SunRotation = FRotator(-12.f, -40.f, 0.f);
		Mood.FogColor = FLinearColor(0.03f, 0.05f, 0.09f);
		Mood.FogDensity = 0.038f;
		Mood.FogStartDistance = 200.f;
		Mood.FogHeightFalloff = 0.12f;
		Mood.VolumetricFogExtinction = 0.85f;
		Mood.VolumetricScattering = 0.22f;
		Mood.SkyLightIntensity = 0.45f;
		Mood.SkyZenith = FLinearColor(0.01f, 0.02f, 0.06f);
		Mood.SkyHorizon = FLinearColor(0.08f, 0.1f, 0.18f);
		Mood.BloomIntensity = 0.55f;
		Mood.BloomThreshold = -0.6f;
		Mood.Vignette = 0.42f;
		Mood.ColorTempKelvin = 7800.f;
		Mood.ColorSaturation = FVector4(0.85f, 0.9f, 1.1f, 1.f);
		Mood.FilmGrain = 0.08f;
		Mood.ChromaticAberration = 0.35f;
		Mood.DirtMaskIntensity = 0.22f;
		Mood.LocalExposureHighlight = 0.65f;
		Mood.LocalExposureShadow = 0.9f;
		Mood.FillLightColor = FLinearColor(1.f, 0.55f, 0.22f);
		Mood.FillLightIntensity = 2.4f;
		Mood.FillLightRotation = FRotator(-8.f, 130.f, 0.f);
		Mood.MoonColor = FLinearColor(0.32f, 0.42f, 0.78f);
		Mood.MoonIntensity = 1.6f;
		Mood.FoliageDensity = 12;
		Mood.AutoExposureBias = -0.4f;
		Mood.bNight = true;
		Mood.GroundTint = FLinearColor(0.07f, 0.09f, 0.06f);
		Mood.WallTint = FLinearColor(0.16f, 0.15f, 0.12f);
		Mood.DefaultGround = EAshlineSurface::Ground;
		break;
	case EAshlineMissionId::ASH_02_DustMarket:
		Mood.SunColor = FLinearColor(1.f, 0.72f, 0.38f);
		Mood.SunIntensity = 10.f;
		Mood.SunRotation = FRotator(-52.f, 20.f, 0.f);
		Mood.FogColor = FLinearColor(0.42f, 0.3f, 0.14f);
		Mood.FogDensity = 0.024f;
		Mood.FogStartDistance = 80.f;
		Mood.VolumetricFogExtinction = 0.45f;
		Mood.SkyLightIntensity = 1.6f;
		Mood.SkyZenith = FLinearColor(0.25f, 0.45f, 0.7f);
		Mood.SkyHorizon = FLinearColor(0.85f, 0.55f, 0.25f);
		Mood.BloomIntensity = 0.4f;
		Mood.BloomThreshold = 0.2f;
		Mood.ColorTempKelvin = 4800.f;
		Mood.ColorSaturation = FVector4(1.15f, 1.05f, 0.85f, 1.f);
		Mood.ChromaticAberration = 0.18f;
		Mood.DirtMaskIntensity = 0.28f;
		Mood.FillLightColor = FLinearColor(1.f, 0.5f, 0.18f);
		Mood.FillLightIntensity = 3.2f;
		Mood.FoliageDensity = 6;
		Mood.AutoExposureBias = 0.2f;
		Mood.GroundTint = FLinearColor(0.36f, 0.26f, 0.14f);
		Mood.WallTint = FLinearColor(0.42f, 0.26f, 0.16f);
		Mood.DefaultGround = EAshlineSurface::Sand;
		break;
	case EAshlineMissionId::ASH_03_Holdfast:
		Mood.SunColor = FLinearColor(0.95f, 0.9f, 0.75f);
		Mood.SunIntensity = 11.f;
		Mood.SunRotation = FRotator(-58.f, -10.f, 0.f);
		Mood.FogColor = FLinearColor(0.45f, 0.42f, 0.32f);
		Mood.FogDensity = 0.014f;
		Mood.VolumetricFogExtinction = 0.28f;
		Mood.SkyLightIntensity = 1.8f;
		Mood.ColorTempKelvin = 6200.f;
		Mood.FillLightColor = FLinearColor(0.95f, 0.88f, 0.7f);
		Mood.FillLightIntensity = 2.0f;
		Mood.FoliageDensity = 10;
		Mood.AutoExposureBias = 0.15f;
		Mood.GroundTint = FLinearColor(0.24f, 0.21f, 0.14f);
		Mood.DefaultGround = EAshlineSurface::Sand;
		break;
	case EAshlineMissionId::ASH_04_NightGlass:
		Mood.SunColor = FLinearColor(0.18f, 0.28f, 0.62f);
		Mood.SunIntensity = 1.6f;
		Mood.SunRotation = FRotator(-8.f, 160.f, 0.f);
		Mood.FogColor = FLinearColor(0.03f, 0.04f, 0.08f);
		Mood.FogDensity = 0.042f;
		Mood.FogStartDistance = 160.f;
		Mood.VolumetricFogExtinction = 0.9f;
		Mood.SkyLightIntensity = 0.35f;
		Mood.BloomIntensity = 0.7f;
		Mood.Vignette = 0.5f;
		Mood.ColorTempKelvin = 9000.f;
		Mood.FilmGrain = 0.1f;
		Mood.ChromaticAberration = 0.45f;
		Mood.DirtMaskIntensity = 0.2f;
		Mood.FillLightColor = FLinearColor(1.f, 0.22f, 0.1f);
		Mood.FillLightIntensity = 2.8f;
		Mood.MoonColor = FLinearColor(0.28f, 0.4f, 0.8f);
		Mood.MoonIntensity = 2.0f;
		Mood.FoliageDensity = 4;
		Mood.AutoExposureBias = -0.5f;
		Mood.bNight = true;
		Mood.GroundTint = FLinearColor(0.1f, 0.1f, 0.12f);
		Mood.DefaultGround = EAshlineSurface::Concrete;
		break;
	case EAshlineMissionId::ASH_05_ConvoyGhost:
		Mood.SunColor = FLinearColor(0.95f, 0.78f, 0.42f);
		Mood.SunIntensity = 9.5f;
		Mood.SunRotation = FRotator(-28.f, 70.f, 0.f);
		Mood.FogColor = FLinearColor(0.5f, 0.42f, 0.22f);
		Mood.FogDensity = 0.02f;
		Mood.VolumetricFogExtinction = 0.42f;
		Mood.ColorTempKelvin = 5000.f;
		Mood.FillLightColor = FLinearColor(1.f, 0.35f, 0.08f);
		Mood.FillLightIntensity = 2.6f;
		Mood.FoliageDensity = 5;
		Mood.AutoExposureBias = 0.05f;
		Mood.GroundTint = FLinearColor(0.4f, 0.34f, 0.18f);
		Mood.DefaultGround = EAshlineSurface::Sand;
		break;
	case EAshlineMissionId::ASH_06_AshHarbor:
		Mood.SunColor = FLinearColor(0.62f, 0.74f, 0.88f);
		Mood.SunIntensity = 7.2f;
		Mood.SunRotation = FRotator(-34.f, -80.f, 0.f);
		Mood.FogColor = FLinearColor(0.16f, 0.22f, 0.28f);
		Mood.FogDensity = 0.026f;
		Mood.VolumetricFogExtinction = 0.55f;
		Mood.ColorTempKelvin = 7100.f;
		Mood.ColorSaturation = FVector4(0.9f, 0.95f, 1.1f, 1.f);
		Mood.FillLightColor = FLinearColor(0.5f, 0.7f, 1.f);
		Mood.FillLightIntensity = 2.2f;
		Mood.FoliageDensity = 3;
		Mood.AutoExposureBias = -0.05f;
		Mood.GroundTint = FLinearColor(0.16f, 0.17f, 0.18f);
		Mood.DefaultGround = EAshlineSurface::Concrete;
		break;
	case EAshlineMissionId::ASH_07_Whiteout:
		Mood.SunColor = FLinearColor(0.85f, 0.9f, 0.98f);
		Mood.SunIntensity = 3.2f;
		Mood.SunRotation = FRotator(-18.f, 10.f, 0.f);
		Mood.FogColor = FLinearColor(0.78f, 0.82f, 0.88f);
		Mood.FogDensity = 0.075f;
		Mood.FogStartDistance = 40.f;
		Mood.VolumetricFogExtinction = 1.15f;
		Mood.VolumetricScattering = 0.55f;
		Mood.SkyLightIntensity = 2.2f;
		Mood.BloomIntensity = 0.25f;
		Mood.Vignette = 0.18f;
		Mood.ColorTempKelvin = 8500.f;
		Mood.ColorSaturation = FVector4(0.75f, 0.8f, 0.95f, 1.f);
		Mood.LocalExposureHighlight = 0.55f;
		Mood.FillLightColor = FLinearColor(0.7f, 0.82f, 1.f);
		Mood.FillLightIntensity = 3.4f;
		Mood.FoliageDensity = 7;
		Mood.AutoExposureBias = 0.35f;
		Mood.GroundTint = FLinearColor(0.82f, 0.85f, 0.9f);
		Mood.DefaultGround = EAshlineSurface::Snow;
		break;
	case EAshlineMissionId::ASH_08_Catacomb:
		Mood.SunColor = FLinearColor(0.32f, 0.24f, 0.16f);
		Mood.SunIntensity = 0.8f;
		Mood.SunRotation = FRotator(-70.f, 0.f, 0.f);
		Mood.FogColor = FLinearColor(0.04f, 0.03f, 0.02f);
		Mood.FogDensity = 0.055f;
		Mood.VolumetricFogExtinction = 1.0f;
		Mood.SkyLightIntensity = 0.2f;
		Mood.BloomIntensity = 0.45f;
		Mood.Vignette = 0.55f;
		Mood.ColorTempKelvin = 3200.f;
		Mood.FilmGrain = 0.09f;
		Mood.ChromaticAberration = 0.3f;
		Mood.DirtMaskIntensity = 0.35f;
		Mood.FillLightColor = FLinearColor(1.f, 0.45f, 0.15f);
		Mood.FillLightIntensity = 1.8f;
		Mood.FoliageDensity = 0;
		Mood.AutoExposureBias = -0.55f;
		Mood.bNight = true;
		Mood.GroundTint = FLinearColor(0.1f, 0.08f, 0.06f);
		Mood.DefaultGround = EAshlineSurface::Concrete;
		break;
	case EAshlineMissionId::ASH_09_RidgeWire:
		Mood.SunColor = FLinearColor(0.7f, 0.8f, 0.92f);
		Mood.SunIntensity = 8.5f;
		Mood.SunRotation = FRotator(-40.f, 120.f, 0.f);
		Mood.FogColor = FLinearColor(0.32f, 0.38f, 0.42f);
		Mood.FogDensity = 0.022f;
		Mood.VolumetricFogExtinction = 0.38f;
		Mood.ColorTempKelvin = 6800.f;
		Mood.FillLightColor = FLinearColor(0.55f, 0.75f, 1.f);
		Mood.FillLightIntensity = 2.4f;
		Mood.FoliageDensity = 9;
		Mood.AutoExposureBias = 0.1f;
		Mood.GroundTint = FLinearColor(0.2f, 0.23f, 0.16f);
		Mood.DefaultGround = EAshlineSurface::Ground;
		break;
	case EAshlineMissionId::ASH_10_FalseFlag:
		Mood.SunColor = FLinearColor(0.16f, 0.2f, 0.4f);
		Mood.SunIntensity = 1.8f;
		Mood.SunRotation = FRotator(-10.f, 200.f, 0.f);
		Mood.FogColor = FLinearColor(0.05f, 0.05f, 0.09f);
		Mood.FogDensity = 0.032f;
		Mood.VolumetricFogExtinction = 0.72f;
		Mood.SkyLightIntensity = 0.4f;
		Mood.BloomIntensity = 0.6f;
		Mood.Vignette = 0.4f;
		Mood.ColorTempKelvin = 8200.f;
		Mood.ChromaticAberration = 0.32f;
		Mood.FillLightColor = FLinearColor(1.f, 0.15f, 0.1f);
		Mood.FillLightIntensity = 2.5f;
		Mood.MoonIntensity = 1.4f;
		Mood.FoliageDensity = 4;
		Mood.AutoExposureBias = -0.35f;
		Mood.bNight = true;
		Mood.GroundTint = FLinearColor(0.09f, 0.09f, 0.11f);
		Mood.DefaultGround = EAshlineSurface::Concrete;
		break;
	case EAshlineMissionId::ASH_11_LastTrain:
		Mood.SunColor = FLinearColor(0.45f, 0.48f, 0.52f);
		Mood.SunIntensity = 4.2f;
		Mood.SunRotation = FRotator(-22.f, -100.f, 0.f);
		Mood.FogColor = FLinearColor(0.1f, 0.1f, 0.1f);
		Mood.FogDensity = 0.03f;
		Mood.VolumetricFogExtinction = 0.6f;
		Mood.ColorTempKelvin = 5600.f;
		Mood.FilmGrain = 0.06f;
		Mood.DirtMaskIntensity = 0.3f;
		Mood.FillLightColor = FLinearColor(1.f, 0.7f, 0.3f);
		Mood.FillLightIntensity = 2.0f;
		Mood.FoliageDensity = 2;
		Mood.AutoExposureBias = -0.1f;
		Mood.GroundTint = FLinearColor(0.14f, 0.14f, 0.13f);
		Mood.DefaultGround = EAshlineSurface::Concrete;
		break;
	case EAshlineMissionId::ASH_12_Ashline:
		Mood.SunColor = FLinearColor(0.7f, 0.12f, 0.08f);
		Mood.SunIntensity = 2.8f;
		Mood.SunRotation = FRotator(-16.f, 30.f, 0.f);
		Mood.FogColor = FLinearColor(0.12f, 0.02f, 0.015f);
		Mood.FogDensity = 0.048f;
		Mood.FogStartDistance = 120.f;
		Mood.VolumetricFogExtinction = 0.95f;
		Mood.SkyLightIntensity = 0.55f;
		Mood.BloomIntensity = 0.85f;
		Mood.BloomThreshold = -0.8f;
		Mood.Vignette = 0.48f;
		Mood.ColorTempKelvin = 2800.f;
		Mood.ColorSaturation = FVector4(1.25f, 0.75f, 0.7f, 1.f);
		Mood.FilmGrain = 0.11f;
		Mood.ChromaticAberration = 0.4f;
		Mood.DirtMaskIntensity = 0.25f;
		Mood.FillLightColor = FLinearColor(1.f, 0.16f, 0.06f);
		Mood.FillLightIntensity = 3.6f;
		Mood.MoonColor = FLinearColor(0.7f, 0.1f, 0.08f);
		Mood.MoonIntensity = 1.2f;
		Mood.FoliageDensity = 0;
		Mood.AutoExposureBias = -0.45f;
		Mood.bNight = true;
		Mood.GroundTint = FLinearColor(0.07f, 0.05f, 0.05f);
		Mood.DefaultGround = EAshlineSurface::Concrete;
		break;
	default:
		break;
	}
	return Mood;
}

UAshlineEnvironmentKit* UAshlinePresentationLibrary::FindEnvironmentKit(EAshlineMissionId MissionId)
{
	if (const UAshlinePresentationSettings* Settings = GetDefault<UAshlinePresentationSettings>())
	{
		if (const TSoftObjectPtr<UAshlineEnvironmentKit>* Found = Settings->MissionKits.Find(MissionId))
		{
			if (UAshlineEnvironmentKit* Kit = AshlineLoad::Soft(Found))
			{
				return Kit;
			}
		}
	}
	return AshlineLoad::Object<UAshlineEnvironmentKit>(UAshlineContentManifest::KitDataAssetPath(MissionId));
}

UAshlineCharacterPresentation* UAshlinePresentationLibrary::FindCharacterPresentation(bool bHero, EAshlineAIArchetype Archetype)
{
	if (bHero)
	{
		if (const UAshlinePresentationSettings* Settings = GetDefault<UAshlinePresentationSettings>())
		{
			if (UAshlineCharacterPresentation* Hero = AshlineLoad::Soft(Settings->HeroPresentation))
			{
				return Hero;
			}
		}
		return AshlineLoad::Object<UAshlineCharacterPresentation>(TEXT("/Game/Ashline/Data/Kits/DA_Hero_Operator.DA_Hero_Operator"));
	}

	if (UAshlineCharacterPresentation* Named = AshlineLoad::Object<UAshlineCharacterPresentation>(UAshlineContentManifest::AIPresentationPath(Archetype)))
	{
		return Named;
	}
	return AshlineLoad::Object<UAshlineCharacterPresentation>(TEXT("/Game/Ashline/Data/Kits/DA_AI_Rifleman.DA_AI_Rifleman"));
}

UAshlineWeaponVisual* UAshlinePresentationLibrary::FindWeaponVisual(FName WeaponId)
{
	if (const UAshlinePresentationSettings* Settings = GetDefault<UAshlinePresentationSettings>())
	{
		if (const TSoftObjectPtr<UAshlineWeaponVisual>* Found = Settings->WeaponVisuals.Find(WeaponId))
		{
			if (UAshlineWeaponVisual* Visual = AshlineLoad::Soft(Found))
			{
				return Visual;
			}
		}
	}
	const FString Path = FString::Printf(TEXT("/Game/Ashline/Data/Kits/DA_WPN_%s.DA_WPN_%s"), *WeaponId.ToString(), *WeaponId.ToString());
	return AshlineLoad::Object<UAshlineWeaponVisual>(Path);
}

USkeletalMesh* UAshlinePresentationLibrary::ResolveHumanoidMesh()
{
	if (const UAshlinePresentationSettings* Settings = GetDefault<UAshlinePresentationSettings>())
	{
		if (USkeletalMesh* Configured = AshlineLoad::Soft(Settings->DefaultHeroMesh))
		{
			return Configured;
		}
	}
	return LoadSkeletalMesh(UAshlineContentManifest::HumanoidMeshCandidates());
}

UStaticMesh* UAshlinePresentationLibrary::ResolveWeaponPlaceholderMesh()
{
	if (const UAshlinePresentationSettings* Settings = GetDefault<UAshlinePresentationSettings>())
	{
		if (UStaticMesh* Configured = AshlineLoad::Soft(Settings->PlaceholderWeaponMesh))
		{
			return Configured;
		}
	}
	return LoadStaticMesh({
		UAshlineContentManifest::WeaponMeshPath(TEXT("WPN_AR_ASH16")),
		TEXT("/Game/Ashline/Weapons/Meshes/SM_ASH16.SM_ASH16"),
		TEXT("/Game/StarterContent/Props/SM_CornerFrame.SM_CornerFrame"),
		TEXT("/Engine/BasicShapes/Cube.Cube")
	});
}

UMaterialInterface* UAshlinePresentationLibrary::ResolveImpactDecalMaterial()
{
	return LoadMaterial({
		TEXT("/Game/Ashline/Materials/Decals/M_Impact_Bullet.M_Impact_Bullet"),
		TEXT("/Game/StarterContent/Decals/M_Decal_BulletHole.M_Decal_BulletHole"),
		TEXT("/Engine/EngineMaterials/DefaultDeferredDecalMaterial.DefaultDeferredDecalMaterial")
	});
}

FString UAshlinePresentationLibrary::DescribeFallbackChain()
{
	return TEXT("Authored /Game/Ashline DataAssets → StarterContent (if Add Content Pack) → Engine materials/meshes. Fab/Quixel/MetaHuman binaries are not in git.");
}

void UAshlinePresentationLibrary::ApplyHumanoidBlockout(ACharacter* Character, const FLinearColor& Tint)
{
	if (!Character)
	{
		return;
	}

	UStaticMesh* Cube = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* Sphere = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	UStaticMesh* Cyl = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (!Cube)
	{
		return;
	}

	USceneComponent* Root = Character->GetCapsuleComponent();
	if (!Root)
	{
		Root = Character->GetRootComponent();
	}

	auto Part = [&](const TCHAR* Name, UStaticMesh* MeshAsset, const FVector& Rel, const FVector& Scale, const FLinearColor& Color)
	{
		TArray<UStaticMeshComponent*> Existing;
		Character->GetComponents<UStaticMeshComponent>(Existing);
		for (UStaticMeshComponent* MeshComp : Existing)
		{
			if (MeshComp && MeshComp->GetFName() == Name)
			{
				if (UMaterialInstanceDynamic* MID = MakeTintedMaterial(Character, EAshlineSurface::Plastic, Color))
				{
					MeshComp->SetMaterial(0, MID);
				}
				return;
			}
		}
		UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(Character, Name);
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->SetCastShadow(true);
		Comp->SetupAttachment(Root);
		Comp->SetRelativeLocation(Rel);
		Comp->SetRelativeScale3D(Scale);
		Comp->RegisterComponent();
		if (MeshAsset)
		{
			Comp->SetStaticMesh(MeshAsset);
		}
		if (UMaterialInstanceDynamic* MID = MakeTintedMaterial(Character, EAshlineSurface::Plastic, Color))
		{
			Comp->SetMaterial(0, MID);
		}
	};

	const FLinearColor Dark = Tint * 0.65f;
	Part(TEXT("AshlineBlock_Torso"), Cube, FVector(0.f, 0.f, 10.f), FVector(0.38f, 0.28f, 0.55f), Tint);
	Part(TEXT("AshlineBlock_Head"), Sphere, FVector(0.f, 0.f, 52.f), FVector(0.28f, 0.28f, 0.32f), Tint * 1.1f);
	Part(TEXT("AshlineBlock_Helmet"), Sphere, FVector(2.f, 0.f, 58.f), FVector(0.32f, 0.3f, 0.18f), Dark);
	Part(TEXT("AshlineBlock_ArmL"), Cube, FVector(0.f, 22.f, 18.f), FVector(0.12f, 0.12f, 0.42f), Dark);
	Part(TEXT("AshlineBlock_ArmR"), Cube, FVector(0.f, -22.f, 18.f), FVector(0.12f, 0.12f, 0.42f), Dark);
	Part(TEXT("AshlineBlock_LegL"), Cyl ? Cyl : Cube, FVector(0.f, 10.f, -40.f), FVector(0.16f, 0.16f, 0.45f), Dark);
	Part(TEXT("AshlineBlock_LegR"), Cyl ? Cyl : Cube, FVector(0.f, -10.f, -40.f), FVector(0.16f, 0.16f, 0.45f), Dark);
}

void UAshlinePresentationLibrary::TintNamedStaticMesh(AActor* Actor, FName ComponentName, const FLinearColor& Tint, EAshlineSurface Surface)
{
	if (!Actor)
	{
		return;
	}
	TArray<UStaticMeshComponent*> Meshes;
	Actor->GetComponents<UStaticMeshComponent>(Meshes);
	for (UStaticMeshComponent* MeshComp : Meshes)
	{
		if (MeshComp && MeshComp->GetFName() == ComponentName)
		{
			if (UMaterialInstanceDynamic* MID = MakeTintedMaterial(Actor, Surface, Tint))
			{
				MeshComp->SetMaterial(0, MID);
			}
			return;
		}
	}
}

USkeletalMesh* UAshlinePresentationLibrary::ResolveCosmeticMesh(const FAshlineCosmeticDefinition& Cosmetic)
{
	if (USkeletalMesh* Mesh = AshlineLoad::Soft(Cosmetic.MeshOverride))
	{
		return Mesh;
	}
	return LoadSkeletalMesh({
		UAshlineContentManifest::CosmeticMeshPath(Cosmetic.CosmeticId)
	});
}

UStaticMesh* UAshlinePresentationLibrary::ResolveCosmeticPartMesh(const FAshlineCosmeticDefinition& Cosmetic)
{
	if (UAshlineCosmeticVisual* Visual = AshlineLoad::Object<UAshlineCosmeticVisual>(UAshlineContentManifest::CosmeticDataAssetPath(Cosmetic.CosmeticId)))
	{
		if (UStaticMesh* Part = AshlineLoad::Soft(Visual->PartMesh))
		{
			return Part;
		}
	}
	return LoadStaticMesh({
		UAshlineContentManifest::ClothingPartPath(Cosmetic.Slot, Cosmetic.CosmeticId),
		UAshlineContentManifest::CosmeticStaticMeshPath(Cosmetic.CosmeticId)
	});
}

UMaterialInterface* UAshlinePresentationLibrary::ResolveCosmeticMaterial(const FAshlineCosmeticDefinition& Cosmetic)
{
	if (UMaterialInterface* Mat = AshlineLoad::Soft(Cosmetic.MaterialOverride))
	{
		return Mat;
	}
	return LoadMaterial({
		UAshlineContentManifest::CosmeticMaterialPath(Cosmetic.CosmeticId)
	});
}

UMaterialInterface* UAshlinePresentationLibrary::ResolveSkinMaterial(const FAshlineWeaponSkinDefinition& Skin)
{
	if (UMaterialInterface* Mat = AshlineLoad::Soft(Skin.MaterialOverride))
	{
		return Mat;
	}
	return LoadMaterial({
		UAshlineContentManifest::SkinMaterialPath(Skin.SkinId)
	});
}

UStaticMesh* UAshlinePresentationLibrary::ResolveFoliageMesh()
{
	return LoadStaticMesh(UAshlineContentManifest::FoliageMeshCandidates());
}

UStaticMesh* UAshlinePresentationLibrary::ResolveTreeMesh()
{
	return LoadStaticMesh(UAshlineContentManifest::TreeMeshCandidates());
}

FName UAshlinePresentationLibrary::ClothingComponentName(EAshlineCosmeticSlot Slot)
{
	switch (Slot)
	{
	case EAshlineCosmeticSlot::Helmet: return TEXT("AshlineCloth_Helmet");
	case EAshlineCosmeticSlot::Vest: return TEXT("AshlineCloth_Vest");
	case EAshlineCosmeticSlot::Pants: return TEXT("AshlineCloth_Pants");
	case EAshlineCosmeticSlot::Gloves: return TEXT("AshlineCloth_Gloves");
	case EAshlineCosmeticSlot::Boots: return TEXT("AshlineCloth_Boots");
	case EAshlineCosmeticSlot::Face: return TEXT("AshlineCloth_Face");
	case EAshlineCosmeticSlot::Charm: return TEXT("AshlineCloth_Charm");
	default: return TEXT("AshlineCloth_Other");
	}
}

void UAshlinePresentationLibrary::HideCapsuleVisual(ACharacter* Character)
{
	// Collision capsule stays. It has no mesh. Do not SetHiddenInGame — FPS camera
	// and other children attach to it. "No capsules" means we never use the capsule
	// as the visible body (mannequin / clothing / blockout instead).
	if (!Character)
	{
		return;
	}
	(void)Character;
}

void UAshlinePresentationLibrary::ApplyClothingPart(ACharacter* Character, EAshlineCosmeticSlot Slot, FName CosmeticId, const FLinearColor& Tint)
{
	if (!Character || CosmeticId.IsNone() || Slot == EAshlineCosmeticSlot::Camo || Slot == EAshlineCosmeticSlot::Voice)
	{
		return;
	}

	FAshlineCosmeticDefinition Def;
	if (!UAshlineMetaCatalog::FindCosmetic(CosmeticId, Def))
	{
		return;
	}

	UStaticMesh* PartMesh = ResolveCosmeticPartMesh(Def);
	UMaterialInterface* Override = ResolveCosmeticMaterial(Def);
	if (!PartMesh)
	{
		return;
	}

	const FName CompName = ClothingComponentName(Slot);
	UStaticMeshComponent* Comp = nullptr;
	TArray<UStaticMeshComponent*> Existing;
	Character->GetComponents<UStaticMeshComponent>(Existing);
	for (UStaticMeshComponent* MeshComp : Existing)
	{
		if (MeshComp && MeshComp->GetFName() == CompName)
		{
			Comp = MeshComp;
			break;
		}
	}

	USceneComponent* AttachParent = Character->GetMesh();
	if (!AttachParent || !Character->GetMesh()->GetSkeletalMeshAsset())
	{
		AttachParent = Character->GetCapsuleComponent();
	}

	if (!Comp)
	{
		Comp = NewObject<UStaticMeshComponent>(Character, CompName);
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->SetCastShadow(true);
		Comp->SetupAttachment(AttachParent);
		Comp->RegisterComponent();
	}

	FVector Rel(0.f, 0.f, 0.f);
	FVector Scale(0.22f, 0.22f, 0.18f);
	switch (Slot)
	{
	case EAshlineCosmeticSlot::Helmet:
		Rel = FVector(2.f, 0.f, 58.f);
		Scale = FVector(0.32f, 0.3f, 0.18f);
		break;
	case EAshlineCosmeticSlot::Vest:
		Rel = FVector(0.f, 0.f, 16.f);
		Scale = FVector(0.4f, 0.3f, 0.42f);
		break;
	case EAshlineCosmeticSlot::Pants:
		Rel = FVector(0.f, 0.f, -36.f);
		Scale = FVector(0.28f, 0.22f, 0.4f);
		break;
	case EAshlineCosmeticSlot::Gloves:
		Rel = FVector(0.f, 22.f, 18.f);
		Scale = FVector(0.1f, 0.1f, 0.12f);
		break;
	case EAshlineCosmeticSlot::Boots:
		Rel = FVector(0.f, 10.f, -62.f);
		Scale = FVector(0.14f, 0.12f, 0.12f);
		break;
	case EAshlineCosmeticSlot::Face:
		Rel = FVector(4.f, 0.f, 50.f);
		Scale = FVector(0.2f, 0.18f, 0.2f);
		break;
	case EAshlineCosmeticSlot::Charm:
		Rel = FVector(8.f, 6.f, 8.f);
		Scale = FVector(0.06f, 0.06f, 0.08f);
		break;
	default:
		break;
	}

	if (UAshlineCosmeticVisual* Visual = AshlineLoad::Object<UAshlineCosmeticVisual>(UAshlineContentManifest::CosmeticDataAssetPath(CosmeticId)))
	{
		if (!Visual->RelativeLocation.IsNearlyZero())
		{
			Rel = Visual->RelativeLocation;
		}
		if (!Visual->RelativeScale.Equals(FVector::OneVector))
		{
			Scale = Visual->RelativeScale;
		}
		if (!Visual->AttachSocket.IsNone() && Character->GetMesh() && Character->GetMesh()->GetSkeletalMeshAsset())
		{
			Comp->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Visual->AttachSocket);
		}
	}

	Comp->SetStaticMesh(PartMesh);
	Comp->SetRelativeLocation(Rel);
	Comp->SetRelativeScale3D(Scale);
	Comp->SetVisibility(true);
	Comp->SetHiddenInGame(false);
	if (Override)
	{
		Comp->SetMaterial(0, Override);
	}
	else if (UMaterialInstanceDynamic* MID = MakeTintedMaterial(Character, EAshlineSurface::Plastic, Tint))
	{
		Comp->SetMaterial(0, MID);
	}
}
