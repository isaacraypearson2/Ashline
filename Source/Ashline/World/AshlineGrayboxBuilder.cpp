#include "World/AshlineGrayboxBuilder.h"

#include "AI/AshlineAICatalog.h"
#include "AI/AshlineAICharacter.h"
#include "AI/AshlineCoverPoint.h"
#include "Campaign/AshlineObjectiveTrigger.h"
#include "Engine/CollisionProfile.h"
#include "Camera/CameraActor.h"
#include "Components/BoxComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Engine/DecalActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/GameInstance.h"
#include "Engine/Light.h"
#include "Engine/PointLight.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/SpotLight.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Presentation/AshlineAudioDirector.h"
#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlineEnvironmentKit.h"
#include "Presentation/AshlineLoad.h"
#include "Presentation/AshlineMaterialFactory.h"
#include "Presentation/AshlinePresentationLibrary.h"
#include "Progression/AshlineProgressionSubsystem.h"

AAshlineGrayboxBuilder::AAshlineGrayboxBuilder()
{
	PrimaryActorTick.bCanEverTick = false;
	bNetLoadOnClient = false;
}

void AAshlineGrayboxBuilder::BeginPlay()
{
	Super::BeginPlay();
	LoadPrimitives();
}

void AAshlineGrayboxBuilder::LoadPrimitives()
{
	if (!CubeMesh)
	{
		CubeMesh = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Cube.Cube"));
	}
	if (!CylinderMesh)
	{
		CylinderMesh = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	}
	if (!SphereMesh)
	{
		SphereMesh = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	}
	if (!ShapeMaterial)
	{
		ShapeMaterial = AshlineLoad::Object<UMaterialInterface>(TEXT("/Engine/EngineMaterials/WorldGridMaterial.WorldGridMaterial"));
		if (!ShapeMaterial)
		{
			ShapeMaterial = AshlineLoad::Object<UMaterialInterface>(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		}
		if (!ShapeMaterial)
		{
			ShapeMaterial = UAshlinePresentationLibrary::GetSurfaceMaterial(EAshlineSurface::Concrete);
		}
	}
}

void AAshlineGrayboxBuilder::ClearBuiltActors()
{
	for (AActor* Actor : BuiltActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	BuiltActors.Reset();
	FrontendCamera = nullptr;
}

void AAshlineGrayboxBuilder::BuildFrontendHub()
{
	LoadPrimitives();
	ClearBuiltActors();
	CachedDifficulty = EAshlineDifficulty::Regular;
	ActiveMood = FAshlineLightingMood();
	ActiveMood.SunColor = FLinearColor(0.82f, 0.72f, 0.52f);
	ActiveMood.SunIntensity = 7.5f;
	ActiveMood.FogColor = FLinearColor(0.22f, 0.2f, 0.16f);
	ActiveMood.FogDensity = 0.018f;
	ActiveMood.ColorTempKelvin = 5200.f;
	ActiveMood.Vignette = 0.32f;
	ActiveMood.GroundTint = FLinearColor(0.16f, 0.15f, 0.12f);
	SpawnFullAtmosphere();

	Floor(FVector::ZeroVector, FVector2D(14000.f, 14000.f), FLinearColor(0.18f, 0.17f, 0.14f));
	Building(FVector(0.f, 0.f, 0.f), FVector2D(900.f, 280.f), 220.f, FLinearColor(0.35f, 0.28f, 0.16f));
	Building(FVector(-900.f, 1100.f, 0.f), FVector2D(420.f, 420.f), 260.f, FLinearColor(0.22f, 0.24f, 0.2f));
	Building(FVector(1100.f, -800.f, 0.f), FVector2D(360.f, 620.f), 200.f, FLinearColor(0.28f, 0.2f, 0.14f));
	Building(FVector(1600.f, 1400.f, 0.f), FVector2D(500.f, 360.f), 180.f, FLinearColor(0.2f, 0.18f, 0.14f));
	Cylinder(FVector(0.f, 0.f, 280.f), FVector(0.4f, 0.4f, 3.2f), FLinearColor(0.7f, 0.55f, 0.2f), EAshlineSurface::Metal);
	Practical(FVector(0.f, 0.f, 480.f), FLinearColor(1.f, 0.78f, 0.4f), 8000.f, 2200.f);
	Watchtower(FVector(-1800.f, 1600.f, 0.f), 720.f);
	TreeBelt(FVector(-2400.f, -2200.f, 0.f), FVector(2400.f, -2200.f, 0.f), 8, 360.f);
	TreeBelt(FVector(-2400.f, 2200.f, 0.f), FVector(2400.f, 2200.f, 0.f), 8, 340.f);
	Bush(FVector(500.f, 600.f, 0.f));
	CoverLine(FVector(-600.f, -400.f, 0.f), FVector(600.f, 400.f, 0.f), 5);
	DecalMark(FVector(0.f, 0.f, 4.f), FRotator(-90.f, 0.f, 0.f), FVector(400.f, 400.f, 20.f));
	ScatterKitProps(FVector::ZeroVector, 1800.f, 10);
	ScatterDress(FVector::ZeroVector, 1400.f, 8);
	StreetLamp(FVector(-800.f, -600.f, 0.f), 420.f);

	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FrontendCamera = World->SpawnActor<ACameraActor>(FVector(-2400.f, 0.f, 520.f), FRotator(-14.f, 0.f, 0.f), Params);
		if (FrontendCamera)
		{
			BuiltActors.Add(FrontendCamera);
		}
	}

	PlayerStartAt(FVector(-400.f, 0.f, 120.f), FRotator::ZeroRotator);
}

void AAshlineGrayboxBuilder::BuildMission(EAshlineMissionId MissionId)
{
	LoadPrimitives();
	ClearBuiltActors();

	CachedDifficulty = EAshlineDifficulty::Regular;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				CachedDifficulty = Save->Difficulty;
			}
		}
	}

	ApplyMissionMood(MissionId);

	switch (MissionId)
	{
	case EAshlineMissionId::ASH_01_WireCut:
		BuildWireCut();
		break;
	case EAshlineMissionId::ASH_02_DustMarket:
		BuildDustMarket();
		break;
	case EAshlineMissionId::ASH_03_Holdfast:
		BuildHoldfast();
		break;
	case EAshlineMissionId::ASH_04_NightGlass:
		BuildNightGlass();
		break;
	case EAshlineMissionId::ASH_05_ConvoyGhost:
		BuildConvoyGhost();
		break;
	case EAshlineMissionId::ASH_06_AshHarbor:
		BuildAshHarbor();
		break;
	case EAshlineMissionId::ASH_07_Whiteout:
		BuildWhiteout();
		break;
	case EAshlineMissionId::ASH_08_Catacomb:
		BuildCatacomb();
		break;
	case EAshlineMissionId::ASH_09_RidgeWire:
		BuildRidgeWire();
		break;
	case EAshlineMissionId::ASH_10_FalseFlag:
		BuildFalseFlag();
		break;
	case EAshlineMissionId::ASH_11_LastTrain:
		BuildLastTrain();
		break;
	case EAshlineMissionId::ASH_12_Ashline:
		BuildAshlineFinale();
		break;
	default:
		BuildWireCut();
		break;
	}

	ExtraAIAround(LastPlayerStartLocation + FVector(6200.f, 0.f, 0.f), 5200.f);
	DressMission(MissionId);
}

