#include "Game/AshlineGameMode.h"

#include "Ashline.h"
#include "AI/AshlineAICatalog.h"
#include "Camera/CameraActor.h"
#include "Campaign/AshlineMissionCatalog.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AshlineCharacter.h"
#include "Player/AshlinePlayerController.h"
#include "Progression/AshlineProgressionSubsystem.h"
#include "UI/AshlineHUD.h"
#include "Presentation/AshlineAudioDirector.h"
#include "World/AshlineGrayboxBuilder.h"

AAshlineGameMode::AAshlineGameMode()
{
	DefaultPawnClass = AAshlineCharacter::StaticClass();
	PlayerControllerClass = AAshlinePlayerController::StaticClass();
	HUDClass = AAshlineHUD::StaticClass();
	SpectatorClass = ASpectatorPawn::StaticClass();
	bStartPlayersAsSpectators = true;
}

void AAshlineGameMode::StartPlay()
{
	EnsureBuilder();
	if (Builder)
	{
		Builder->BuildFrontendHub();
	}
	Super::StartPlay();
	ReturnToFrontend();
}

void AAshlineGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AAshlineGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (PlayPhase == EAshlinePlayPhase::Frontend)
	{
		if (NewPlayer)
		{
			NewPlayer->StartSpectatingOnly();
			ApplyFrontendView(NewPlayer);
		}
	}
}

AAshlineGrayboxBuilder* AAshlineGameMode::EnsureBuilder()
{
	if (IsValid(Builder))
	{
		return Builder;
	}

	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters Params;
		Params.Name = TEXT("AshlineGrayboxBuilder");
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Builder = World->SpawnActor<AAshlineGrayboxBuilder>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
	}
	return Builder;
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

void AAshlineGameMode::DeployMission(EAshlineMissionId MissionId)
{
	if (MissionId == EAshlineMissionId::None || !IsMissionDeployable(MissionId))
	{
		UE_LOG(LogAshline, Warning, TEXT("Mission is locked."));
		return;
	}

	EnsureBuilder();
	if (Builder)
	{
		Builder->BuildMission(MissionId);
	}

	StartMission(MissionId);
	PlayPhase = EAshlinePlayPhase::InMission;
	bStartPlayersAsSpectators = false;

	ForEachLocalController([this](APlayerController* PC)
	{
		ApplyMissionView(PC);
		RestartPlayer(PC);
	});

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
		{
			Audio->StartMusicBed(this, MissionId);
		}
	}
}

void AAshlineGameMode::ReturnToFrontend()
{
	PlayPhase = EAshlinePlayPhase::Frontend;
	bStartPlayersAsSpectators = true;
	SelectDefaultMission();

	EnsureBuilder();
	if (Builder)
	{
		Builder->BuildFrontendHub();
	}

	ForEachLocalController([this](APlayerController* PC)
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			PC->UnPossess();
			Pawn->Destroy();
		}
		PC->StartSpectatingOnly();
		ApplyFrontendView(PC);
	});

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
		{
			Audio->StopMusicBed();
			Audio->StartMusicBed(this, EAshlineMissionId::None);
		}
	}
}

void AAshlineGameMode::CompleteActiveMission(int32 Stars, bool bOptionalComplete)
{
	if (PlayPhase != EAshlinePlayPhase::InMission && PlayPhase != EAshlinePlayPhase::Paused)
	{
		return;
	}

	LastAwardedStars = FMath::Clamp(Stars, 0, 3);
	LastAwardedXP = 0;
	LastAwardedCrates = 0;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				const int32 XPBefore = Save->Operator.XP;
				const int32 RankBefore = Save->Operator.Rank;
				const int32 TokensBefore = Save->CrateTokens;
				Progression->CompleteMission(ActiveMission, LastAwardedStars, bOptionalComplete);
				LastAwardedXP = (Save->Operator.Rank - RankBefore) * 1000 + (Save->Operator.XP - XPBefore);
				LastAwardedCrates = Save->CrateTokens - TokensBefore;
			}
			else
			{
				Progression->CompleteMission(ActiveMission, LastAwardedStars, bOptionalComplete);
				LastAwardedXP = ActiveDefinition.XPReward + LastAwardedStars * 50;
				LastAwardedCrates = ActiveDefinition.CrateTokensReward;
			}
		}
	}

	const EAshlineMissionId Next = UAshlineMissionCatalog::NextMission(ActiveMission);
	PlayPhase = (Next == EAshlineMissionId::None) ? EAshlinePlayPhase::CampaignComplete : EAshlinePlayPhase::MissionComplete;
	UE_LOG(LogAshline, Log, TEXT("Mission %s complete. Stars=%d XP~%d crates=%d"),
		*ActiveDefinition.CodeName, LastAwardedStars, LastAwardedXP, LastAwardedCrates);
}

void AAshlineGameMode::SetObjectiveComplete(FName ObjectiveId, bool bComplete)
{
	for (FAshlineMissionObjective& Objective : ActiveDefinition.Objectives)
	{
		if (Objective.ObjectiveId == ObjectiveId)
		{
			Objective.bCompleted = bComplete;
			UE_LOG(LogAshline, Log, TEXT("Objective %s = %s"), *ObjectiveId.ToString(), bComplete ? TEXT("done") : TEXT("open"));
			break;
		}
	}
}

bool AAshlineGameMode::AreRequiredObjectivesComplete() const
{
	for (const FAshlineMissionObjective& Objective : ActiveDefinition.Objectives)
	{
		if (!Objective.bOptional && !Objective.bCompleted)
		{
			return false;
		}
	}
	return ActiveDefinition.Objectives.Num() > 0;
}

