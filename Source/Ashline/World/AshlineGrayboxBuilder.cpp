#include "World/AshlineGrayboxBuilder.h"

#include "AI/AshlineAICatalog.h"
#include "AI/AshlineAICharacter.h"
#include "Campaign/AshlineObjectiveTrigger.h"
#include "Engine/CollisionProfile.h"
#include "Camera/CameraActor.h"
#include "Components/BoxComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/GameInstance.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
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
		CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	}
	if (!CylinderMesh)
	{
		CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	}
	if (!SphereMesh)
	{
		SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	}
	if (!ShapeMaterial)
	{
		ShapeMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
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
	SpawnAtmosphere(FLinearColor(0.75f, 0.7f, 0.55f), 6.f, FLinearColor(0.25f, 0.22f, 0.18f), 0.015f);

	Floor(FVector::ZeroVector, FVector2D(4000.f, 4000.f), FLinearColor(0.18f, 0.17f, 0.14f));
	Box(FVector(0.f, 0.f, 80.f), FVector(8.f, 1.2f, 1.6f), FLinearColor(0.35f, 0.28f, 0.16f));
	Box(FVector(-400.f, 500.f, 90.f), FVector(2.f, 2.f, 1.8f), FLinearColor(0.22f, 0.24f, 0.2f));
	Box(FVector(500.f, -350.f, 70.f), FVector(1.6f, 3.f, 1.4f), FLinearColor(0.28f, 0.2f, 0.14f));
	Cylinder(FVector(0.f, 0.f, 220.f), FVector(0.4f, 0.4f, 2.4f), FLinearColor(0.7f, 0.55f, 0.2f));

	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FrontendCamera = World->SpawnActor<ACameraActor>(FVector(-900.f, 0.f, 380.f), FRotator(-12.f, 0.f, 0.f), Params);
		if (FrontendCamera)
		{
			BuiltActors.Add(FrontendCamera);
		}
	}

	PlayerStartAt(FVector(-200.f, 0.f, 120.f), FRotator::ZeroRotator);
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

	ExtraAIAround(LastPlayerStartLocation + FVector(900.f, 0.f, 0.f), 700.f);
}

void AAshlineGrayboxBuilder::SpawnAtmosphere(const FLinearColor& SunColor, float SunIntensity, const FLinearColor& FogColor, float FogDensity)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(FVector(0.f, 0.f, 1800.f), FRotator(-46.f, -28.f, 0.f), Params))
	{
		if (UDirectionalLightComponent* Light = Sun->GetComponent())
		{
			Light->SetIntensity(SunIntensity);
			Light->SetLightColor(SunColor);
			Light->SetAtmosphereSunLight(true);
		}
		BuiltActors.Add(Sun);
	}

	if (ASkyLight* Sky = World->SpawnActor<ASkyLight>(FVector::ZeroVector, FRotator::ZeroRotator, Params))
	{
		if (USkyLightComponent* SkyComp = Sky->GetLightComponent())
		{
			SkyComp->SetRealTimeCapture(true);
			SkyComp->SetIntensity(1.1f);
		}
		BuiltActors.Add(Sky);
	}

	if (AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(FVector(0.f, 0.f, 200.f), FRotator::ZeroRotator, Params))
	{
		if (UExponentialHeightFogComponent* FogComp = Fog->GetComponent())
		{
			FogComp->SetFogDensity(FogDensity);
			FogComp->SetFogInscatteringColor(FogColor);
			FogComp->SetFogHeightFalloff(0.15f);
		}
		BuiltActors.Add(Fog);
	}
}

AActor* AAshlineGrayboxBuilder::Box(const FVector& Location, const FVector& Scale, const FLinearColor& Color, bool bCollision)
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
		if (ShapeMaterial)
		{
			if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(ShapeMaterial, Actor))
			{
				MID->SetVectorParameterValue(TEXT("Color"), Color);
				Mesh->SetMaterial(0, MID);
			}
		}
	}
	BuiltActors.Add(Actor);
	return Actor;
}

AActor* AAshlineGrayboxBuilder::Cylinder(const FVector& Location, const FVector& Scale, const FLinearColor& Color)
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
		if (ShapeMaterial)
		{
			if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(ShapeMaterial, Actor))
			{
				MID->SetVectorParameterValue(TEXT("Color"), Color);
				Mesh->SetMaterial(0, MID);
			}
		}
	}
	BuiltActors.Add(Actor);
	return Actor;
}