void AAshlineGrayboxBuilder::SpawnAtmosphere(const FLinearColor& SunColor, float SunIntensity, const FLinearColor& FogColor, float FogDensity)
{
	// Mission kits already filled ActiveMood. Keep the passed args as a fallback
	// if ApplyMissionMood was not called (frontend / tests).
	if (ActiveMood.SunIntensity <= 0.01f)
	{
		ActiveMood.SunColor = SunColor;
		ActiveMood.SunIntensity = SunIntensity;
		ActiveMood.FogColor = FogColor;
		ActiveMood.FogDensity = FogDensity;
	}
	SpawnFullAtmosphere();
}

AActor* AAshlineGrayboxBuilder::Box(const FVector& Location, const FVector& Scale, const FLinearColor& Color, bool bCollision, EAshlineSurface Surface)
{
	UWorld* World = GetWorld();
	if (!World || !CubeMesh)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator, Params);
	if (!Actor)
	{
		return nullptr;
	}

	if (UStaticMeshComponent* Mesh = Actor->GetStaticMeshComponent())
	{
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetStaticMesh(CubeMesh);
		Mesh->SetWorldScale3D(Scale);
		Mesh->SetMobility(EComponentMobility::Stationary);
		Mesh->SetCollisionEnabled(bCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
		if (bCollision)
		{
			Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		}
		ApplySurfaceMaterial(Mesh, Actor, Color, Surface);
	}
	BuiltActors.Add(Actor);
	return Actor;
}

AActor* AAshlineGrayboxBuilder::Cylinder(const FVector& Location, const FVector& Scale, const FLinearColor& Color, EAshlineSurface Surface)
{
	UWorld* World = GetWorld();
	if (!World || !CylinderMesh)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator, Params);
	if (!Actor)
	{
		return nullptr;
	}
	if (UStaticMeshComponent* Mesh = Actor->GetStaticMeshComponent())
	{
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetStaticMesh(CylinderMesh);
		Mesh->SetWorldScale3D(Scale);
		Mesh->SetMobility(EComponentMobility::Stationary);
		Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		ApplySurfaceMaterial(Mesh, Actor, Color, Surface);
	}
	BuiltActors.Add(Actor);
	return Actor;
}

AActor* AAshlineGrayboxBuilder::Sphere(const FVector& Location, const FVector& Scale, const FLinearColor& Color, EAshlineSurface Surface)
{
	UWorld* World = GetWorld();
	if (!World || !SphereMesh)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator, Params);
	if (!Actor)
	{
		return nullptr;
	}
	if (UStaticMeshComponent* Mesh = Actor->GetStaticMeshComponent())
	{
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetStaticMesh(SphereMesh);
		Mesh->SetWorldScale3D(Scale);
		Mesh->SetMobility(EComponentMobility::Stationary);
		Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		ApplySurfaceMaterial(Mesh, Actor, Color, Surface);
	}
	BuiltActors.Add(Actor);
	return Actor;
}

void AAshlineGrayboxBuilder::Floor(const FVector& Center, const FVector2D& Size, const FLinearColor& Color)
{
	// Engine cube is 100uu. Thickness 20uu, top sits at Center.Z.
	const FVector Scale(Size.X / 100.f, Size.Y / 100.f, 0.2f);
	Box(Center + FVector(0.f, 0.f, -10.f), Scale, Color, true, ActiveMood.DefaultGround);
}

void AAshlineGrayboxBuilder::Wall(const FVector& Location, const FVector& Scale, const FLinearColor& Color)
{
	Box(Location, Scale, Color, true, EAshlineSurface::Concrete);
}

void AAshlineGrayboxBuilder::Cover(const FVector& Location)
{
	Box(Location + FVector(0.f, 0.f, 50.f), FVector(1.4f, 2.2f, 1.1f), FLinearColor(0.28f, 0.26f, 0.22f), true, EAshlineSurface::Concrete);
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (AAshlineCoverPoint* Point = World->SpawnActor<AAshlineCoverPoint>(Location + FVector(0.f, 0.f, 20.f), FRotator::ZeroRotator, Params))
		{
			BuiltActors.Add(Point);
		}
	}
}

void AAshlineGrayboxBuilder::PlayerStartAt(const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	LastPlayerStartLocation = Location;
	LastPlayerStartRotation = Rotation;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (APlayerStart* Start = World->SpawnActor<APlayerStart>(Location, Rotation, Params))
	{
		BuiltActors.Add(Start);
	}
}

void AAshlineGrayboxBuilder::SpawnAI(const FVector& Location, EAshlineAIArchetype Archetype)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if (AAshlineAICharacter* Bot = World->SpawnActor<AAshlineAICharacter>(Location, FRotator::ZeroRotator, Params))
	{
		Bot->Archetype = Archetype;
		BuiltActors.Add(Bot);
	}
}

void AAshlineGrayboxBuilder::Objective(FName Id, const FVector& Location, bool bCompleteMission, const FLinearColor& Color)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (AAshlineObjectiveTrigger* Trigger = World->SpawnActor<AAshlineObjectiveTrigger>(Location, FRotator::ZeroRotator, Params))
	{
		Trigger->ObjectiveId = Id;
		Trigger->bCompleteMissionOnEnter = bCompleteMission;
		Trigger->StarsIfMissionComplete = 2;
		if (Trigger->Volume)
		{
			Trigger->Volume->SetBoxExtent(FVector(180.f, 180.f, 140.f));
		}
		BuiltActors.Add(Trigger);
	}

	Box(Location + FVector(0.f, 0.f, 40.f), FVector(0.7f, 0.7f, 0.7f), Color, false, EAshlineSurface::Emissive);
	Practical(Location + FVector(0.f, 0.f, 90.f), Color, 3500.f, 420.f, false);
}