void AAshlineGameMode::MenuMove(int32 Delta)
{
	if (PlayPhase != EAshlinePlayPhase::Frontend)
	{
		return;
	}

	const TArray<FAshlineMissionDefinition> Campaign = UAshlineMissionCatalog::BuildCampaign();
	if (Campaign.Num() == 0)
	{
		return;
	}
	SelectedMissionIndex = (SelectedMissionIndex + Delta + Campaign.Num()) % Campaign.Num();
}

void AAshlineGameMode::MenuConfirm()
{
	if (PlayPhase == EAshlinePlayPhase::Frontend)
	{
		const TArray<FAshlineMissionDefinition> Campaign = UAshlineMissionCatalog::BuildCampaign();
		if (Campaign.IsValidIndex(SelectedMissionIndex))
		{
			DeployMission(Campaign[SelectedMissionIndex].MissionId);
		}
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::Paused)
	{
		PlayPhase = EAshlinePlayPhase::InMission;
		ForEachLocalController([](APlayerController* PC)
		{
			PC->SetPause(false);
		});
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::MissionComplete || PlayPhase == EAshlinePlayPhase::CampaignComplete)
	{
		ReturnToFrontend();
	}
}

void AAshlineGameMode::MenuBack()
{
	if (PlayPhase == EAshlinePlayPhase::InMission)
	{
		PlayPhase = EAshlinePlayPhase::Paused;
		ForEachLocalController([](APlayerController* PC)
		{
			PC->SetPause(true);
		});
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::Paused)
	{
		ForEachLocalController([](APlayerController* PC)
		{
			PC->SetPause(false);
		});
		ReturnToFrontend();
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::MissionComplete || PlayPhase == EAshlinePlayPhase::CampaignComplete)
	{
		ReturnToFrontend();
	}
}

void AAshlineGameMode::MenuCycleDifficulty(int32 Delta)
{
	if (PlayPhase != EAshlinePlayPhase::Frontend)
	{
		return;
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				const int32 Current = static_cast<int32>(Save->Difficulty);
				const int32 Next = (Current + Delta + 4) % 4;
				Progression->SetDifficulty(static_cast<EAshlineDifficulty>(Next));
			}
		}
	}
}

void AAshlineGameMode::RespawnPlayer(AAshlineCharacter* Character)
{
	if (!Character)
	{
		return;
	}

	FVector Loc = FVector(-1800.f, 0.f, 140.f);
	FRotator Rot = FRotator::ZeroRotator;
	if (Builder)
	{
		Loc = Builder->LastPlayerStartLocation;
		Rot = Builder->LastPlayerStartRotation;
	}
	else if (AActor* Start = FindPlayerStart(Character->GetController()))
	{
		Loc = Start->GetActorLocation();
		Rot = Start->GetActorRotation();
	}

	Character->SetActorLocationAndRotation(Loc, Rot, false, nullptr, ETeleportType::TeleportPhysics);
	if (AController* Controller = Character->GetController())
	{
		Controller->SetControlRotation(Rot);
	}
}

void AAshlineGameMode::UnlockAllMissions()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				for (FAshlineMissionProgress& Progress : Save->Missions)
				{
					if (Progress.State == EAshlineMissionState::Locked)
					{
						Progress.State = EAshlineMissionState::Available;
					}
				}
				Progression->SaveCampaign();
			}
		}
	}
}

void AAshlineGameMode::ApplyFrontendView(APlayerController* PC)
{
	if (!PC)
	{
		return;
	}
	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
	if (Builder && Builder->FrontendCamera)
	{
		PC->SetViewTarget(static_cast<AActor*>(Builder->FrontendCamera.Get()));
	}
}

void AAshlineGameMode::ApplyMissionView(APlayerController* PC)
{
	if (!PC)
	{
		return;
	}
	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
}

void AAshlineGameMode::SelectDefaultMission()
{
	SelectedMissionIndex = 0;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				for (int32 i = 0; i < Save->Missions.Num(); ++i)
				{
					if (Save->Missions[i].State == EAshlineMissionState::Available
						|| Save->Missions[i].State == EAshlineMissionState::InProgress)
					{
						SelectedMissionIndex = i;
						return;
					}
				}
				for (int32 i = Save->Missions.Num() - 1; i >= 0; --i)
				{
					if (Save->Missions[i].State == EAshlineMissionState::Completed)
					{
						SelectedMissionIndex = i;
						return;
					}
				}
			}
		}
	}
}

int32 AAshlineGameMode::ComputeStars() const
{
	bool bRequired = true;
	bool bOptional = true;
	for (const FAshlineMissionObjective& Objective : ActiveDefinition.Objectives)
	{
		if (!Objective.bOptional && !Objective.bCompleted)
		{
			bRequired = false;
		}
		if (Objective.bOptional && !Objective.bCompleted)
		{
			bOptional = false;
		}
	}
	if (bRequired && bOptional)
	{
		return 3;
	}
	if (bRequired)
	{
		return 2;
	}
	return 1;
}

bool AAshlineGameMode::IsMissionDeployable(EAshlineMissionId MissionId) const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				if (const FAshlineMissionProgress* Progress = Save->FindMissionProgress(MissionId))
				{
					return Progress->State != EAshlineMissionState::Locked;
				}
			}
		}
	}
	return MissionId == EAshlineMissionId::ASH_01_WireCut;
}

void AAshlineGameMode::ForEachLocalController(TFunctionRef<void(APlayerController*)> Fn)
{
	if (UWorld* World = GetWorld())
	{
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				Fn(PC);
			}
		}
	}
}
