#include "World/AshlineGrayboxBuilder.h"

#include "Engine/GameInstance.h"
#include "Presentation/AshlineAudioDirector.h"
#include "Presentation/AshlinePresentationLibrary.h"

void AAshlineGrayboxBuilder::DressMission(EAshlineMissionId MissionId)
{
	switch (MissionId)
	{
	case EAshlineMissionId::ASH_01_WireCut:
		Practical(FVector(800.f, 200.f, 320.f), FLinearColor(1.f, 0.55f, 0.2f), 7000.f, 1400.f);
		Practical(FVector(-8000.f, 0.f, 280.f), FLinearColor(0.4f, 0.7f, 1.f), 3200.f, 1100.f);
		Practical(FVector(-4200.f, 1800.f, 260.f), FLinearColor(1.f, 0.7f, 0.3f), 2800.f, 900.f);
		Practical(FVector(14800.f, 0.f, 240.f), FLinearColor(0.35f, 0.55f, 1.f), 4000.f, 1200.f);
		Spot(FVector(-200.f, 0.f, 720.f), FRotator(-50.f, 0.f, 0.f), FLinearColor(1.f, 0.72f, 0.35f), 5000.f, 1800.f, 22.f, 40.f, false);
		ScatterFoliage(FVector(-13000.f, 0.f, 0.f), 2200.f, 10, 8);
		ScatterFoliage(FVector(9000.f, -3500.f, 0.f), 1600.f, 6, 6);
		GrassPatch(FVector(-12000.f, 400.f, 0.f), 400.f, 12);
		DecalMark(FVector(-200.f, 0.f, 4.f), FRotator(-90.f, 20.f, 0.f), FVector(420.f, 260.f, 16.f));
		ScatterKitProps(FVector(-4200.f, 0.f, 0.f), 1400.f, 8);
		ScatterKitProps(FVector(600.f, 0.f, 0.f), 900.f, 6);
		break;
	case EAshlineMissionId::ASH_02_DustMarket:
		Practical(FVector(0.f, 0.f, 280.f), FLinearColor(1.f, 0.7f, 0.25f), 5000.f, 1200.f);
		Practical(FVector(-2400.f, -2400.f, 220.f), FLinearColor(1.f, 0.45f, 0.15f), 2800.f, 700.f);
		Practical(FVector(2400.f, 2400.f, 220.f), FLinearColor(1.f, 0.5f, 0.2f), 2800.f, 700.f);
		Practical(FVector(0.f, 6200.f, 200.f), FLinearColor(0.4f, 0.7f, 1.f), 3500.f, 900.f);
		DecalMark(FVector(0.f, 0.f, 4.f), FRotator(-90.f, 0.f, 0.f), FVector(280.f, 280.f, 12.f));
		ScatterKitProps(FVector(0.f, 0.f, 0.f), 2800.f, 12);
		break;
	case EAshlineMissionId::ASH_03_Holdfast:
		Practical(FVector(-400.f, -1800.f, 180.f), FLinearColor(1.f, 0.85f, 0.5f), 4200.f, 900.f);
		Practical(FVector(0.f, 200.f, 160.f), FLinearColor(1.f, 0.8f, 0.45f), 3000.f, 700.f);
		ScatterFoliage(FVector(0.f, 4500.f, 0.f), 2400.f, 8, 6);
		ScatterKitProps(FVector(0.f, -800.f, 0.f), 1600.f, 8);
		break;
	case EAshlineMissionId::ASH_04_NightGlass:
		Practical(FVector(-7000.f, 400.f, 520.f), FLinearColor(1.f, 0.2f, 0.1f), 3600.f, 1100.f);
		Practical(FVector(7000.f, -300.f, 640.f), FLinearColor(0.4f, 0.7f, 1.f), 4200.f, 1300.f);
		Practical(FVector(0.f, 0.f, 280.f), FLinearColor(0.6f, 0.75f, 1.f), 2200.f, 800.f);
		ScatterFoliage(FVector(0.f, -2500.f, 80.f), 1800.f, 6, 5);
		break;
	case EAshlineMissionId::ASH_05_ConvoyGhost:
		Practical(FVector(-4000.f, 0.f, 180.f), FLinearColor(1.f, 0.85f, 0.4f), 2800.f, 500.f);
		Practical(FVector(1200.f, 40.f, 180.f), FLinearColor(1.f, 0.35f, 0.08f), 5000.f, 700.f);
		Practical(FVector(5200.f, -30.f, 180.f), FLinearColor(1.f, 0.35f, 0.08f), 5000.f, 700.f);
		ScatterFoliage(FVector(0.f, -2200.f, 0.f), 2000.f, 7, 8);
		break;
	case EAshlineMissionId::ASH_06_AshHarbor:
		Practical(FVector(-3500.f, -1600.f, 620.f), FLinearColor(1.f, 0.7f, 0.25f), 9000.f, 1800.f);
		Practical(FVector(6200.f, 0.f, 320.f), FLinearColor(0.5f, 0.7f, 1.f), 4500.f, 1200.f);
		Practical(FVector(-800.f, 400.f, 180.f), FLinearColor(1.f, 0.8f, 0.4f), 2400.f, 600.f);
		DecalMark(FVector(-800.f, 0.f, 44.f), FRotator(-90.f, 0.f, 0.f), FVector(480.f, 280.f, 14.f));
		ScatterKitProps(FVector(-800.f, 0.f, 40.f), 2000.f, 10);
		break;
	case EAshlineMissionId::ASH_07_Whiteout:
		Practical(FVector(7000.f, 0.f, 400.f), FLinearColor(0.3f, 0.7f, 1.f), 10000.f, 2200.f);
		ScatterFoliage(FVector(-2000.f, 800.f, 0.f), 2800.f, 9, 4);
		break;
	case EAshlineMissionId::ASH_08_Catacomb:
		Practical(FVector(-8000.f, 0.f, 180.f), FLinearColor(1.f, 0.55f, 0.2f), 2200.f, 500.f);
		Practical(FVector(-2000.f, 0.f, 180.f), FLinearColor(1.f, 0.45f, 0.15f), 2200.f, 500.f);
		Practical(FVector(4000.f, 0.f, 180.f), FLinearColor(1.f, 0.4f, 0.12f), 2200.f, 500.f);
		Practical(FVector(11000.f, 0.f, 200.f), FLinearColor(0.2f, 0.8f, 0.4f), 2800.f, 700.f);
		InteriorVolume(FVector(2000.f, 0.f, 160.f), FVector(14000.f, 900.f, 400.f), 0.18f, -0.7f);
		break;
	case EAshlineMissionId::ASH_09_RidgeWire:
		Practical(FVector(6500.f, 0.f, 820.f), FLinearColor(0.5f, 0.85f, 1.f), 5500.f, 1400.f);
		ScatterFoliage(FVector(0.f, 0.f, 200.f), 2200.f, 8, 6);
		break;
	case EAshlineMissionId::ASH_10_FalseFlag:
		Practical(FVector(-1200.f, 0.f, 260.f), FLinearColor(0.4f, 0.7f, 1.f), 2800.f, 700.f);
		Practical(FVector(800.f, 1200.f, 220.f), FLinearColor(1.f, 0.15f, 0.1f), 3200.f, 600.f);
		Practical(FVector(7200.f, 0.f, 200.f), FLinearColor(1.f, 0.75f, 0.3f), 3800.f, 900.f);
		InteriorVolume(FVector(0.f, 0.f, 180.f), FVector(2800.f, 2200.f, 420.f), 0.12f, -0.25f);
		DecalMark(FVector(0.f, 0.f, 4.f), FRotator(-90.f, 45.f, 0.f), FVector(220.f, 160.f, 12.f));
		break;
	case EAshlineMissionId::ASH_11_LastTrain:
		Practical(FVector(-10000.f, 0.f, 180.f), FLinearColor(1.f, 0.7f, 0.3f), 3200.f, 700.f);
		Practical(FVector(0.f, 0.f, 240.f), FLinearColor(1.f, 0.35f, 0.1f), 2600.f, 600.f);
		Practical(FVector(14000.f, 0.f, 260.f), FLinearColor(0.3f, 0.6f, 1.f), 4200.f, 900.f);
		break;
	case EAshlineMissionId::ASH_12_Ashline:
		Practical(FVector(-9000.f, 0.f, 260.f), FLinearColor(1.f, 0.12f, 0.06f), 6000.f, 1100.f);
		Practical(FVector(-1500.f, 0.f, 240.f), FLinearColor(1.f, 0.2f, 0.08f), 4200.f, 800.f);
		Practical(FVector(4000.f, 0.f, 240.f), FLinearColor(1.f, 0.18f, 0.08f), 4200.f, 800.f);
		Practical(FVector(9500.f, 0.f, 240.f), FLinearColor(1.f, 0.16f, 0.06f), 4200.f, 800.f);
		Practical(FVector(15000.f, 0.f, 280.f), FLinearColor(1.f, 0.75f, 0.25f), 7500.f, 1300.f);
		InteriorVolume(FVector(3000.f, 0.f, 180.f), FVector(16000.f, 1600.f, 500.f), 0.16f, -0.55f);
		break;
	default:
		break;
	}

	if (ActiveMood.FoliageDensity > 0)
	{
		const FVector Origin = LastPlayerStartLocation + FVector(1800.f, 0.f, 0.f);
		ScatterFoliage(Origin, 1400.f, FMath::Min(ActiveMood.FoliageDensity, 14), FMath::Min(ActiveMood.FoliageDensity, 10));
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
		{
			Audio->StartMusicBed(this, MissionId);
		}
	}
}