void AAshlineGrayboxBuilder::ExtraAIAround(const FVector& Origin, float Radius)
{
	const int32 Extra = UAshlineAICatalog::GetDifficulty(CachedDifficulty).ExtraAICount;
	for (int32 i = 0; i < Extra; ++i)
	{
		const float Angle = (2.f * PI * i) / FMath::Max(1, Extra);
		const FVector Loc = Origin + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 100.f);
		SpawnAI(Loc, EAshlineAIArchetype::Rifleman);
	}
}

void AAshlineGrayboxBuilder::ApplyMissionMood(EAshlineMissionId MissionId)
{
	ActiveMissionId = MissionId;
	ActiveMood = UAshlinePresentationLibrary::MoodForMission(MissionId);
	ActiveKit = UAshlinePresentationLibrary::FindEnvironmentKit(MissionId);
	if (ActiveKit && ActiveKit->bOverrideMood)
	{
		ActiveMood = ActiveKit->Mood;
	}
}

void AAshlineGrayboxBuilder::SpawnFullAtmosphere()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ConfigureDynamicWorldLighting();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(FVector(0.f, 0.f, 1800.f), ActiveMood.SunRotation, Params))
	{
		if (UDirectionalLightComponent* Light = Sun->GetComponent())
		{
			ConfigureSun(Light);
			Light->SetIntensity(ActiveMood.SunIntensity);
			Light->SetLightColor(ActiveMood.SunColor);
			Light->SetCastShadows(true);
			Light->bUseTemperature = true;
			Light->SetTemperature(ActiveMood.ColorTempKelvin);
		}
		BuiltActors.Add(Sun);
	}

	if (ActiveMood.FillLightIntensity > 0.05f)
	{
		if (ADirectionalLight* Fill = World->SpawnActor<ADirectionalLight>(FVector(0.f, 0.f, 1600.f), ActiveMood.FillLightRotation, Params))
		{
			if (UDirectionalLightComponent* Light = Fill->GetComponent())
			{
				ConfigureSecondaryDirectional(Light);
				Light->SetIntensity(ActiveMood.FillLightIntensity);
				Light->SetLightColor(ActiveMood.FillLightColor);
				Light->SetCastShadows(false);
			}
			BuiltActors.Add(Fill);
		}
	}

	if (ActiveMood.MoonIntensity > 0.05f)
	{
		const FRotator MoonRot(-18.f, ActiveMood.SunRotation.Yaw + 160.f, 0.f);
		if (ADirectionalLight* Moon = World->SpawnActor<ADirectionalLight>(FVector(0.f, 0.f, 2000.f), MoonRot, Params))
		{
			if (UDirectionalLightComponent* Light = Moon->GetComponent())
			{
				ConfigureSecondaryDirectional(Light);
				Light->SetIntensity(ActiveMood.MoonIntensity);
				Light->SetLightColor(ActiveMood.MoonColor);
				Light->SetCastShadows(true);
			}
			BuiltActors.Add(Moon);
		}
	}

	if (ASkyLight* Sky = World->SpawnActor<ASkyLight>(FVector::ZeroVector, FRotator::ZeroRotator, Params))
	{
		if (USkyLightComponent* SkyComp = Sky->GetLightComponent())
		{
			MakeRuntimeMovable(SkyComp);
			SkyComp->SetRealTimeCapture(true);
			SkyComp->SetIntensity(ActiveMood.SkyLightIntensity);
			SkyComp->bLowerHemisphereIsBlack = false;
		}
		BuiltActors.Add(Sky);
	}

	if (ASkyAtmosphere* Atmo = World->SpawnActor<ASkyAtmosphere>(FVector::ZeroVector, FRotator::ZeroRotator, Params))
	{
		BuiltActors.Add(Atmo);
	}

	if (AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(FVector(0.f, 0.f, 200.f), FRotator::ZeroRotator, Params))
	{
		if (UExponentialHeightFogComponent* FogComp = Fog->GetComponent())
		{
			FogComp->SetFogDensity(ActiveMood.FogDensity);
			FogComp->SetFogInscatteringColor(ActiveMood.FogColor);
			FogComp->SetFogHeightFalloff(ActiveMood.FogHeightFalloff);
			FogComp->SetStartDistance(ActiveMood.FogStartDistance);
			FogComp->SetVolumetricFog(true);
			FogComp->VolumetricFogScatteringDistribution = ActiveMood.VolumetricScattering;
			FogComp->VolumetricFogExtinctionScale = ActiveMood.VolumetricFogExtinction;
		}
		BuiltActors.Add(Fog);
	}

	SpawnPostProcess();
}

void AAshlineGrayboxBuilder::SpawnPostProcess()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APostProcessVolume* Volume = World->SpawnActor<APostProcessVolume>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
	if (!Volume)
	{
		return;
	}

	Volume->bUnbound = true;
	Volume->Priority = 10.f;
	Volume->BlendWeight = 1.f;

	FPostProcessSettings& S = Volume->Settings;
	S.bOverride_BloomIntensity = true;
	S.BloomIntensity = ActiveMood.BloomIntensity;
	S.bOverride_BloomThreshold = true;
	S.BloomThreshold = ActiveMood.BloomThreshold;
	S.bOverride_BloomDirtMaskIntensity = true;
	S.BloomDirtMaskIntensity = ActiveMood.DirtMaskIntensity;
	S.bOverride_VignetteIntensity = true;
	S.VignetteIntensity = ActiveMood.Vignette;
	S.bOverride_AmbientOcclusionIntensity = true;
	S.AmbientOcclusionIntensity = ActiveMood.AmbientOcclusion;
	S.bOverride_AmbientOcclusionRadius = true;
	S.AmbientOcclusionRadius = 180.f;
	S.bOverride_AutoExposureMethod = true;
	S.AutoExposureMethod = AEM_Histogram;
	S.bOverride_AutoExposureBias = true;
	S.AutoExposureBias = ActiveMood.AutoExposureBias != 0.f
		? ActiveMood.AutoExposureBias
		: (ActiveMood.bNight ? -0.4f : 0.15f);
	S.bOverride_ColorSaturation = true;
	S.ColorSaturation = ActiveMood.ColorSaturation;
	S.bOverride_ColorContrast = true;
	S.ColorContrast = ActiveMood.ColorContrast;
	S.bOverride_FilmGrainIntensity = true;
	S.FilmGrainIntensity = ActiveMood.FilmGrain;
	S.bOverride_MotionBlurAmount = true;
	S.MotionBlurAmount = 0.f;
	S.bOverride_SceneFringeIntensity = true;
	S.SceneFringeIntensity = ActiveMood.ChromaticAberration;
	S.bOverride_IndirectLightingIntensity = true;
	S.IndirectLightingIntensity = ActiveMood.bNight ? 0.7f : 1.f;
	S.bOverride_WhiteTemp = true;
	S.WhiteTemp = ActiveMood.ColorTempKelvin;
	S.bOverride_LocalExposureHighlightContrastScale = true;
	S.LocalExposureHighlightContrastScale = ActiveMood.LocalExposureHighlight;
	S.bOverride_LocalExposureShadowContrastScale = true;
	S.LocalExposureShadowContrastScale = ActiveMood.LocalExposureShadow;

	BuiltActors.Add(Volume);
}

