#include "Game/AshlineGameMode.h"

#include "Ashline.h"
#include "AI/AshlineAICatalog.h"
#include "Campaign/AshlineMissionCatalog.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AshlineCharacter.h"
#include "Player/AshlinePlayerController.h"
#include "Progression/AshlineProgressionSubsystem.h"
#include "UI/AshlineHUD.h"

AAshlineGameMode::AAshlineGameMode()
{
	DefaultPawnClass = AAshlineCharacter::StaticClass();
	PlayerControllerClass = AAshlinePlayerController::StaticClass();
	HUDClass = AAshlineHUD::StaticClass();
}

void AAshlineGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				for (const FAshlineMissionProgress& Progress : Save->Missions)
				{
					if (Progress.State == EAshlineMissionState::InProgress || Progress.State == EAshlineMissionState::Available)
					{
						ActiveMission = Progress.MissionId;
						break;
					}
				}
			}
		}
	}

	StartMission(ActiveMission);
}

void AAshlineGameMode::StartMission(EAshlineMissionId MissionId)
{
	ActiveMission = MissionId;
	if (!UAshlineMissionCatalog::FindMission(MissionId, ActiveDefinition))
	{
		UE_LOG(LogAshline, Warning, TEXT("Unknown mission id."));
		return;
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				if (FAshlineMissionProgress* Progress = Save->FindMissionProgress(MissionId))
				{
					Progress->State = EAshlineMissionState::InProgress;
					Progression->SaveCampaign();
				}

				const FAshlineDifficultyTuning Tuning = UAshlineAICatalog::GetDifficulty(Save->Difficulty);
				UE_LOG(LogAshline, Log, TEXT("Mission %s live. Difficulty damage taken x%.2f AI acc x%.2f"),
					*ActiveDefinition.CodeName, Tuning.PlayerDamageTakenMul, Tuning.AIAccuracyMul);
			}
		}
	}

	UE_LOG(LogAshline, Log, TEXT("Briefing: %s"), *ActiveDefinition.Briefing.ToString());
}

void AAshlineGameMode::CompleteActiveMission(int32 Stars, bool bOptionalComplete)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			Progression->CompleteMission(ActiveMission, Stars, bOptionalComplete);
		}
	}
}

void AAshlineGameMode::SetObjectiveComplete(FName ObjectiveId, bool bComplete)
{
	for (FAshlineMissionObjective& Objective : ActiveDefinition.Objectives)
	{
		if (Objective.ObjectiveId == ObjectiveId)
		{
			Objective.bCompleted = bComplete;
			return;
		}
	}
}
