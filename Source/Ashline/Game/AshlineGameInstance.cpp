#include "Game/AshlineGameInstance.h"

#include "Ashline.h"
#include "Campaign/AshlineMissionCatalog.h"
#include "Game/AshlineGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "Progression/AshlineProgressionSubsystem.h"
#include "Settings/AshlineGraphicsSettings.h"

void UAshlineGameInstance::Init()
{
	Super::Init();

	if (UAshlineProgressionSubsystem* Progression = GetSubsystem<UAshlineProgressionSubsystem>())
	{
		Progression->LoadOrCreate();
	}

	if (UAshlineGraphicsSettings* Graphics = GetSubsystem<UAshlineGraphicsSettings>())
	{
		Graphics->ApplySavedOrDetect();
	}

	UE_LOG(LogAshline, Log, TEXT("Ashline game instance ready (Windows-first AAA presentation, UE 5.8.2)."));
}

void UAshlineGameInstance::OpenFrontend()
{
	static const TCHAR* FrontendMap = TEXT("/Game/Ashline/Maps/Frontend/ASH_Frontend");
	if (FPackageName::DoesPackageExist(FrontendMap))
	{
		UGameplayStatics::OpenLevel(this, FName(FrontendMap));
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (AAshlineGameMode* GameMode = World->GetAuthGameMode<AAshlineGameMode>())
		{
			GameMode->ReturnToFrontend();
			return;
		}
	}

	UE_LOG(LogAshline, Log, TEXT("No authored frontend map — stay on the runtime campaign host and press Play."));
}

void UAshlineGameInstance::TravelToMission(FName MapAsset)
{
	if (MapAsset.IsNone())
	{
		return;
	}

	if (FPackageName::DoesPackageExist(MapAsset.ToString()))
	{
		UGameplayStatics::OpenLevel(this, MapAsset);
		return;
	}

	for (const FAshlineMissionDefinition& Mission : UAshlineMissionCatalog::BuildCampaign())
	{
		if (Mission.MapAsset == MapAsset)
		{
			if (UWorld* World = GetWorld())
			{
				if (AAshlineGameMode* GameMode = World->GetAuthGameMode<AAshlineGameMode>())
				{
					GameMode->DeployMission(Mission.MissionId);
					return;
				}
			}
		}
	}

	UE_LOG(LogAshline, Warning, TEXT("TravelToMission: no authored map and no catalog match for %s"), *MapAsset.ToString());
}

void UAshlineGameInstance::StartMissionById(EAshlineMissionId MissionId)
{
	if (UWorld* World = GetWorld())
	{
		if (AAshlineGameMode* GameMode = World->GetAuthGameMode<AAshlineGameMode>())
		{
			GameMode->DeployMission(MissionId);
		}
	}
}