void AAshlineGrayboxBuilder::Practical(const FVector& Location, const FLinearColor& Color, float Intensity, float Radius, bool bCastShadows)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (APointLight* Light = World->SpawnActor<APointLight>(Location, FRotator::ZeroRotator, Params))
	{
		if (UPointLightComponent* Comp = Light->PointLightComponent)
		{
			MakeRuntimeMovable(Comp);
			Comp->SetIntensity(Intensity);
			Comp->SetLightColor(Color);
			Comp->SetAttenuationRadius(Radius);
			Comp->SetCastShadows(bCastShadows);
		}
		BuiltActors.Add(Light);
	}
}

void AAshlineGrayboxBuilder::ConfigureDynamicWorldLighting()
{
	if (UWorld* World = GetWorld())
	{
		if (AWorldSettings* Settings = World->GetWorldSettings())
		{
			Settings->bForceNoPrecomputedLighting = true;
		}
	}
}

void AAshlineGrayboxBuilder::MakeRuntimeMovable(USceneComponent* Component) const
{
	if (Component)
	{
		Component->SetMobility(EComponentMobility::Movable);
	}
}

void AAshlineGrayboxBuilder::ConfigureSun(UDirectionalLightComponent* Light) const
{
	if (!Light)
	{
		return;
	}
	MakeRuntimeMovable(Light);
	Light->SetAtmosphereSunLight(true);
	Light->SetAtmosphereSunLightIndex(0);
	Light->ForwardShadingPriority = 2;
}

void AAshlineGrayboxBuilder::ConfigureSecondaryDirectional(UDirectionalLightComponent* Light) const
{
	if (!Light)
	{
		return;
	}
	MakeRuntimeMovable(Light);
	Light->SetAtmosphereSunLight(false);
	Light->ForwardShadingPriority = 0;
}

void AAshlineGrayboxBuilder::Spot(const FVector& Location, const FRotator& Rotation, const FLinearColor& Color, float Intensity, float Radius, float InnerCone, float OuterCone, bool bCastShadows)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ASpotLight* Light = World->SpawnActor<ASpotLight>(Location, Rotation, Params))
	{
		if (USpotLightComponent* Comp = Cast<USpotLightComponent>(Light->GetLightComponent()))
		{
			MakeRuntimeMovable(Comp);
			Comp->SetIntensity(Intensity);
			Comp->SetLightColor(Color);
			Comp->SetAttenuationRadius(Radius);
			Comp->SetInnerConeAngle(InnerCone);
			Comp->SetOuterConeAngle(OuterCone);
			Comp->SetCastShadows(bCastShadows);
		}
		BuiltActors.Add(Light);
	}
}

void AAshlineGrayboxBuilder::InteriorVolume(const FVector& Location, const FVector& Extent, float ExtraVignette, float ExposureBias)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APostProcessVolume* Volume = World->SpawnActor<APostProcessVolume>(Location, FRotator::ZeroRotator, Params);
	if (!Volume)
	{
		return;
	}
	Volume->bUnbound = false;
	Volume->BlendRadius = 420.f;
	Volume->Priority = 20.f;
	Volume->BlendWeight = 1.f;
	Volume->SetActorScale3D(FVector(
		FMath::Max(Extent.X, 80.f) / 200.f,
		FMath::Max(Extent.Y, 80.f) / 200.f,
		FMath::Max(Extent.Z, 80.f) / 200.f));

	FPostProcessSettings& S = Volume->Settings;
	S.bOverride_VignetteIntensity = true;
	S.VignetteIntensity = FMath::Clamp(ActiveMood.Vignette + ExtraVignette, 0.f, 1.f);
	S.bOverride_AutoExposureBias = true;
	S.AutoExposureBias = ExposureBias;
	S.bOverride_AmbientOcclusionIntensity = true;
	S.AmbientOcclusionIntensity = FMath::Min(ActiveMood.AmbientOcclusion + 0.25f, 1.f);
	S.bOverride_IndirectLightingIntensity = true;
	S.IndirectLightingIntensity = ActiveMood.bNight ? 0.45f : 0.8f;
	BuiltActors.Add(Volume);
}

void AAshlineGrayboxBuilder::DecalMark(const FVector& Location, const FRotator& Rotation, const FVector& Size)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	UMaterialInterface* DecalMat = nullptr;
	if (ActiveKit)
	{
		DecalMat = AshlineLoad::Soft(ActiveKit->DecalMaterial);
	}
	if (!DecalMat)
	{
		DecalMat = UAshlinePresentationLibrary::ResolveImpactDecalMaterial();
	}
	if (!DecalMat)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ADecalActor* Decal = World->SpawnActor<ADecalActor>(Location, Rotation, Params))
	{
		Decal->SetDecalMaterial(DecalMat);
		Decal->GetDecal()->DecalSize = Size;
		BuiltActors.Add(Decal);
	}
}