AActor* AAshlineGrayboxBuilder::Sphere(const FVector& Location, const FVector& Scale, const FLinearColor& Color)
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
		if (ShapeMaterial)
		{
			if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(ShapeMaterial, Actor))
			{
				MID->SetVectorParameterValue(TEXT("Color"), Color);
				Mesh->SetMaterial(0, MID);
			}
		}
	}
	BuiltActors.Add(Actor);
	return Actor;
}

void AAshlineGrayboxBuilder::Floor(const FVector& Center, const FVector2D& Size, const FLinearColor& Color)
{
	// Engine cube is 100uu. Thickness 20uu, top sits at Center.Z.
	const FVector Scale(Size.X / 100.f, Size.Y / 100.f, 0.2f);
	Box(Center + FVector(0.f, 0.f, -10.f), Scale, Color);
}

void AAshlineGrayboxBuilder::Wall(const FVector& Location, const FVector& Scale, const FLinearColor& Color)
{
	Box(Location, Scale, Color);
}

void AAshlineGrayboxBuilder::Cover(const FVector& Location)
{
	Box(Location + FVector(0.f, 0.f, 50.f), FVector(1.4f, 2.2f, 1.1f), FLinearColor(0.28f, 0.26f, 0.22f));
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

	Box(Location + FVector(0.f, 0.f, 40.f), FVector(0.7f, 0.7f, 0.7f), Color, false);
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

void AAshlineGrayboxBuilder::BuildWireCut()
{
	SpawnAtmosphere(FLinearColor(0.35f, 0.45f, 0.75f), 3.2f, FLinearColor(0.05f, 0.07f, 0.12f), 0.03f);
	Floor(FVector::ZeroVector, FVector2D(7000.f, 5000.f), FLinearColor(0.08f, 0.1f, 0.07f));

	// Approach trench / west berm.
	Wall(FVector(-2200.f, -400.f, 80.f), FVector(8.f, 0.6f, 1.6f), FLinearColor(0.12f, 0.16f, 0.1f));
	Wall(FVector(-2200.f, 400.f, 80.f), FVector(8.f, 0.6f, 1.6f), FLinearColor(0.12f, 0.16f, 0.1f));
	Cover(FVector(-1600.f, -180.f, 0.f));
	Cover(FVector(-1400.f, 220.f, 0.f));

	// Compound walls.
	Wall(FVector(-200.f, -1100.f, 160.f), FVector(24.f, 0.5f, 3.2f), FLinearColor(0.2f, 0.18f, 0.14f));
	Wall(FVector(-200.f, 1100.f, 160.f), FVector(24.f, 0.5f, 3.2f), FLinearColor(0.2f, 0.18f, 0.14f));
	Wall(FVector(-1400.f, 0.f, 160.f), FVector(0.5f, 22.f, 3.2f), FLinearColor(0.2f, 0.18f, 0.14f));
	Wall(FVector(1000.f, 0.f, 160.f), FVector(0.5f, 22.f, 3.2f), FLinearColor(0.2f, 0.18f, 0.14f));
	// Gate opening on west wall (gap around Y=0).
	Wall(FVector(-1400.f, -700.f, 160.f), FVector(0.5f, 8.f, 3.2f), FLinearColor(0.22f, 0.2f, 0.16f));
	Wall(FVector(-1400.f, 700.f, 160.f), FVector(0.5f, 8.f, 3.2f), FLinearColor(0.22f, 0.2f, 0.16f));

	// Comms hut.
	Box(FVector(350.f, 0.f, 140.f), FVector(6.f, 5.f, 2.8f), FLinearColor(0.16f, 0.18f, 0.22f));
	Cylinder(FVector(350.f, 0.f, 360.f), FVector(0.35f, 0.35f, 2.2f), FLinearColor(0.55f, 0.55f, 0.5f));
	Cover(FVector(-200.f, -250.f, 0.f));
	Cover(FVector(80.f, 320.f, 0.f));

	// Creek exfil east.
	Box(FVector(1800.f, 0.f, 20.f), FVector(10.f, 18.f, 0.3f), FLinearColor(0.1f, 0.14f, 0.18f));

	PlayerStartAt(FVector(-2800.f, 0.f, 120.f), FRotator(0.f, 0.f, 0.f));
	Objective(TEXT("INFIL"), FVector(-1450.f, 0.f, 80.f), false, FLinearColor(0.4f, 0.8f, 0.4f));
	Objective(TEXT("CUT"), FVector(350.f, 280.f, 80.f), false, FLinearColor(0.9f, 0.4f, 0.2f));
	Objective(TEXT("EXFIL"), FVector(2100.f, 0.f, 80.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("GHOST"), FVector(200.f, -800.f, 80.f), false, FLinearColor(0.7f, 0.7f, 0.9f));

	SpawnAI(FVector(-400.f, -200.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(100.f, 280.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(500.f, -180.f, 100.f), EAshlineAIArchetype::Officer);
}

void AAshlineGrayboxBuilder::BuildDustMarket()
{
	SpawnAtmosphere(FLinearColor(0.95f, 0.72f, 0.4f), 7.5f, FLinearColor(0.35f, 0.25f, 0.12f), 0.022f);
	Floor(FVector::ZeroVector, FVector2D(6000.f, 6000.f), FLinearColor(0.32f, 0.24f, 0.14f));

	for (int32 x = -2; x <= 2; ++x)
	{
		for (int32 y = -2; y <= 2; ++y)
		{
			if (x == 0 && y == 0)
			{
				continue;
			}
			Box(FVector(x * 700.f, y * 700.f, 90.f), FVector(3.4f, 3.4f, 1.8f), FLinearColor(0.45f, 0.28f, 0.16f));
		}
	}

	// Safe-house balcony south.
	Box(FVector(0.f, -1800.f, 180.f), FVector(8.f, 3.f, 0.3f), FLinearColor(0.3f, 0.22f, 0.16f));
	Box(FVector(0.f, -1950.f, 120.f), FVector(8.f, 1.2f, 2.4f), FLinearColor(0.28f, 0.2f, 0.14f));
	Cover(FVector(-400.f, -400.f, 0.f));
	Cover(FVector(450.f, 200.f, 0.f));

	// Case table + van alley north.
	Box(FVector(0.f, 0.f, 50.f), FVector(1.2f, 1.2f, 1.f), FLinearColor(0.15f, 0.1f, 0.08f));
	Wall(FVector(0.f, 2000.f, 140.f), FVector(16.f, 0.5f, 2.8f), FLinearColor(0.25f, 0.2f, 0.15f));

	PlayerStartAt(FVector(0.f, -2300.f, 120.f), FRotator(0.f, 90.f, 0.f));
	Objective(TEXT("CONFIRM"), FVector(0.f, -1750.f, 220.f), false);
	Objective(TEXT("CASE"), FVector(0.f, 0.f, 80.f), false, FLinearColor(0.9f, 0.75f, 0.2f));
	Objective(TEXT("EXTRACT"), FVector(0.f, 1700.f, 80.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("CIV"), FVector(1400.f, 0.f, 80.f), false, FLinearColor(0.8f, 0.8f, 0.4f));

	SpawnAI(FVector(-600.f, 200.f, 100.f), EAshlineAIArchetype::CivilianIrregular);
	SpawnAI(FVector(600.f, 400.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(200.f, 900.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(-200.f, -200.f, 100.f), EAshlineAIArchetype::Officer);
}

void AAshlineGrayboxBuilder::BuildHoldfast()
{
	SpawnAtmosphere(FLinearColor(0.9f, 0.85f, 0.7f), 9.f, FLinearColor(0.4f, 0.38f, 0.3f), 0.012f);
	Floor(FVector::ZeroVector, FVector2D(8000.f, 6000.f), FLinearColor(0.22f, 0.2f, 0.14f));

	// Sandbag berm.
	for (int32 i = -6; i <= 6; ++i)
	{
		Box(FVector(i * 180.f, 0.f, 45.f), FVector(1.6f, 1.1f, 0.9f), FLinearColor(0.4f, 0.32f, 0.18f));
	}
	// Mortar pit.
	Cylinder(FVector(-200.f, -500.f, 40.f), FVector(3.5f, 3.5f, 0.4f), FLinearColor(0.25f, 0.22f, 0.16f));
	Cylinder(FVector(-200.f, -500.f, 90.f), FVector(0.5f, 0.5f, 1.1f), FLinearColor(0.15f, 0.15f, 0.12f));
	// Gate / relief.
	Wall(FVector(0.f, -1600.f, 140.f), FVector(18.f, 0.6f, 2.8f), FLinearColor(0.3f, 0.28f, 0.2f));
	Box(FVector(0.f, -1600.f, 80.f), FVector(3.f, 0.4f, 1.6f), FLinearColor(0.18f, 0.16f, 0.1f), false);

	Cover(FVector(400.f, 200.f, 0.f));
	Cover(FVector(-500.f, 180.f, 0.f));

	PlayerStartAt(FVector(0.f, -900.f, 120.f), FRotator(0.f, 90.f, 0.f));
	Objective(TEXT("BERM"), FVector(0.f, 80.f, 80.f), false);
	Objective(TEXT("MORTAR"), FVector(-200.f, -500.f, 80.f), false, FLinearColor(0.8f, 0.4f, 0.2f));
	Objective(TEXT("RELIEF"), FVector(0.f, -1500.f, 80.f), true, FLinearColor(0.3f, 0.7f, 1.f));
	Objective(TEXT("NOFALL"), FVector(900.f, 0.f, 80.f), false);

	SpawnAI(FVector(-300.f, 900.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(200.f, 1100.f, 100.f), EAshlineAIArchetype::Heavy);
	SpawnAI(FVector(700.f, 800.f, 100.f), EAshlineAIArchetype::MachineGunner);
	SpawnAI(FVector(-800.f, 700.f, 100.f), EAshlineAIArchetype::Breacher);
}

void AAshlineGrayboxBuilder::BuildNightGlass()
{
	SpawnAtmosphere(FLinearColor(0.25f, 0.35f, 0.7f), 2.4f, FLinearColor(0.04f, 0.05f, 0.1f), 0.035f);
	Floor(FVector::ZeroVector, FVector2D(7000.f, 5000.f), FLinearColor(0.12f, 0.12f, 0.14f));

	// Ridge.
	Box(FVector(0.f, 0.f, 80.f), FVector(40.f, 8.f, 1.6f), FLinearColor(0.18f, 0.17f, 0.16f));
	// Nest Alpha (west kiln).
	Box(FVector(-1600.f, 200.f, 220.f), FVector(4.f, 4.f, 3.5f), FLinearColor(0.28f, 0.16f, 0.1f));
	Cylinder(FVector(-1600.f, 200.f, 420.f), FVector(1.2f, 1.2f, 1.4f), FLinearColor(0.2f, 0.12f, 0.08f));
	// Nest Bravo water tower.
	Cylinder(FVector(1600.f, -150.f, 240.f), FVector(1.6f, 1.6f, 4.6f), FLinearColor(0.25f, 0.28f, 0.3f));
	Cylinder(FVector(1600.f, -150.f, 500.f), FVector(3.2f, 3.2f, 0.8f), FLinearColor(0.2f, 0.22f, 0.25f));
	Cover(FVector(-400.f, 80.f, 160.f));
	Cover(FVector(400.f, -60.f, 160.f));

	PlayerStartAt(FVector(0.f, -900.f, 200.f), FRotator(0.f, 90.f, 0.f));
	Objective(TEXT("NEST_A"), FVector(-1600.f, 200.f, 280.f), false);
	Objective(TEXT("NEST_B"), FVector(1600.f, -150.f, 280.f), false);
	Objective(TEXT("OWN"), FVector(0.f, 0.f, 200.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("SILENT"), FVector(0.f, 600.f, 200.f), false);

	SpawnAI(FVector(-1600.f, 80.f, 280.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(1600.f, -40.f, 280.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(200.f, 200.f, 220.f), EAshlineAIArchetype::Scout);
}

void AAshlineGrayboxBuilder::BuildConvoyGhost()
{
	SpawnAtmosphere(FLinearColor(0.85f, 0.75f, 0.45f), 8.f, FLinearColor(0.45f, 0.38f, 0.2f), 0.018f);
	Floor(FVector::ZeroVector, FVector2D(10000.f, 4000.f), FLinearColor(0.42f, 0.36f, 0.2f));

	// Highway strip.
	Box(FVector(0.f, 0.f, 8.f), FVector(90.f, 8.f, 0.12f), FLinearColor(0.18f, 0.18f, 0.16f));
	// Lead, tankers, command wagon.
	Box(FVector(-800.f, 0.f, 80.f), FVector(4.5f, 2.2f, 1.6f), FLinearColor(0.15f, 0.16f, 0.12f));
	Box(FVector(200.f, 40.f, 90.f), FVector(5.5f, 2.4f, 1.8f), FLinearColor(0.35f, 0.18f, 0.08f));
	Box(FVector(900.f, -30.f, 90.f), FVector(5.5f, 2.4f, 1.8f), FLinearColor(0.35f, 0.18f, 0.08f));
	Box(FVector(1800.f, 20.f, 80.f), FVector(4.2f, 2.2f, 1.6f), FLinearColor(0.1f, 0.12f, 0.18f));
	Cover(FVector(-400.f, -400.f, 0.f));
	Cover(FVector(400.f, 380.f, 0.f));
	Cover(FVector(1400.f, -360.f, 0.f));

	PlayerStartAt(FVector(-2200.f, -500.f, 120.f), FRotator(0.f, 10.f, 0.f));
	Objective(TEXT("LEAD"), FVector(-800.f, 0.f, 80.f), false);
	Objective(TEXT("FUEL"), FVector(550.f, 0.f, 80.f), false, FLinearColor(0.9f, 0.35f, 0.1f));
	Objective(TEXT("CMD"), FVector(1800.f, 20.f, 80.f), false);
	Objective(TEXT("GHOST"), FVector(2600.f, 0.f, 80.f), true, FLinearColor(0.3f, 0.6f, 1.f));

	SpawnAI(FVector(-600.f, 180.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(200.f, -200.f, 100.f), EAshlineAIArchetype::MachineGunner);
	SpawnAI(FVector(1800.f, 200.f, 100.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(900.f, 220.f, 100.f), EAshlineAIArchetype::Breacher);
}

void AAshlineGrayboxBuilder::BuildAshHarbor()
{
	SpawnAtmosphere(FLinearColor(0.55f, 0.65f, 0.8f), 6.5f, FLinearColor(0.15f, 0.2f, 0.28f), 0.02f);
	Floor(FVector::ZeroVector, FVector2D(8000.f, 6000.f), FLinearColor(0.16f, 0.17f, 0.18f));
	// Water.
	Box(FVector(1800.f, 0.f, 6.f), FVector(30.f, 50.f, 0.08f), FLinearColor(0.08f, 0.14f, 0.22f));
	// Dock + crane.
	Box(FVector(-400.f, 0.f, 40.f), FVector(22.f, 16.f, 0.6f), FLinearColor(0.28f, 0.26f, 0.22f));
	Box(FVector(-900.f, -400.f, 280.f), FVector(2.f, 2.f, 5.4f), FLinearColor(0.55f, 0.4f, 0.15f));
	Box(FVector(-700.f, -400.f, 520.f), FVector(8.f, 1.2f, 0.5f), FLinearColor(0.5f, 0.38f, 0.12f));
	// Freighter.
	Box(FVector(1600.f, 0.f, 140.f), FVector(18.f, 6.f, 2.6f), FLinearColor(0.2f, 0.22f, 0.25f));
	Box(FVector(2000.f, 0.f, 260.f), FVector(4.f, 5.f, 2.2f), FLinearColor(0.18f, 0.2f, 0.24f));
	Cover(FVector(-200.f, 200.f, 40.f));
	Cover(FVector(200.f, -180.f, 40.f));

	PlayerStartAt(FVector(-1800.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("CRANE"), FVector(-900.f, -400.f, 120.f), false);
	Objective(TEXT("BOARD"), FVector(1200.f, 0.f, 180.f), false);
	Objective(TEXT("CARGO"), FVector(2000.f, 0.f, 200.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("HOSTAGE"), FVector(-200.f, 700.f, 80.f), false);

	SpawnAI(FVector(-600.f, 150.f, 120.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(200.f, -120.f, 120.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(1500.f, 180.f, 220.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(1800.f, -160.f, 220.f), EAshlineAIArchetype::Heavy);
}

void AAshlineGrayboxBuilder::BuildWhiteout()
{
	SpawnAtmosphere(FLinearColor(0.8f, 0.85f, 0.95f), 4.f, FLinearColor(0.7f, 0.75f, 0.82f), 0.06f);
	Floor(FVector::ZeroVector, FVector2D(9000.f, 5000.f), FLinearColor(0.78f, 0.8f, 0.84f));

	for (int32 i = 0; i < 8; ++i)
	{
		Cylinder(FVector(-2400.f + i * 550.f, FMath::Sin(i * 0.8f) * 180.f, 80.f), FVector(0.25f, 0.25f, 1.6f), FLinearColor(0.2f, 0.2f, 0.22f));
	}
	// Beacon + post + sled LZ.
	Cylinder(FVector(1400.f, 0.f, 160.f), FVector(1.4f, 1.4f, 3.2f), FLinearColor(0.35f, 0.4f, 0.55f));
	Sphere(FVector(1400.f, 0.f, 360.f), FVector(0.8f, 0.8f, 0.8f), FLinearColor(0.2f, 0.6f, 1.f));
	Box(FVector(1400.f, 400.f, 80.f), FVector(5.f, 4.f, 1.6f), FLinearColor(0.45f, 0.48f, 0.52f));
	Box(FVector(2600.f, 0.f, 30.f), FVector(8.f, 6.f, 0.4f), FLinearColor(0.55f, 0.3f, 0.15f));

	PlayerStartAt(FVector(-2800.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("WIRE"), FVector(-1800.f, 0.f, 80.f), false);
	Objective(TEXT("BEACON"), FVector(1400.f, 0.f, 80.f), false, FLinearColor(0.2f, 0.6f, 1.f));
	Objective(TEXT("CREW"), FVector(2600.f, 0.f, 80.f), true);
	Objective(TEXT("STORM"), FVector(1400.f, 400.f, 80.f), false);

	SpawnAI(FVector(-400.f, 200.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(800.f, -180.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(1600.f, 250.f, 100.f), EAshlineAIArchetype::Marksman);
}

void AAshlineGrayboxBuilder::BuildCatacomb()
{
	SpawnAtmosphere(FLinearColor(0.35f, 0.3f, 0.25f), 1.6f, FLinearColor(0.05f, 0.04f, 0.03f), 0.05f);
	Floor(FVector::ZeroVector, FVector2D(8000.f, 4000.f), FLinearColor(0.12f, 0.1f, 0.08f));

	auto Tunnel = [this](float X0, float X1, float Y)
	{
		Wall(FVector((X0 + X1) * 0.5f, Y - 220.f, 160.f), FVector(FMath::Abs(X1 - X0) / 100.f, 0.4f, 3.2f), FLinearColor(0.18f, 0.15f, 0.12f));
		Wall(FVector((X0 + X1) * 0.5f, Y + 220.f, 160.f), FVector(FMath::Abs(X1 - X0) / 100.f, 0.4f, 3.2f), FLinearColor(0.18f, 0.15f, 0.12f));
	};
	Tunnel(-2500.f, -800.f, 0.f);
	Tunnel(-800.f, 400.f, 0.f);
	Tunnel(400.f, 1600.f, 0.f);
	// Platforms.
	Box(FVector(-1600.f, 0.f, 20.f), FVector(8.f, 6.f, 0.3f), FLinearColor(0.2f, 0.18f, 0.14f));
	Box(FVector(-200.f, 0.f, 20.f), FVector(8.f, 6.f, 0.3f), FLinearColor(0.2f, 0.18f, 0.14f));
	Box(FVector(1000.f, 0.f, 20.f), FVector(8.f, 6.f, 0.3f), FLinearColor(0.2f, 0.18f, 0.14f));
	// Server racks.
	for (int32 i = 0; i < 5; ++i)
	{
		Box(FVector(2200.f, -300.f + i * 150.f, 110.f), FVector(1.4f, 1.f, 2.2f), FLinearColor(0.1f, 0.12f, 0.16f));
	}
	Cover(FVector(-1600.f, 120.f, 0.f));
	Cover(FVector(-200.f, -140.f, 0.f));
	Cover(FVector(1000.f, 100.f, 0.f));

	PlayerStartAt(FVector(-2800.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("PUSH"), FVector(-200.f, 0.f, 80.f), false);
	Objective(TEXT("DUMP"), FVector(2200.f, 0.f, 80.f), false, FLinearColor(0.3f, 0.8f, 0.4f));
	Objective(TEXT("COLLAPSE"), FVector(2800.f, 0.f, 80.f), true);
	Objective(TEXT("INTEL"), FVector(2200.f, 400.f, 80.f), false);

	SpawnAI(FVector(-1400.f, 80.f, 100.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(-200.f, -80.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(1000.f, 80.f, 100.f), EAshlineAIArchetype::Heavy);
	SpawnAI(FVector(2200.f, -200.f, 100.f), EAshlineAIArchetype::Officer);
}

void AAshlineGrayboxBuilder::BuildRidgeWire()
{
	SpawnAtmosphere(FLinearColor(0.6f, 0.7f, 0.85f), 7.f, FLinearColor(0.3f, 0.35f, 0.4f), 0.02f);
	Floor(FVector::ZeroVector, FVector2D(7000.f, 5000.f), FLinearColor(0.2f, 0.22f, 0.16f));

	Box(FVector(-1600.f, 0.f, 40.f), FVector(16.f, 10.f, 0.8f), FLinearColor(0.24f, 0.26f, 0.18f));
	Box(FVector(-200.f, 200.f, 140.f), FVector(14.f, 10.f, 0.8f), FLinearColor(0.26f, 0.28f, 0.2f));
	Box(FVector(1200.f, 0.f, 260.f), FVector(12.f, 10.f, 0.8f), FLinearColor(0.28f, 0.3f, 0.22f));
	Cylinder(FVector(1200.f, 0.f, 520.f), FVector(0.5f, 0.5f, 5.f), FLinearColor(0.6f, 0.6f, 0.55f));
	Box(FVector(1600.f, 500.f, 280.f), FVector(2.5f, 2.5f, 1.4f), FLinearColor(0.3f, 0.15f, 0.1f));
	Cover(FVector(-800.f, 80.f, 80.f));
	Cover(FVector(400.f, 160.f, 180.f));

	PlayerStartAt(FVector(-2200.f, 0.f, 140.f), FRotator::ZeroRotator);
	Objective(TEXT("CLIMB"), FVector(1200.f, 0.f, 320.f), false);
	Objective(TEXT("PLANT"), FVector(1200.f, 80.f, 320.f), false, FLinearColor(0.4f, 0.8f, 1.f));
	Objective(TEXT("HOLD"), FVector(900.f, 0.f, 320.f), true);
	Objective(TEXT("DRONE"), FVector(1600.f, 500.f, 320.f), false);

	SpawnAI(FVector(-200.f, 200.f, 220.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(1000.f, -180.f, 340.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(1400.f, 200.f, 340.f), EAshlineAIArchetype::Rifleman);
}

void AAshlineGrayboxBuilder::BuildFalseFlag()
{
	SpawnAtmosphere(FLinearColor(0.2f, 0.25f, 0.45f), 2.8f, FLinearColor(0.06f, 0.06f, 0.1f), 0.028f);
	Floor(FVector::ZeroVector, FVector2D(7000.f, 5000.f), FLinearColor(0.1f, 0.1f, 0.12f));

	// Campus block.
	Wall(FVector(0.f, -900.f, 180.f), FVector(28.f, 0.5f, 3.6f), FLinearColor(0.22f, 0.22f, 0.26f));
	Wall(FVector(0.f, 900.f, 180.f), FVector(28.f, 0.5f, 3.6f), FLinearColor(0.22f, 0.22f, 0.26f));
	Wall(FVector(-1400.f, 0.f, 180.f), FVector(0.5f, 18.f, 3.6f), FLinearColor(0.22f, 0.22f, 0.26f));
	Wall(FVector(1400.f, 0.f, 180.f), FVector(0.5f, 18.f, 3.6f), FLinearColor(0.22f, 0.22f, 0.26f));
	// Interior rooms.
	Wall(FVector(-400.f, 0.f, 140.f), FVector(0.4f, 12.f, 2.8f), FLinearColor(0.28f, 0.26f, 0.3f));
	Wall(FVector(400.f, 200.f, 140.f), FVector(10.f, 0.4f, 2.8f), FLinearColor(0.28f, 0.26f, 0.3f));
	Box(FVector(200.f, 400.f, 80.f), FVector(3.f, 2.f, 1.4f), FLinearColor(0.15f, 0.16f, 0.2f));
	// Loading dock.
	Box(FVector(1800.f, 0.f, 50.f), FVector(6.f, 8.f, 1.f), FLinearColor(0.18f, 0.16f, 0.12f));
	Cover(FVector(-200.f, -200.f, 0.f));
	Cover(FVector(600.f, 0.f, 0.f));

	PlayerStartAt(FVector(-1800.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("INFIL"), FVector(-1200.f, 0.f, 80.f), false);
	Objective(TEXT("SWAP"), FVector(200.f, 400.f, 80.f), false, FLinearColor(0.8f, 0.3f, 0.3f));
	Objective(TEXT("WALK"), FVector(1800.f, 0.f, 80.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("CLEAN"), FVector(200.f, -500.f, 80.f), false);

	SpawnAI(FVector(-200.f, 200.f, 100.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(500.f, -100.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(1100.f, 150.f, 100.f), EAshlineAIArchetype::Scout);
}

void AAshlineGrayboxBuilder::BuildLastTrain()
{
	SpawnAtmosphere(FLinearColor(0.4f, 0.45f, 0.5f), 4.5f, FLinearColor(0.12f, 0.12f, 0.12f), 0.025f);
	Floor(FVector::ZeroVector, FVector2D(10000.f, 4000.f), FLinearColor(0.16f, 0.16f, 0.15f));

	Box(FVector(-2200.f, 0.f, 40.f), FVector(16.f, 10.f, 0.6f), FLinearColor(0.22f, 0.2f, 0.18f));
	for (int32 i = 0; i < 4; ++i)
	{
		Box(FVector(-400.f + i * 900.f, 0.f, 130.f), FVector(7.5f, 3.2f, 2.4f), FLinearColor(0.18f, 0.14f, 0.1f));
	}
	Box(FVector(3200.f, 0.f, 140.f), FVector(5.f, 3.f, 2.6f), FLinearColor(0.12f, 0.12f, 0.14f));
	Cover(FVector(-2000.f, 200.f, 40.f));
	Cover(FVector(-400.f, 220.f, 40.f));
	Cover(FVector(1400.f, -220.f, 40.f));

	PlayerStartAt(FVector(-2800.f, 0.f, 140.f), FRotator::ZeroRotator);
	Objective(TEXT("PLATFORM"), FVector(-2200.f, 0.f, 80.f), false);
	Objective(TEXT("RIDE"), FVector(500.f, 0.f, 160.f), false);
	Objective(TEXT("ENGINE"), FVector(3200.f, 0.f, 160.f), true);
	Objective(TEXT("WITNESS"), FVector(1400.f, 0.f, 160.f), false);

	SpawnAI(FVector(-2000.f, 180.f, 140.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(-200.f, -160.f, 200.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(1400.f, 160.f, 200.f), EAshlineAIArchetype::Heavy);
	SpawnAI(FVector(2800.f, -120.f, 200.f), EAshlineAIArchetype::Officer);
}

void AAshlineGrayboxBuilder::BuildAshlineFinale()
{
	SpawnAtmosphere(FLinearColor(0.55f, 0.15f, 0.12f), 3.5f, FLinearColor(0.12f, 0.03f, 0.02f), 0.04f);
	Floor(FVector::ZeroVector, FVector2D(9000.f, 5000.f), FLinearColor(0.08f, 0.07f, 0.07f));

	// Buried gate.
	Wall(FVector(-1600.f, 0.f, 200.f), FVector(0.8f, 18.f, 4.f), FLinearColor(0.16f, 0.14f, 0.14f));
	Box(FVector(-1600.f, 0.f, 120.f), FVector(0.3f, 4.f, 2.4f), FLinearColor(0.4f, 0.1f, 0.08f), false);
	// Spine chambers.
	for (int32 i = 0; i < 3; ++i)
	{
		const float X = -400.f + i * 1200.f;
		Wall(FVector(X, -700.f, 180.f), FVector(10.f, 0.5f, 3.6f), FLinearColor(0.2f, 0.12f, 0.12f));
		Wall(FVector(X, 700.f, 180.f), FVector(10.f, 0.5f, 3.6f), FLinearColor(0.2f, 0.12f, 0.12f));
		Cylinder(FVector(X + 200.f, 0.f, 160.f), FVector(1.8f, 1.8f, 3.f), FLinearColor(0.35f, 0.08f, 0.08f));
		Cover(FVector(X - 150.f, 180.f, 0.f));
	}
	// Lift.
	Box(FVector(3200.f, 0.f, 40.f), FVector(5.f, 5.f, 0.5f), FLinearColor(0.25f, 0.22f, 0.1f));
	Box(FVector(3200.f, 0.f, 200.f), FVector(0.3f, 0.3f, 4.f), FLinearColor(0.4f, 0.35f, 0.15f));

	PlayerStartAt(FVector(-2400.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("BREACH"), FVector(-1500.f, 0.f, 80.f), false, FLinearColor(0.9f, 0.2f, 0.15f));
	Objective(TEXT("CUT"), FVector(800.f, 0.f, 80.f), false, FLinearColor(1.f, 0.3f, 0.2f));
	Objective(TEXT("COLLAPSE"), FVector(3200.f, 0.f, 80.f), true, FLinearColor(0.9f, 0.8f, 0.3f));
	Objective(TEXT("END"), FVector(2000.f, 0.f, 80.f), false);

	SpawnAI(FVector(-1200.f, 200.f, 100.f), EAshlineAIArchetype::Heavy);
	SpawnAI(FVector(-200.f, -180.f, 100.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(800.f, 220.f, 100.f), EAshlineAIArchetype::MachineGunner);
	SpawnAI(FVector(2000.f, -160.f, 100.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(2800.f, 140.f, 100.f), EAshlineAIArchetype::Rifleman);
}
