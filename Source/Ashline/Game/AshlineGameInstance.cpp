#include "Game/AshlineGameInstance.h"

#include "Ashline.h"
#include "Kismet/GameplayStatics.h"
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

	UE_LOG(LogAshline, Log, TEXT("Ashline game instance ready (Apple-first FPS/TPS campaign)."));
}

void UAshlineGameInstance::OpenFrontend()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("/Game/Ashline/Maps/Frontend/ASH_Frontend")));
}

void UAshlineGameInstance::TravelToMission(FName MapAsset)
{
	if (MapAsset.IsNone())
	{
		return;
	}
	UGameplayStatics::OpenLevel(this, MapAsset);
}