void AAshlineGrayboxBuilder::Tree(const FVector& Location, float Height)
{
	if (ActiveKit)
	{
		for (const FAshlineSoftMeshSlot& Slot : ActiveKit->PropMeshes)
		{
			if (Slot.SlotId == TEXT("Tree"))
			{
				if (UStaticMesh* KitTree = AshlineLoad::Soft(Slot.Mesh))
				{
					if (AActor* Actor = Sphere(Location + FVector(0.f, 0.f, Height * 0.4f), FVector::OneVector, ActiveMood.GroundTint, EAshlineSurface::Foliage))
					{
						if (UStaticMeshComponent* Mesh = Actor->FindComponentByClass<UStaticMeshComponent>())
						{
							Mesh->SetStaticMesh(KitTree);
							Mesh->SetWorldScale3D(Slot.Scale.IsNearlyZero() ? FVector(Height / 400.f) : Slot.Scale * (Height / 400.f));
						}
					}
					return;
				}
			}
		}
	}

	if (UStaticMesh* Authored = UAshlinePresentationLibrary::ResolveTreeMesh())
	{
		if (Authored != CylinderMesh && Authored->GetPathName().Contains(TEXT("Ashline")))
		{
			if (AActor* Actor = Cylinder(Location + FVector(0.f, 0.f, Height * 0.4f), FVector(1.f, 1.f, Height / 200.f), FLinearColor(0.16f, 0.1f, 0.06f), EAshlineSurface::Wood))
			{
				if (UStaticMeshComponent* Mesh = Actor->FindComponentByClass<UStaticMeshComponent>())
				{
					Mesh->SetStaticMesh(Authored);
				}
			}
			return;
		}
	}

	const float Trunk = FMath::Clamp(Height / 420.f, 0.6f, 1.6f);
	Cylinder(Location + FVector(0.f, 0.f, Height * 0.28f), FVector(0.22f * Trunk, 0.22f * Trunk, Height / 100.f * 0.55f),
		FLinearColor(0.16f, 0.1f, 0.06f), EAshlineSurface::Wood);
	Sphere(Location + FVector(0.f, 0.f, Height * 0.72f), FVector(1.6f * Trunk, 1.6f * Trunk, 1.4f * Trunk),
		FLinearColor(0.08f, 0.18f, 0.07f), EAshlineSurface::Foliage);
}

void AAshlineGrayboxBuilder::Bush(const FVector& Location)
{
	if (ActiveKit)
	{
		for (const FAshlineSoftMeshSlot& Slot : ActiveKit->PropMeshes)
		{
			if (Slot.SlotId == TEXT("Bush"))
			{
				if (UStaticMesh* KitBush = AshlineLoad::Soft(Slot.Mesh))
				{
					if (AActor* Actor = Sphere(Location + FVector(0.f, 0.f, 30.f), FVector(0.7f, 0.85f, 0.45f), FLinearColor(0.1f, 0.2f, 0.08f), EAshlineSurface::Foliage))
					{
						if (UStaticMeshComponent* Mesh = Actor->FindComponentByClass<UStaticMeshComponent>())
						{
							Mesh->SetStaticMesh(KitBush);
						}
					}
					return;
				}
			}
		}
	}

	if (UStaticMesh* Authored = UAshlinePresentationLibrary::ResolveFoliageMesh())
	{
		if (Authored != SphereMesh && Authored->GetPathName().Contains(TEXT("StarterContent")))
		{
			if (AActor* Actor = Sphere(Location + FVector(0.f, 0.f, 30.f), FVector(0.7f, 0.85f, 0.45f), FLinearColor(0.1f, 0.2f, 0.08f), EAshlineSurface::Foliage))
			{
				if (UStaticMeshComponent* Mesh = Actor->FindComponentByClass<UStaticMeshComponent>())
				{
					Mesh->SetStaticMesh(Authored);
				}
			}
			return;
		}
	}

	Sphere(Location + FVector(0.f, 0.f, 30.f), FVector(0.7f, 0.85f, 0.45f), FLinearColor(0.1f, 0.2f, 0.08f), EAshlineSurface::Foliage);
}

void AAshlineGrayboxBuilder::GrassPatch(const FVector& Center, float Radius, int32 Count)
{
	for (int32 i = 0; i < Count; ++i)
	{
		const float Angle = (2.f * PI * i) / FMath::Max(1, Count);
		const FVector Loc = Center + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 8.f);
		Box(Loc, FVector(0.08f, 0.08f, 0.22f), FLinearColor(0.12f, 0.22f, 0.08f), false, EAshlineSurface::Foliage);
	}
}

void AAshlineGrayboxBuilder::ApplySurfaceMaterial(UStaticMeshComponent* Mesh, AActor* MaterialOuter, const FLinearColor& Color, EAshlineSurface Surface)
{
	if (!Mesh)
	{
		return;
	}
	if (Surface == EAshlineSurface::Auto)
	{
		Surface = UAshlinePresentationLibrary::InferSurface(Color);
	}

	UMaterialInterface* KitMat = nullptr;
	if (ActiveKit)
	{
		if (Surface == EAshlineSurface::Ground || Surface == EAshlineSurface::Sand || Surface == EAshlineSurface::Snow
			|| Surface == EAshlineSurface::Dirt || Surface == EAshlineSurface::Asphalt)
		{
			KitMat = AshlineLoad::Soft(ActiveKit->GroundMaterial);
		}
		else if (Surface == EAshlineSurface::Foliage)
		{
			KitMat = AshlineLoad::Soft(ActiveKit->FoliageMaterial);
		}
		else if (Surface == EAshlineSurface::Metal)
		{
			KitMat = AshlineLoad::Soft(ActiveKit->TrimMaterial);
		}
		else if (Surface == EAshlineSurface::Glass || Surface == EAshlineSurface::Water)
		{
			KitMat = AshlineLoad::Soft(ActiveKit->GlassMaterial);
		}
		else
		{
			KitMat = AshlineLoad::Soft(ActiveKit->WallMaterial);
		}
	}

	if (!KitMat && ActiveMissionId != EAshlineMissionId::None)
	{
		if (Surface == EAshlineSurface::Ground || Surface == EAshlineSurface::Sand || Surface == EAshlineSurface::Snow
			|| Surface == EAshlineSurface::Dirt || Surface == EAshlineSurface::Asphalt)
		{
			KitMat = UAshlinePresentationLibrary::LoadMaterial({ UAshlineContentManifest::KitGroundPath(ActiveMissionId) });
		}
		else if (Surface == EAshlineSurface::Foliage)
		{
			KitMat = UAshlinePresentationLibrary::LoadMaterial({ UAshlineContentManifest::KitFoliagePath(ActiveMissionId) });
		}
		else if (Surface == EAshlineSurface::Metal)
		{
			KitMat = UAshlinePresentationLibrary::LoadMaterial({ UAshlineContentManifest::KitTrimPath(ActiveMissionId) });
		}
		else
		{
			KitMat = UAshlinePresentationLibrary::LoadMaterial({ UAshlineContentManifest::KitWallPath(ActiveMissionId) });
		}
	}

	if (KitMat)
	{
		if (UMaterialInstanceDynamic* KitMID = UMaterialInstanceDynamic::Create(KitMat, MaterialOuter))
		{
			FAshlineTextureSet Stamp = UAshlineMaterialFactory::DefaultsForSurface(Surface, Color);
			Stamp.Tint = Color;
			UAshlineMaterialFactory::StampTextureSet(KitMID, Stamp);
			Mesh->SetMaterial(0, KitMID);
			return;
		}
		Mesh->SetMaterial(0, KitMat);
		return;
	}

	FAshlineTextureSet Empty;
	if (UAshlineMaterialFactory::ApplyToMesh(Mesh, MaterialOuter, Surface, Color, Empty))
	{
		return;
	}

	if (UMaterialInstanceDynamic* PrimaryMID = UAshlinePresentationLibrary::MakeTintedMaterial(MaterialOuter, Surface, Color))
	{
		Mesh->SetMaterial(0, PrimaryMID);
	}
	else if (ShapeMaterial)
	{
		if (UMaterialInstanceDynamic* FallbackMID = UMaterialInstanceDynamic::Create(ShapeMaterial, MaterialOuter))
		{
			FallbackMID->SetVectorParameterValue(TEXT("Color"), Color);
			Mesh->SetMaterial(0, FallbackMID);
		}
	}
}