void AAshlineGrayboxBuilder::BuildWireCut()
{
	// Night raid: ~360m E-W x 200m N-S. Woods → trench → outer fence → motor pool → relay compound → creek exfil.
	SpawnAtmosphere(FLinearColor(0.35f, 0.45f, 0.75f), 3.2f, FLinearColor(0.05f, 0.07f, 0.12f), 0.03f);
	Floor(FVector::ZeroVector, FVector2D(40000.f, 24000.f), FLinearColor(0.08f, 0.1f, 0.07f));

	RoadStrip(FVector(-16000.f, 0.f, 0.f), FVector(16000.f, 0.f, 0.f), 820.f);

	// Drop woods / west berms.
	TreeBelt(FVector(-17000.f, -2800.f, 0.f), FVector(-11000.f, -2600.f, 0.f), 9, 400.f);
	TreeBelt(FVector(-17000.f, 2800.f, 0.f), FVector(-11000.f, 2400.f, 0.f), 9, 360.f);
	CoverLine(FVector(-15000.f, -220.f, 0.f), FVector(-11500.f, 180.f, 0.f), 7);

	// Approach trench.
	Wall(FVector(-10000.f, -520.f, 90.f), FVector(28.f, 0.7f, 1.8f), FLinearColor(0.12f, 0.16f, 0.1f));
	Wall(FVector(-10000.f, 520.f, 90.f), FVector(28.f, 0.7f, 1.8f), FLinearColor(0.12f, 0.16f, 0.1f));
	CoverLine(FVector(-11200.f, -280.f, 0.f), FVector(-8600.f, 260.f, 0.f), 6);
	Sandbag(FVector(-10800.f, -180.f, 0.f));
	Sandbag(FVector(-10200.f, 210.f, 0.f));

	// Outer fence (west gate).
	WalledYard(FVector(-8000.f, 0.f, 0.f), FVector2D(1800.f, 3200.f), 320.f, FLinearColor(0.16f, 0.15f, 0.12f), 0, 700.f);
	Doorway(FVector(-9800.f, 0.f, 0.f));
	Watchtower(FVector(-8000.f, -3000.f, 0.f), 820.f);
	Watchtower(FVector(-8000.f, 3000.f, 0.f), 780.f);
	CoverLine(FVector(-9200.f, -900.f, 0.f), FVector(-7000.f, 800.f, 0.f), 6);

	// Motor pool / generator yard.
	WalledYard(FVector(-4300.f, 1600.f, 0.f), FVector2D(1400.f, 1200.f), 280.f, FLinearColor(0.2f, 0.18f, 0.14f), 2, 500.f);
	VehicleHull(FVector(-4600.f, 1500.f, 0.f), FRotator(0.f, 20.f, 0.f), FLinearColor(0.12f, 0.11f, 0.09f), 5.4f);
	VehicleHull(FVector(-4000.f, 1900.f, 0.f), FRotator(0.f, -15.f, 0.f), FLinearColor(0.14f, 0.12f, 0.08f), 4.6f);
	Cylinder(FVector(-4100.f, 1100.f, 140.f), FVector(1.1f, 1.1f, 2.4f), FLinearColor(0.28f, 0.22f, 0.1f), EAshlineSurface::Metal);
	Cover(FVector(-4800.f, 900.f, 0.f));

	// Main relay compound.
	WalledYard(FVector(200.f, 0.f, 0.f), FVector2D(2800.f, 2400.f), 360.f, FLinearColor(0.2f, 0.18f, 0.14f), 0, 800.f);
	Doorway(FVector(-2600.f, 0.f, 0.f));
	Building(FVector(800.f, 200.f, 0.f), FVector2D(900.f, 720.f), 320.f, FLinearColor(0.16f, 0.18f, 0.22f));
	Cylinder(FVector(800.f, 200.f, 520.f), FVector(0.4f, 0.4f, 3.2f), FLinearColor(0.55f, 0.55f, 0.5f), EAshlineSurface::Metal);
	WindowStrip(FVector(800.f, 200.f, 240.f), 4, 110.f, FLinearColor(1.f, 0.55f, 0.15f));
	InteriorVolume(FVector(800.f, 200.f, 180.f), FVector(1000.f, 800.f, 360.f), 0.14f, -0.45f);
	Building(FVector(-400.f, 1400.f, 0.f), FVector2D(1400.f, 520.f), 240.f, FLinearColor(0.22f, 0.2f, 0.16f));
	Building(FVector(-200.f, -1500.f, 0.f), FVector2D(900.f, 480.f), 200.f, FLinearColor(0.2f, 0.17f, 0.14f));
	Watchtower(FVector(2600.f, 2200.f, 0.f), 980.f);
	Watchtower(FVector(2600.f, -2200.f, 0.f), 940.f);
	CoverLine(FVector(-1800.f, -400.f, 0.f), FVector(400.f, 500.f, 0.f), 7);
	Sandbag(FVector(-2200.f, -240.f, 0.f));
	Sandbag(FVector(-1900.f, 280.f, 0.f));
	Sandbag(FVector(200.f, 600.f, 0.f));

	// North drainage lane.
	Wall(FVector(-2000.f, 5200.f, 70.f), FVector(60.f, 0.5f, 1.4f), FLinearColor(0.1f, 0.14f, 0.1f));
	CoverLine(FVector(-5000.f, 4800.f, 0.f), FVector(2000.f, 5000.f, 0.f), 8);
	TreeBelt(FVector(-6000.f, 6200.f, 0.f), FVector(4000.f, 6400.f, 0.f), 10, 340.f);

	// South wreck lane.
	VehicleHull(FVector(-1200.f, -4800.f, 0.f), FRotator(0.f, 70.f, 0.f), FLinearColor(0.1f, 0.1f, 0.08f), 5.8f);
	CoverLine(FVector(-5000.f, -4600.f, 0.f), FVector(1500.f, -4200.f, 0.f), 7);
	Bush(FVector(-3000.f, -4000.f, 0.f));

	// Service road and fiber trench.
	CoverLine(FVector(3200.f, -200.f, 0.f), FVector(7000.f, 180.f, 0.f), 6);
	Building(FVector(5200.f, 900.f, 0.f), FVector2D(420.f, 360.f), 180.f, FLinearColor(0.18f, 0.16f, 0.12f));

	// Creek exfil + QRF berm.
	Box(FVector(14800.f, 0.f, 18.f), FVector(28.f, 48.f, 0.28f), FLinearColor(0.1f, 0.14f, 0.18f));
	Wall(FVector(12000.f, -1800.f, 120.f), FVector(0.6f, 22.f, 2.4f), FLinearColor(0.16f, 0.15f, 0.12f));
	Wall(FVector(12000.f, 1800.f, 120.f), FVector(0.6f, 22.f, 2.4f), FLinearColor(0.16f, 0.15f, 0.12f));
	CoverLine(FVector(11000.f, -400.f, 0.f), FVector(15000.f, 300.f, 0.f), 6);
	TreeBelt(FVector(13000.f, -4200.f, 0.f), FVector(17000.f, -3800.f, 0.f), 6, 300.f);

	PlayerStartAt(FVector(-16000.f, 0.f, 120.f), FRotator(0.f, 0.f, 0.f));
	Objective(TEXT("INFIL"), FVector(-8000.f, 0.f, 80.f), false, FLinearColor(0.4f, 0.8f, 0.4f));
	Objective(TEXT("CUT"), FVector(800.f, 480.f, 80.f), false, FLinearColor(0.9f, 0.4f, 0.2f));
	Objective(TEXT("EXFIL"), FVector(15200.f, 0.f, 80.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("GHOST"), FVector(-200.f, 5000.f, 80.f), false, FLinearColor(0.7f, 0.7f, 0.9f));

	SpawnAI(FVector(-8600.f, -240.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(-7800.f, 400.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(-4300.f, 1400.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(-4000.f, 2000.f, 100.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(-1600.f, -200.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(-200.f, 900.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(700.f, -80.f, 100.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(2400.f, 1800.f, 100.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(2400.f, -1800.f, 100.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(5000.f, 200.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(11800.f, -160.f, 100.f), EAshlineAIArchetype::MachineGunner);
	SpawnAI(FVector(14600.f, 280.f, 100.f), EAshlineAIArchetype::Scout);
}

void AAshlineGrayboxBuilder::BuildDustMarket()
{
	// Urban bazaar: ~240m square, 5x5 blocks, balcony south, van alley north.
	SpawnAtmosphere(FLinearColor(0.95f, 0.72f, 0.4f), 7.5f, FLinearColor(0.35f, 0.25f, 0.12f), 0.022f);
	Floor(FVector::ZeroVector, FVector2D(28000.f, 28000.f), FLinearColor(0.32f, 0.24f, 0.14f));

	for (int32 x = -4; x <= 4; ++x)
	{
		for (int32 y = -4; y <= 4; ++y)
		{
			if (FMath::Abs(x) + FMath::Abs(y) == 0)
			{
				continue;
			}
			if (x == 0 || y == 0)
			{
				continue; // keep cross streets open
			}
			const FLinearColor Tint(0.42f + (x * 0.01f), 0.26f, 0.15f + (y * 0.008f));
			Building(FVector(x * 1400.f, y * 1400.f, 0.f), FVector2D(780.f, 780.f), 220.f + (FMath::Abs(x) % 3) * 80.f, Tint);
		}
	}

	RoadStrip(FVector(0.f, -9000.f, 0.f), FVector(0.f, 9000.f, 0.f), 900.f);
	RoadStrip(FVector(-9000.f, 0.f, 0.f), FVector(9000.f, 0.f, 0.f), 900.f);
	CoverLine(FVector(-1800.f, -1800.f, 0.f), FVector(1800.f, 1600.f, 0.f), 8);

	Building(FVector(0.f, -7200.f, 0.f), FVector2D(1600.f, 520.f), 360.f, FLinearColor(0.3f, 0.22f, 0.16f));
	Box(FVector(0.f, -6800.f, 220.f), FVector(14.f, 4.f, 0.25f), FLinearColor(0.28f, 0.2f, 0.14f));
	WindowStrip(FVector(-2400.f, -2400.f, 180.f), 3, 90.f, FLinearColor(1.f, 0.6f, 0.2f));

	Box(FVector(0.f, 0.f, 50.f), FVector(1.4f, 1.4f, 1.f), FLinearColor(0.15f, 0.1f, 0.08f));
	WalledYard(FVector(0.f, 7200.f, 0.f), FVector2D(2200.f, 900.f), 280.f, FLinearColor(0.25f, 0.2f, 0.15f), 2, 700.f);
	VehicleHull(FVector(0.f, 6800.f, 0.f), FRotator(0.f, 90.f, 0.f), FLinearColor(0.12f, 0.1f, 0.08f), 5.2f);
	Bush(FVector(6200.f, -1800.f, 0.f));
	TreeBelt(FVector(-9000.f, 9000.f, 0.f), FVector(9000.f, 9000.f, 0.f), 8, 280.f);

	PlayerStartAt(FVector(0.f, -9200.f, 120.f), FRotator(0.f, 90.f, 0.f));
	Objective(TEXT("CONFIRM"), FVector(0.f, -6800.f, 260.f), false);
	Objective(TEXT("CASE"), FVector(0.f, 0.f, 80.f), false, FLinearColor(0.9f, 0.75f, 0.2f));
	Objective(TEXT("EXTRACT"), FVector(0.f, 6800.f, 80.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("CIV"), FVector(5600.f, 0.f, 80.f), false, FLinearColor(0.8f, 0.8f, 0.4f));

	SpawnAI(FVector(-2000.f, 400.f, 100.f), EAshlineAIArchetype::CivilianIrregular);
	SpawnAI(FVector(2000.f, 800.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(400.f, 2800.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(-800.f, -1200.f, 100.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(-3600.f, 1600.f, 100.f), EAshlineAIArchetype::CivilianIrregular);
	SpawnAI(FVector(3600.f, -800.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(200.f, 6400.f, 100.f), EAshlineAIArchetype::Breacher);
}

void AAshlineGrayboxBuilder::BuildHoldfast()
{
	// Firebase with 220m berm frontage, mortar pit, south gate, north assault field.
	SpawnAtmosphere(FLinearColor(0.9f, 0.85f, 0.7f), 9.f, FLinearColor(0.4f, 0.38f, 0.3f), 0.012f);
	Floor(FVector::ZeroVector, FVector2D(32000.f, 28000.f), FLinearColor(0.22f, 0.2f, 0.14f));

	for (int32 i = -12; i <= 12; ++i)
	{
		Box(FVector(i * 220.f, 200.f, 50.f), FVector(1.8f, 1.2f, 1.0f), FLinearColor(0.4f, 0.32f, 0.18f));
	}
	CoverLine(FVector(-2200.f, 400.f, 0.f), FVector(2200.f, 400.f, 0.f), 10);
	Sandbag(FVector(-400.f, 80.f, 0.f));
	Sandbag(FVector(400.f, 80.f, 0.f));
	Sandbag(FVector(-1200.f, 120.f, 0.f));
	Sandbag(FVector(1200.f, 120.f, 0.f));

	Cylinder(FVector(-600.f, -1800.f, 40.f), FVector(4.2f, 4.2f, 0.45f), FLinearColor(0.25f, 0.22f, 0.16f));
	Cylinder(FVector(-600.f, -1800.f, 110.f), FVector(0.55f, 0.55f, 1.2f), FLinearColor(0.15f, 0.15f, 0.12f));
	Building(FVector(900.f, -2200.f, 0.f), FVector2D(720.f, 480.f), 200.f, FLinearColor(0.28f, 0.24f, 0.16f));
	Watchtower(FVector(-2400.f, -400.f, 0.f), 760.f);
	Watchtower(FVector(2400.f, -400.f, 0.f), 760.f);

	WalledYard(FVector(0.f, -5200.f, 0.f), FVector2D(2800.f, 900.f), 300.f, FLinearColor(0.3f, 0.28f, 0.2f), 3, 800.f);
	Box(FVector(0.f, -4300.f, 80.f), FVector(4.f, 0.4f, 1.6f), FLinearColor(0.18f, 0.16f, 0.1f), false);
	VehicleHull(FVector(2200.f, -3600.f, 0.f), FRotator::ZeroRotator, FLinearColor(0.18f, 0.2f, 0.12f), 5.4f);

	CoverLine(FVector(-1800.f, 2800.f, 0.f), FVector(1800.f, 4200.f, 0.f), 8);
	TreeBelt(FVector(-6000.f, 7000.f, 0.f), FVector(6000.f, 7200.f, 0.f), 10, 300.f);

	PlayerStartAt(FVector(0.f, -2400.f, 120.f), FRotator(0.f, 90.f, 0.f));
	Objective(TEXT("BERM"), FVector(0.f, 250.f, 80.f), false);
	Objective(TEXT("MORTAR"), FVector(-600.f, -1800.f, 80.f), false, FLinearColor(0.8f, 0.4f, 0.2f));
	Objective(TEXT("RELIEF"), FVector(0.f, -5000.f, 80.f), true, FLinearColor(0.3f, 0.7f, 1.f));
	Objective(TEXT("NOFALL"), FVector(2400.f, 200.f, 80.f), false);

	SpawnAI(FVector(-800.f, 3600.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(400.f, 4200.f, 100.f), EAshlineAIArchetype::Heavy);
	SpawnAI(FVector(1600.f, 3400.f, 100.f), EAshlineAIArchetype::MachineGunner);
	SpawnAI(FVector(-2000.f, 3000.f, 100.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(2200.f, 2400.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(-1600.f, 4800.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(800.f, 5600.f, 100.f), EAshlineAIArchetype::Officer);
}

void AAshlineGrayboxBuilder::BuildNightGlass()
{
	// 320m glassworks ridge with stepped elevation, kiln west, water tower east.
	SpawnAtmosphere(FLinearColor(0.25f, 0.35f, 0.7f), 2.4f, FLinearColor(0.04f, 0.05f, 0.1f), 0.035f);
	Floor(FVector::ZeroVector, FVector2D(36000.f, 18000.f), FLinearColor(0.12f, 0.12f, 0.14f));

	Box(FVector(-4000.f, 0.f, 70.f), FVector(80.f, 14.f, 1.4f), FLinearColor(0.18f, 0.17f, 0.16f));
	Box(FVector(4000.f, 0.f, 140.f), FVector(70.f, 12.f, 2.2f), FLinearColor(0.2f, 0.18f, 0.16f));
	Box(FVector(0.f, 0.f, 90.f), FVector(30.f, 10.f, 1.6f), FLinearColor(0.19f, 0.17f, 0.15f));

	Building(FVector(-7000.f, 400.f, 160.f), FVector2D(720.f, 720.f), 380.f, FLinearColor(0.28f, 0.16f, 0.1f));
	Cylinder(FVector(-7000.f, 400.f, 620.f), FVector(1.4f, 1.4f, 1.8f), FLinearColor(0.2f, 0.12f, 0.08f));
	Cylinder(FVector(7000.f, -300.f, 280.f), FVector(1.8f, 1.8f, 6.2f), FLinearColor(0.25f, 0.28f, 0.3f), EAshlineSurface::Metal);
	Cylinder(FVector(7000.f, -300.f, 720.f), FVector(3.6f, 3.6f, 0.9f), FLinearColor(0.2f, 0.22f, 0.25f), EAshlineSurface::Metal);
	CoverLine(FVector(-2000.f, 200.f, 160.f), FVector(2000.f, -120.f, 220.f), 8);
	Watchtower(FVector(0.f, 1800.f, 80.f), 640.f);

	PlayerStartAt(FVector(0.f, -3600.f, 200.f), FRotator(0.f, 90.f, 0.f));
	Objective(TEXT("NEST_A"), FVector(-7000.f, 400.f, 360.f), false);
	Objective(TEXT("NEST_B"), FVector(7000.f, -300.f, 360.f), false);
	Objective(TEXT("OWN"), FVector(0.f, 0.f, 220.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("SILENT"), FVector(0.f, 2400.f, 220.f), false);

	SpawnAI(FVector(-6800.f, 200.f, 360.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(6800.f, -120.f, 360.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(400.f, 400.f, 240.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(-2400.f, -200.f, 220.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(2400.f, 180.f, 260.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(0.f, 1600.f, 280.f), EAshlineAIArchetype::Officer);
}

void AAshlineGrayboxBuilder::BuildConvoyGhost()
{
	// 500m highway ambush on salt flats with berms both sides.
	SpawnAtmosphere(FLinearColor(0.85f, 0.75f, 0.45f), 8.f, FLinearColor(0.45f, 0.38f, 0.2f), 0.018f);
	Floor(FVector::ZeroVector, FVector2D(52000.f, 18000.f), FLinearColor(0.42f, 0.36f, 0.2f));

	RoadStrip(FVector(-20000.f, 0.f, 0.f), FVector(22000.f, 0.f, 0.f), 1100.f);
	VehicleHull(FVector(-4000.f, 0.f, 0.f), FRotator::ZeroRotator, FLinearColor(0.14f, 0.15f, 0.11f), 5.2f);
	Box(FVector(1200.f, 40.f, 90.f), FVector(7.f, 2.6f, 2.0f), FLinearColor(0.35f, 0.18f, 0.08f));
	Box(FVector(5200.f, -30.f, 90.f), FVector(7.f, 2.6f, 2.0f), FLinearColor(0.35f, 0.18f, 0.08f));
	VehicleHull(FVector(9200.f, 20.f, 0.f), FRotator::ZeroRotator, FLinearColor(0.1f, 0.12f, 0.18f), 4.8f);
	CoverLine(FVector(-6000.f, -1600.f, 0.f), FVector(8000.f, -1400.f, 0.f), 10);
	CoverLine(FVector(-5000.f, 1500.f, 0.f), FVector(7000.f, 1700.f, 0.f), 9);
	Bush(FVector(-2000.f, -1800.f, 0.f));
	Bush(FVector(6000.f, 1900.f, 0.f));
	Watchtower(FVector(-8000.f, -2200.f, 0.f), 520.f);

	PlayerStartAt(FVector(-12000.f, -1800.f, 120.f), FRotator(0.f, 10.f, 0.f));
	Objective(TEXT("LEAD"), FVector(-4000.f, 0.f, 80.f), false);
	Objective(TEXT("FUEL"), FVector(3200.f, 0.f, 80.f), false, FLinearColor(0.9f, 0.35f, 0.1f));
	Objective(TEXT("CMD"), FVector(9200.f, 20.f, 80.f), false);
	Objective(TEXT("GHOST"), FVector(16000.f, 0.f, 80.f), true, FLinearColor(0.3f, 0.6f, 1.f));

	SpawnAI(FVector(-3600.f, 400.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(1200.f, -500.f, 100.f), EAshlineAIArchetype::MachineGunner);
	SpawnAI(FVector(9200.f, 400.f, 100.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(5200.f, 500.f, 100.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(-2000.f, -1200.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(14000.f, 200.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(7000.f, -900.f, 100.f), EAshlineAIArchetype::Heavy);
}

void AAshlineGrayboxBuilder::BuildAshHarbor()
{
	// 300m dock, crane yard, container lanes, 90m freighter.
	SpawnAtmosphere(FLinearColor(0.55f, 0.65f, 0.8f), 6.5f, FLinearColor(0.15f, 0.2f, 0.28f), 0.02f);
	Floor(FVector::ZeroVector, FVector2D(36000.f, 22000.f), FLinearColor(0.16f, 0.17f, 0.18f));
	Box(FVector(7000.f, 0.f, 6.f), FVector(80.f, 90.f, 0.08f), FLinearColor(0.08f, 0.14f, 0.22f));
	Box(FVector(-800.f, 0.f, 40.f), FVector(48.f, 28.f, 0.6f), FLinearColor(0.28f, 0.26f, 0.22f));
	Building(FVector(-3500.f, -1600.f, 0.f), FVector2D(360.f, 360.f), 640.f, FLinearColor(0.55f, 0.4f, 0.15f));
	Box(FVector(-2800.f, -1600.f, 620.f), FVector(16.f, 1.4f, 0.55f), FLinearColor(0.5f, 0.38f, 0.12f));
	Building(FVector(6200.f, 0.f, 40.f), FVector2D(2800.f, 720.f), 280.f, FLinearColor(0.2f, 0.22f, 0.25f));
	Building(FVector(8200.f, 0.f, 180.f), FVector2D(720.f, 640.f), 260.f, FLinearColor(0.18f, 0.2f, 0.24f));
	for (int32 i = 0; i < 8; ++i)
	{
		Box(FVector(-2000.f + i * 420.f, 1800.f, 90.f), FVector(3.4f, 1.6f, 1.8f), FLinearColor(0.22f, 0.18f, 0.1f));
	}
	CoverLine(FVector(-2000.f, 400.f, 40.f), FVector(2000.f, -300.f, 40.f), 8);
	Watchtower(FVector(-5000.f, 2200.f, 40.f), 700.f);

	PlayerStartAt(FVector(-7200.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("CRANE"), FVector(-3500.f, -1600.f, 120.f), false);
	Objective(TEXT("BOARD"), FVector(4800.f, 0.f, 180.f), false);
	Objective(TEXT("CARGO"), FVector(8200.f, 0.f, 220.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("HOSTAGE"), FVector(-800.f, 2600.f, 80.f), false);

	SpawnAI(FVector(-2400.f, 300.f, 120.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(400.f, -240.f, 120.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(5600.f, 280.f, 220.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(7400.f, -200.f, 220.f), EAshlineAIArchetype::Heavy);
	SpawnAI(FVector(-3600.f, -800.f, 120.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(200.f, 1600.f, 140.f), EAshlineAIArchetype::MachineGunner);
	SpawnAI(FVector(8000.f, 400.f, 240.f), EAshlineAIArchetype::Rifleman);
}

void AAshlineGrayboxBuilder::BuildWhiteout()
{
	// 400m wire run across a plateau, beacon mid, sled LZ east.
	SpawnAtmosphere(FLinearColor(0.8f, 0.85f, 0.95f), 4.f, FLinearColor(0.7f, 0.75f, 0.82f), 0.06f);
	Floor(FVector::ZeroVector, FVector2D(44000.f, 20000.f), FLinearColor(0.78f, 0.8f, 0.84f));

	for (int32 i = 0; i < 14; ++i)
	{
		Cylinder(FVector(-16000.f + i * 2200.f, FMath::Sin(i * 0.7f) * 280.f, 90.f), FVector(0.28f, 0.28f, 1.8f), FLinearColor(0.2f, 0.2f, 0.22f));
	}
	Cylinder(FVector(7000.f, 0.f, 180.f), FVector(1.6f, 1.6f, 3.6f), FLinearColor(0.35f, 0.4f, 0.55f));
	Sphere(FVector(7000.f, 0.f, 420.f), FVector(0.9f, 0.9f, 0.9f), FLinearColor(0.2f, 0.6f, 1.f));
	Building(FVector(7000.f, 1600.f, 0.f), FVector2D(900.f, 640.f), 200.f, FLinearColor(0.45f, 0.48f, 0.52f));
	Box(FVector(14000.f, 0.f, 30.f), FVector(14.f, 10.f, 0.4f), FLinearColor(0.55f, 0.3f, 0.15f));
	CoverLine(FVector(-8000.f, -400.f, 0.f), FVector(4000.f, 300.f, 0.f), 9);
	TreeBelt(FVector(-10000.f, 2800.f, 0.f), FVector(2000.f, 2600.f, 0.f), 8, 220.f);

	PlayerStartAt(FVector(-17000.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("WIRE"), FVector(-10000.f, 0.f, 80.f), false);
	Objective(TEXT("BEACON"), FVector(7000.f, 0.f, 80.f), false, FLinearColor(0.2f, 0.6f, 1.f));
	Objective(TEXT("CREW"), FVector(14000.f, 0.f, 80.f), true);
	Objective(TEXT("STORM"), FVector(7000.f, 1600.f, 80.f), false);

	SpawnAI(FVector(-2000.f, 400.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(3000.f, -360.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(7200.f, 500.f, 100.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(12000.f, 200.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(-6000.f, -200.f, 100.f), EAshlineAIArchetype::Scout);
}

void AAshlineGrayboxBuilder::BuildCatacomb()
{
	// 350m metro spine with three platforms, side ossuary, server farm east.
	SpawnAtmosphere(FLinearColor(0.35f, 0.3f, 0.25f), 1.6f, FLinearColor(0.05f, 0.04f, 0.03f), 0.05f);
	Floor(FVector::ZeroVector, FVector2D(40000.f, 14000.f), FLinearColor(0.12f, 0.1f, 0.08f));

	auto Tunnel = [this](float X0, float X1, float Y)
	{
		const float Mid = (X0 + X1) * 0.5f;
		const float Span = FMath::Abs(X1 - X0) / 100.f;
		Wall(FVector(Mid, Y - 280.f, 180.f), FVector(Span, 0.45f, 3.6f), FLinearColor(0.18f, 0.15f, 0.12f));
		Wall(FVector(Mid, Y + 280.f, 180.f), FVector(Span, 0.45f, 3.6f), FLinearColor(0.18f, 0.15f, 0.12f));
	};
	Tunnel(-14000.f, -6000.f, 0.f);
	Tunnel(-6000.f, 2000.f, 0.f);
	Tunnel(2000.f, 9000.f, 0.f);
	Tunnel(9000.f, 15000.f, 0.f);
	Box(FVector(-10000.f, 0.f, 20.f), FVector(14.f, 8.f, 0.3f), FLinearColor(0.2f, 0.18f, 0.14f));
	Box(FVector(-2000.f, 0.f, 20.f), FVector(14.f, 8.f, 0.3f), FLinearColor(0.2f, 0.18f, 0.14f));
	Box(FVector(5000.f, 0.f, 20.f), FVector(14.f, 8.f, 0.3f), FLinearColor(0.2f, 0.18f, 0.14f));
	WalledYard(FVector(-2000.f, 1800.f, 0.f), FVector2D(900.f, 700.f), 280.f, FLinearColor(0.16f, 0.14f, 0.12f), 2, 400.f);
	for (int32 i = 0; i < 8; ++i)
	{
		Box(FVector(11000.f, -500.f + i * 160.f, 120.f), FVector(1.6f, 1.1f, 2.4f), FLinearColor(0.1f, 0.12f, 0.16f));
	}
	Cover(FVector(-10000.f, 160.f, 0.f));
	Cover(FVector(-2000.f, -180.f, 0.f));
	Cover(FVector(5000.f, 140.f, 0.f));
	CoverLine(FVector(-12000.f, 0.f, 0.f), FVector(8000.f, 0.f, 0.f), 10);

	PlayerStartAt(FVector(-15000.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("PUSH"), FVector(-2000.f, 0.f, 80.f), false);
	Objective(TEXT("DUMP"), FVector(11000.f, 0.f, 80.f), false, FLinearColor(0.3f, 0.8f, 0.4f));
	Objective(TEXT("COLLAPSE"), FVector(14800.f, 0.f, 80.f), true);
	Objective(TEXT("INTEL"), FVector(11000.f, 1400.f, 80.f), false);

	SpawnAI(FVector(-9000.f, 120.f, 100.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(-2000.f, -120.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(5000.f, 120.f, 100.f), EAshlineAIArchetype::Heavy);
	SpawnAI(FVector(11000.f, -200.f, 100.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(-6000.f, 80.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(2000.f, -80.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(13000.f, 200.f, 100.f), EAshlineAIArchetype::MachineGunner);
}

void AAshlineGrayboxBuilder::BuildRidgeWire()
{
	// 280m stepped climb, three terraces, mast on the high spur.
	SpawnAtmosphere(FLinearColor(0.6f, 0.7f, 0.85f), 7.f, FLinearColor(0.3f, 0.35f, 0.4f), 0.02f);
	Floor(FVector::ZeroVector, FVector2D(32000.f, 18000.f), FLinearColor(0.2f, 0.22f, 0.16f));

	Box(FVector(-8000.f, 0.f, 50.f), FVector(36.f, 18.f, 1.0f), FLinearColor(0.24f, 0.26f, 0.18f));
	Box(FVector(-1500.f, 400.f, 180.f), FVector(30.f, 16.f, 1.0f), FLinearColor(0.26f, 0.28f, 0.2f));
	Box(FVector(6500.f, 0.f, 340.f), FVector(24.f, 16.f, 1.0f), FLinearColor(0.28f, 0.3f, 0.22f));
	Cylinder(FVector(6500.f, 0.f, 780.f), FVector(0.55f, 0.55f, 7.f), FLinearColor(0.6f, 0.6f, 0.55f), EAshlineSurface::Metal);
	Building(FVector(8200.f, 1800.f, 340.f), FVector2D(420.f, 420.f), 180.f, FLinearColor(0.3f, 0.15f, 0.1f));
	CoverLine(FVector(-6000.f, 200.f, 80.f), FVector(4000.f, 400.f, 280.f), 9);
	Watchtower(FVector(4000.f, -1400.f, 220.f), 560.f);
	TreeBelt(FVector(-9000.f, -2400.f, 40.f), FVector(2000.f, -1800.f, 180.f), 8, 300.f);

	PlayerStartAt(FVector(-11000.f, 0.f, 160.f), FRotator::ZeroRotator);
	Objective(TEXT("CLIMB"), FVector(6500.f, 0.f, 400.f), false);
	Objective(TEXT("PLANT"), FVector(6500.f, 160.f, 400.f), false, FLinearColor(0.4f, 0.8f, 1.f));
	Objective(TEXT("HOLD"), FVector(5200.f, 0.f, 400.f), true);
	Objective(TEXT("DRONE"), FVector(8200.f, 1800.f, 420.f), false);

	SpawnAI(FVector(-1500.f, 400.f, 260.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(5000.f, -360.f, 420.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(7000.f, 400.f, 420.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(2000.f, 200.f, 320.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(8000.f, 1200.f, 440.f), EAshlineAIArchetype::Officer);
}

void AAshlineGrayboxBuilder::BuildFalseFlag()
{
	// Broadcast campus: three wings, courtyard, loading dock east. ~220m.
	SpawnAtmosphere(FLinearColor(0.2f, 0.25f, 0.45f), 2.8f, FLinearColor(0.06f, 0.06f, 0.1f), 0.028f);
	Floor(FVector::ZeroVector, FVector2D(30000.f, 20000.f), FLinearColor(0.1f, 0.1f, 0.12f));

	WalledYard(FVector(0.f, 0.f, 0.f), FVector2D(3600.f, 2400.f), 380.f, FLinearColor(0.22f, 0.22f, 0.26f), 0, 700.f);
	Building(FVector(-1200.f, 0.f, 0.f), FVector2D(1400.f, 900.f), 320.f, FLinearColor(0.28f, 0.26f, 0.3f));
	Building(FVector(800.f, 1200.f, 0.f), FVector2D(1600.f, 700.f), 280.f, FLinearColor(0.26f, 0.24f, 0.28f));
	Building(FVector(800.f, -1100.f, 0.f), FVector2D(1200.f, 640.f), 240.f, FLinearColor(0.24f, 0.22f, 0.26f));
	Wall(FVector(-400.f, 0.f, 160.f), FVector(0.45f, 16.f, 3.0f), FLinearColor(0.28f, 0.26f, 0.3f));
	Box(FVector(800.f, 1200.f, 90.f), FVector(3.4f, 2.2f, 1.5f), FLinearColor(0.15f, 0.16f, 0.2f));
	WalledYard(FVector(7200.f, 0.f, 0.f), FVector2D(1400.f, 1600.f), 260.f, FLinearColor(0.18f, 0.16f, 0.12f), 0, 700.f);
	Box(FVector(7200.f, 0.f, 50.f), FVector(8.f, 10.f, 1.0f), FLinearColor(0.18f, 0.16f, 0.12f));
	CoverLine(FVector(-2400.f, -400.f, 0.f), FVector(2400.f, 200.f, 0.f), 8);
	WindowStrip(FVector(-1200.f, 0.f, 220.f), 5, 100.f, FLinearColor(0.4f, 0.7f, 1.f));
	Doorway(FVector(-3600.f, 0.f, 0.f));

	PlayerStartAt(FVector(-7200.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("INFIL"), FVector(-3600.f, 0.f, 80.f), false);
	Objective(TEXT("SWAP"), FVector(800.f, 1200.f, 80.f), false, FLinearColor(0.8f, 0.3f, 0.3f));
	Objective(TEXT("WALK"), FVector(7200.f, 0.f, 80.f), true, FLinearColor(0.3f, 0.6f, 1.f));
	Objective(TEXT("CLEAN"), FVector(800.f, -1600.f, 80.f), false);

	SpawnAI(FVector(-800.f, 400.f, 100.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(1400.f, -200.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(3600.f, 300.f, 100.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(-2000.f, -200.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(6800.f, 200.f, 100.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(800.f, 1600.f, 100.f), EAshlineAIArchetype::Scout);
}

void AAshlineGrayboxBuilder::BuildLastTrain()
{
	// 500m freight corridor: platform west, eight cars, engine at the river bridge.
	SpawnAtmosphere(FLinearColor(0.4f, 0.45f, 0.5f), 4.5f, FLinearColor(0.12f, 0.12f, 0.12f), 0.025f);
	Floor(FVector::ZeroVector, FVector2D(52000.f, 16000.f), FLinearColor(0.16f, 0.16f, 0.15f));

	Box(FVector(-10000.f, 0.f, 40.f), FVector(28.f, 16.f, 0.7f), FLinearColor(0.22f, 0.2f, 0.18f));
	RoadStrip(FVector(-8000.f, 0.f, 0.f), FVector(16000.f, 0.f, 0.f), 420.f);
	for (int32 i = 0; i < 8; ++i)
	{
		Box(FVector(-2000.f + i * 1800.f, 0.f, 140.f), FVector(14.f, 3.4f, 2.6f), FLinearColor(0.18f, 0.14f, 0.1f));
	}
	Building(FVector(14000.f, 0.f, 0.f), FVector2D(900.f, 480.f), 280.f, FLinearColor(0.12f, 0.12f, 0.14f));
	CoverLine(FVector(-11000.f, 400.f, 40.f), FVector(2000.f, 500.f, 40.f), 9);
	Cover(FVector(6000.f, -420.f, 40.f));
	Watchtower(FVector(-8000.f, 2200.f, 40.f), 640.f);
	Building(FVector(-10000.f, 1800.f, 0.f), FVector2D(720.f, 480.f), 220.f, FLinearColor(0.2f, 0.18f, 0.16f));

	PlayerStartAt(FVector(-14000.f, 0.f, 140.f), FRotator::ZeroRotator);
	Objective(TEXT("PLATFORM"), FVector(-10000.f, 0.f, 80.f), false);
	Objective(TEXT("RIDE"), FVector(1600.f, 0.f, 160.f), false);
	Objective(TEXT("ENGINE"), FVector(14000.f, 0.f, 160.f), true);
	Objective(TEXT("WITNESS"), FVector(7000.f, 0.f, 160.f), false);

	SpawnAI(FVector(-9600.f, 280.f, 140.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(-800.f, -220.f, 200.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(5200.f, 220.f, 200.f), EAshlineAIArchetype::Heavy);
	SpawnAI(FVector(12000.f, -180.f, 200.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(-4000.f, 200.f, 160.f), EAshlineAIArchetype::Scout);
	SpawnAI(FVector(9000.f, 160.f, 200.f), EAshlineAIArchetype::MachineGunner);
	SpawnAI(FVector(2000.f, -200.f, 200.f), EAshlineAIArchetype::Rifleman);
}

void AAshlineGrayboxBuilder::BuildAshlineFinale()
{
	// Buried command spine ~400m: gate, three node chambers, lift.
	SpawnAtmosphere(FLinearColor(0.55f, 0.15f, 0.12f), 3.5f, FLinearColor(0.12f, 0.03f, 0.02f), 0.04f);
	Floor(FVector::ZeroVector, FVector2D(44000.f, 18000.f), FLinearColor(0.08f, 0.07f, 0.07f));

	WalledYard(FVector(-9000.f, 0.f, 0.f), FVector2D(600.f, 2800.f), 420.f, FLinearColor(0.16f, 0.14f, 0.14f), 1, 700.f);
	Box(FVector(-8400.f, 0.f, 130.f), FVector(0.35f, 5.f, 2.6f), FLinearColor(0.4f, 0.1f, 0.08f), false);
	for (int32 i = 0; i < 3; ++i)
	{
		const float X = -2000.f + i * 5000.f;
		WalledYard(FVector(X, 0.f, 0.f), FVector2D(1600.f, 1400.f), 360.f, FLinearColor(0.2f, 0.12f, 0.12f), 0, 600.f);
		Cylinder(FVector(X + 200.f, 0.f, 180.f), FVector(2.0f, 2.0f, 3.4f), FLinearColor(0.35f, 0.08f, 0.08f));
		Cover(FVector(X - 400.f, 280.f, 0.f));
		Cover(FVector(X + 500.f, -240.f, 0.f));
	}
	Box(FVector(15000.f, 0.f, 40.f), FVector(7.f, 7.f, 0.55f), FLinearColor(0.25f, 0.22f, 0.1f));
	Box(FVector(15000.f, 0.f, 240.f), FVector(0.35f, 0.35f, 4.6f), FLinearColor(0.4f, 0.35f, 0.15f));
	CoverLine(FVector(-11000.f, 200.f, 0.f), FVector(12000.f, -160.f, 0.f), 12);
	Watchtower(FVector(-7000.f, 2200.f, 0.f), 800.f);

	PlayerStartAt(FVector(-14000.f, 0.f, 120.f), FRotator::ZeroRotator);
	Objective(TEXT("BREACH"), FVector(-8400.f, 0.f, 80.f), false, FLinearColor(0.9f, 0.2f, 0.15f));
	Objective(TEXT("CUT"), FVector(3000.f, 0.f, 80.f), false, FLinearColor(1.f, 0.3f, 0.2f));
	Objective(TEXT("COLLAPSE"), FVector(15000.f, 0.f, 80.f), true, FLinearColor(0.9f, 0.8f, 0.3f));
	Objective(TEXT("END"), FVector(8000.f, 0.f, 80.f), false);

	SpawnAI(FVector(-7000.f, 400.f, 100.f), EAshlineAIArchetype::Heavy);
	SpawnAI(FVector(-2000.f, -280.f, 100.f), EAshlineAIArchetype::Officer);
	SpawnAI(FVector(3000.f, 360.f, 100.f), EAshlineAIArchetype::MachineGunner);
	SpawnAI(FVector(8000.f, -240.f, 100.f), EAshlineAIArchetype::Marksman);
	SpawnAI(FVector(13000.f, 220.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(-4000.f, 200.f, 100.f), EAshlineAIArchetype::Breacher);
	SpawnAI(FVector(5000.f, 180.f, 100.f), EAshlineAIArchetype::Rifleman);
	SpawnAI(FVector(11000.f, -160.f, 100.f), EAshlineAIArchetype::Scout);
}