void AAshlineGrayboxBuilder::Sandbag(const FVector& Location, const FRotator& Rotation)
{
	AActor* Actor = Box(Location + FVector(0.f, 0.f, 28.f), FVector(1.1f, 0.45f, 0.32f), FLinearColor(0.38f, 0.3f, 0.16f), true, EAshlineSurface::Sand);
	if (Actor)
	{
		Actor->SetActorRotation(Rotation);
	}
}

void AAshlineGrayboxBuilder::VehicleHull(const FVector& Location, const FRotator& Rotation, const FLinearColor& Color, float Length)
{
	Box(Location + FVector(0.f, 0.f, 70.f), FVector(Length, 2.1f, 1.3f), Color, true, EAshlineSurface::Metal);
	Box(Location + FVector(Length * 28.f, 0.f, 95.f), FVector(1.6f, 2.0f, 0.9f), Color * 0.85f, true, EAshlineSurface::Metal);
	Cylinder(Location + FVector(-Length * 28.f, 90.f, 28.f), FVector(0.55f, 0.55f, 0.18f), FLinearColor(0.05f, 0.05f, 0.05f), EAshlineSurface::Metal);
	Cylinder(Location + FVector(Length * 22.f, 90.f, 28.f), FVector(0.55f, 0.55f, 0.18f), FLinearColor(0.05f, 0.05f, 0.05f), EAshlineSurface::Metal);
	Cylinder(Location + FVector(-Length * 28.f, -90.f, 28.f), FVector(0.55f, 0.55f, 0.18f), FLinearColor(0.05f, 0.05f, 0.05f), EAshlineSurface::Metal);
	Cylinder(Location + FVector(Length * 22.f, -90.f, 28.f), FVector(0.55f, 0.55f, 0.18f), FLinearColor(0.05f, 0.05f, 0.05f), EAshlineSurface::Metal);
	(void)Rotation;
}

void AAshlineGrayboxBuilder::WindowStrip(const FVector& Location, int32 Count, float Spacing, const FLinearColor& Glow)
{
	for (int32 i = 0; i < Count; ++i)
	{
		const FVector Loc = Location + FVector(0.f, (i - Count * 0.5f) * Spacing, 0.f);
		Box(Loc, FVector(0.08f, 0.55f, 0.7f), Glow, false, EAshlineSurface::Emissive);
		Spot(Loc + FVector(-40.f, 0.f, 20.f), FRotator(-12.f, 180.f, 0.f), Glow, 1400.f, 520.f, 18.f, 38.f, false);
	}
}

void AAshlineGrayboxBuilder::Doorway(const FVector& Location, const FRotator& Rotation)
{
	Box(Location + FVector(0.f, -70.f, 110.f), FVector(0.2f, 0.25f, 2.2f), FLinearColor(0.12f, 0.1f, 0.08f), true, EAshlineSurface::Wood);
	Box(Location + FVector(0.f, 70.f, 110.f), FVector(0.2f, 0.25f, 2.2f), FLinearColor(0.12f, 0.1f, 0.08f), true, EAshlineSurface::Wood);
	Box(Location + FVector(0.f, 0.f, 220.f), FVector(0.2f, 1.6f, 0.2f), FLinearColor(0.12f, 0.1f, 0.08f), true, EAshlineSurface::Wood);
	(void)Rotation;
}

void AAshlineGrayboxBuilder::CoverLine(const FVector& From, const FVector& To, int32 Count)
{
	const int32 N = FMath::Max(1, Count);
	for (int32 i = 0; i < N; ++i)
	{
		const float Alpha = (N == 1) ? 0.5f : static_cast<float>(i) / static_cast<float>(N - 1);
		Cover(FMath::Lerp(From, To, Alpha));
	}
}

void AAshlineGrayboxBuilder::TreeBelt(const FVector& From, const FVector& To, int32 Count, float Height)
{
	const int32 N = FMath::Max(1, Count);
	for (int32 i = 0; i < N; ++i)
	{
		const float Alpha = (N == 1) ? 0.5f : static_cast<float>(i) / static_cast<float>(N - 1);
		const FVector Loc = FMath::Lerp(From, To, Alpha);
		const float Jitter = static_cast<float>((i % 3) - 1) * 80.f;
		Tree(Loc + FVector(0.f, Jitter, 0.f), Height + static_cast<float>(i % 4) * 28.f);
	}
}

void AAshlineGrayboxBuilder::ScatterFoliage(const FVector& Center, float Radius, int32 Trees, int32 Bushes)
{
	for (int32 i = 0; i < Trees; ++i)
	{
		const float Angle = (2.f * PI * i) / FMath::Max(1, Trees);
		Tree(Center + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius * 0.85f, 0.f), 280.f + (i % 5) * 40.f);
	}
	for (int32 i = 0; i < Bushes; ++i)
	{
		const float Angle = (2.f * PI * i) / FMath::Max(1, Bushes) + 0.35f;
		Bush(Center + FVector(FMath::Cos(Angle) * Radius * 0.55f, FMath::Sin(Angle) * Radius * 0.55f, 0.f));
	}
}

void AAshlineGrayboxBuilder::Watchtower(const FVector& Location, float Height)
{
	const float H = FMath::Max(500.f, Height);
	Cylinder(Location + FVector(0.f, 0.f, H * 0.45f), FVector(0.45f, 0.45f, H / 100.f * 0.9f), FLinearColor(0.18f, 0.16f, 0.12f), EAshlineSurface::Wood);
	Box(Location + FVector(0.f, 0.f, H), FVector(2.4f, 2.4f, 0.18f), FLinearColor(0.22f, 0.2f, 0.16f), true, EAshlineSurface::Wood);
	Box(Location + FVector(0.f, 0.f, H + 90.f), FVector(2.2f, 2.2f, 1.1f), FLinearColor(0.16f, 0.14f, 0.12f), true, EAshlineSurface::Wood);
	Spot(Location + FVector(0.f, 0.f, H + 40.f), FRotator(-55.f, 0.f, 0.f), FLinearColor(1.f, 0.75f, 0.4f), 3500.f, 1600.f, 25.f, 48.f, false);
}

void AAshlineGrayboxBuilder::Building(const FVector& Location, const FVector2D& Footprint, float Height, const FLinearColor& Color)
{
	Box(Location + FVector(0.f, 0.f, Height * 0.5f),
		FVector(Footprint.X / 100.f, Footprint.Y / 100.f, Height / 100.f),
		Color, true, EAshlineSurface::Concrete);
	TrimBand(Location, Footprint, Height);
	if (Footprint.X >= 500.f || Footprint.Y >= 500.f)
	{
		const int32 Windows = FMath::Clamp(static_cast<int32>(Footprint.Y / 180.f), 2, 6);
		WindowStrip(Location + FVector(Footprint.X * 0.5f + 8.f, 0.f, Height * 0.55f), Windows, 110.f,
			ActiveMood.bNight ? FLinearColor(1.f, 0.62f, 0.28f) : FLinearColor(0.55f, 0.72f, 0.9f));
		GlassPane(Location + FVector(Footprint.X * 0.5f + 6.f, 0.f, Height * 0.45f),
			FVector(0.06f, FMath::Min(Footprint.Y, 420.f) / 100.f * 0.55f, Height / 100.f * 0.28f));
	}
}

void AAshlineGrayboxBuilder::WalledYard(const FVector& Center, const FVector2D& HalfExtent, float WallHeight, const FLinearColor& Color, int32 OpenSide, float GateWidth)
{
	const float Hx = HalfExtent.X;
	const float Hy = HalfExtent.Y;
	const float Z = Center.Z + WallHeight * 0.5f;
	const float Thick = 55.f;
	const float Gate = FMath::Clamp(GateWidth, 250.f, FMath::Min(Hx, Hy) * 1.6f);

	auto Edge = [this, WallHeight, &Color](const FVector& Loc, float SizeX, float SizeY)
	{
		Wall(Loc, FVector(SizeX / 100.f, SizeY / 100.f, WallHeight / 100.f), Color);
	};

	auto GatedAxis = [&](bool bWestEast, float AxisPos, bool bOpen)
	{
		if (!bOpen)
		{
			if (bWestEast)
			{
				Edge(FVector(Center.X + AxisPos, Center.Y, Z), Thick, Hy * 2.f);
			}
			else
			{
				Edge(FVector(Center.X, Center.Y + AxisPos, Z), Hx * 2.f, Thick);
			}
			return;
		}
		if (bWestEast)
		{
			const float Seg = Hy - Gate * 0.5f;
			const float Off = 0.5f * (Hy + Gate * 0.5f);
			Edge(FVector(Center.X + AxisPos, Center.Y - Off, Z), Thick, Seg);
			Edge(FVector(Center.X + AxisPos, Center.Y + Off, Z), Thick, Seg);
		}
		else
		{
			const float Seg = Hx - Gate * 0.5f;
			const float Off = 0.5f * (Hx + Gate * 0.5f);
			Edge(FVector(Center.X - Off, Center.Y + AxisPos, Z), Seg, Thick);
			Edge(FVector(Center.X + Off, Center.Y + AxisPos, Z), Seg, Thick);
		}
	};

	GatedAxis(true, -Hx, OpenSide == 0);
	GatedAxis(true, Hx, OpenSide == 1);
	GatedAxis(false, -Hy, OpenSide == 2);
	GatedAxis(false, Hy, OpenSide == 3);
}

void AAshlineGrayboxBuilder::RoadStrip(const FVector& From, const FVector& To, float Width)
{
	const FVector Delta = To - From;
	const float Len = Delta.Size2D();
	if (Len < 50.f)
	{
		return;
	}
	const FVector Mid = (From + To) * 0.5f + FVector(0.f, 0.f, 6.f);
	if (AActor* Strip = Box(Mid, FVector(Len / 100.f, Width / 100.f, 0.08f), FLinearColor(0.12f, 0.12f, 0.11f), true, EAshlineSurface::Asphalt))
	{
		Strip->SetActorRotation(FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal().Rotation());
	}
}

void AAshlineGrayboxBuilder::ScatterKitProps(const FVector& Center, float Radius, int32 Count)
{
	if (Count <= 0)
	{
		return;
	}

	int32 Placed = 0;
	if (ActiveKit && ActiveKit->PropMeshes.Num() > 0)
	{
		for (int32 i = 0; i < Count && Placed < Count; ++i)
		{
			const FAshlineSoftMeshSlot& Slot = ActiveKit->PropMeshes[i % ActiveKit->PropMeshes.Num()];
			UStaticMesh* MeshAsset = AshlineLoad::Soft(Slot.Mesh);
			if (!MeshAsset)
			{
				continue;
			}
			const float Angle = (2.f * PI * i) / FMath::Max(1, Count);
			const FVector Loc = Center + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius * 0.9f, 40.f);
			if (AActor* Actor = Box(Loc, FVector(1.f), FLinearColor(0.2f, 0.18f, 0.14f), true, EAshlineSurface::Concrete))
			{
				if (UStaticMeshComponent* Mesh = Actor->FindComponentByClass<UStaticMeshComponent>())
				{
					Mesh->SetStaticMesh(MeshAsset);
					Mesh->SetWorldScale3D(Slot.Scale.IsNearlyZero() ? FVector(1.f) : Slot.Scale);
				}
				++Placed;
			}
		}
	}

	if (Placed == 0)
	{
		ScatterDress(Center, Radius, Count);
	}
}

void AAshlineGrayboxBuilder::TrimBand(const FVector& Location, const FVector2D& Footprint, float Height)
{
	const float Z = Location.Z + Height + 8.f;
	const float Hx = Footprint.X * 0.5f;
	const float Hy = Footprint.Y * 0.5f;
	const FLinearColor Trim = ActiveMood.MetalTint;
	Box(Location + FVector(0.f, Hy, Height + 6.f), FVector(Footprint.X / 100.f + 0.12f, 0.12f, 0.14f), Trim, false, EAshlineSurface::Metal);
	Box(Location + FVector(0.f, -Hy, Height + 6.f), FVector(Footprint.X / 100.f + 0.12f, 0.12f, 0.14f), Trim, false, EAshlineSurface::Metal);
	Box(Location + FVector(Hx, 0.f, Height + 6.f), FVector(0.12f, Footprint.Y / 100.f + 0.12f, 0.14f), Trim, false, EAshlineSurface::Metal);
	Box(Location + FVector(-Hx, 0.f, Height + 6.f), FVector(0.12f, Footprint.Y / 100.f + 0.12f, 0.14f), Trim, false, EAshlineSurface::Metal);
	(void)Z;
}

void AAshlineGrayboxBuilder::GlassPane(const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
	if (AActor* Pane = Box(Location, Scale, FLinearColor(0.42f, 0.58f, 0.72f), false, EAshlineSurface::Glass))
	{
		Pane->SetActorRotation(Rotation);
	}
}

void AAshlineGrayboxBuilder::CrateStack(const FVector& Location, int32 Count)
{
	const int32 N = FMath::Clamp(Count, 2, 8);
	for (int32 i = 0; i < N; ++i)
	{
		const float Ox = static_cast<float>(i % 2) * 70.f;
		const float Oy = static_cast<float>((i / 2) % 2) * 64.f;
		const float Oz = 28.f + (i / 4) * 56.f;
		Box(Location + FVector(Ox, Oy, Oz), FVector(0.7f, 0.64f, 0.55f),
			FLinearColor(0.28f, 0.2f, 0.1f), true, EAshlineSurface::Wood);
	}
}

void AAshlineGrayboxBuilder::StreetLamp(const FVector& Location, float Height)
{
	const float H = FMath::Max(240.f, Height);
	Cylinder(Location + FVector(0.f, 0.f, H * 0.5f), FVector(0.12f, 0.12f, H / 100.f), FLinearColor(0.12f, 0.12f, 0.13f), EAshlineSurface::Metal);
	Sphere(Location + FVector(0.f, 0.f, H + 18.f), FVector(0.28f, 0.28f, 0.28f),
		ActiveMood.bNight ? FLinearColor(1.f, 0.78f, 0.42f) : FLinearColor(0.7f, 0.72f, 0.65f), EAshlineSurface::Emissive);
	Practical(Location + FVector(0.f, 0.f, H),
		ActiveMood.bNight ? FLinearColor(1.f, 0.72f, 0.38f) : FLinearColor(0.9f, 0.92f, 0.85f),
		ActiveMood.bNight ? 4200.f : 1800.f, 900.f, false);
}

void AAshlineGrayboxBuilder::AntennaMast(const FVector& Location, float Height)
{
	const float H = FMath::Max(600.f, Height);
	Cylinder(Location + FVector(0.f, 0.f, H * 0.5f), FVector(0.18f, 0.18f, H / 100.f), FLinearColor(0.18f, 0.18f, 0.2f), EAshlineSurface::Metal);
	Box(Location + FVector(0.f, 0.f, H + 20.f), FVector(1.4f, 0.12f, 0.12f), FLinearColor(0.55f, 0.55f, 0.5f), false, EAshlineSurface::Metal);
	Sphere(Location + FVector(0.f, 0.f, H + 40.f), FVector(0.22f, 0.22f, 0.22f), FLinearColor(0.9f, 0.15f, 0.1f), EAshlineSurface::Emissive);
}

void AAshlineGrayboxBuilder::Barricade(const FVector& Location, const FRotator& Rotation)
{
	Sandbag(Location, Rotation);
	Sandbag(Location + FVector(90.f, 20.f, 0.f), Rotation);
	Cover(Location + FVector(40.f, -80.f, 0.f));
}

void AAshlineGrayboxBuilder::DebrisPile(const FVector& Location)
{
	Box(Location + FVector(0.f, 0.f, 18.f), FVector(1.1f, 0.7f, 0.28f), FLinearColor(0.22f, 0.2f, 0.16f), true, EAshlineSurface::Concrete);
	Box(Location + FVector(40.f, 30.f, 22.f), FVector(0.55f, 0.8f, 0.22f), FLinearColor(0.16f, 0.14f, 0.12f), true, EAshlineSurface::Metal);
	Box(Location + FVector(-30.f, -20.f, 16.f), FVector(0.7f, 0.4f, 0.2f), FLinearColor(0.24f, 0.16f, 0.1f), true, EAshlineSurface::Wood);
}

void AAshlineGrayboxBuilder::FenceRun(const FVector& From, const FVector& To, int32 Posts)
{
	const int32 N = FMath::Max(2, Posts);
	for (int32 i = 0; i < N; ++i)
	{
		const float Alpha = static_cast<float>(i) / static_cast<float>(N - 1);
		const FVector Loc = FMath::Lerp(From, To, Alpha);
		Cylinder(Loc + FVector(0.f, 0.f, 90.f), FVector(0.08f, 0.08f, 1.8f), FLinearColor(0.14f, 0.14f, 0.12f), EAshlineSurface::Metal);
	}
	const FVector Mid = (From + To) * 0.5f + FVector(0.f, 0.f, 110.f);
	const float Len = FVector::Dist2D(From, To);
	if (AActor* Rail = Box(Mid, FVector(Len / 100.f, 0.06f, 0.06f), FLinearColor(0.16f, 0.16f, 0.14f), false, EAshlineSurface::Metal))
	{
		Rail->SetActorRotation(FVector(To.X - From.X, To.Y - From.Y, 0.f).GetSafeNormal().Rotation());
	}
}

void AAshlineGrayboxBuilder::ScatterDress(const FVector& Center, float Radius, int32 Count)
{
	const int32 N = FMath::Max(3, Count);
	for (int32 i = 0; i < N; ++i)
	{
		const float Angle = (2.f * PI * i) / static_cast<float>(N);
		const FVector Loc = Center + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius * 0.88f, 0.f);
		switch (i % 5)
		{
		case 0: CrateStack(Loc, 3 + (i % 3)); break;
		case 1: DebrisPile(Loc); break;
		case 2: Barricade(Loc); break;
		case 3: StreetLamp(Loc, 360.f + (i % 2) * 80.f); break;
		default: Cover(Loc); break;
		}
	}
}

void AAshlineGrayboxBuilder::DestructibleStub(const FVector& Location)
{
	// Visual stand-in only. No Chaos / Geometry Collection dependency.
	if (AActor* Stub = Box(Location + FVector(0.f, 0.f, 40.f), FVector(0.9f, 0.7f, 0.8f), FLinearColor(0.32f, 0.22f, 0.12f), true, EAshlineSurface::Wood))
	{
		Stub->Tags.Add(TEXT("AshlineDestructibleStub"));
	}
}


