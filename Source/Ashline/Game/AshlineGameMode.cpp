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
#include "UI/AshlineCombatFeedback.h"
#include "Presentation/AshlineAudioDirector.h"
#include "Settings/AshlineGameUserSettings.h"
#include "Settings/AshlineGraphicsSettings.h"
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
	MenuLayer = EAshlineMenuLayer::None;
	bStartPlayersAsSpectators = false;
	MissionKills = 0;

	if (UWorld* World = GetWorld())
	{
		if (UAshlineCombatFeedback* Feedback = World->GetSubsystem<UAshlineCombatFeedback>())
		{
			Feedback->ResetMission();
			if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
			{
				if (User->Feel.bSubtitles)
				{
					Feedback->SetSubtitle(ActiveDefinition.Briefing.ToString(), 8.f);
				}
			}
		}
	}

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
	MenuLayer = EAshlineMenuLayer::None;
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
	LastAwardedCredits = 0;
	if (UWorld* World = GetWorld())
	{
		if (UAshlineCombatFeedback* Feedback = World->GetSubsystem<UAshlineCombatFeedback>())
		{
			MissionKills = Feedback->MissionKills;
		}
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				const int32 TokensBefore = Save->CrateTokens;
				const int32 CreditsBefore = Save->Credits;
				Progression->CompleteMission(ActiveMission, LastAwardedStars, bOptionalComplete);
				LastAwardedXP = ActiveDefinition.XPReward + LastAwardedStars * 50;
				LastAwardedCrates = Save->CrateTokens - TokensBefore;
				LastAwardedCredits = Save->Credits - CreditsBefore;
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
			if (UWorld* World = GetWorld())
			{
				if (UAshlineCombatFeedback* Feedback = World->GetSubsystem<UAshlineCombatFeedback>())
				{
					Feedback->NotifyObjective(FString::Printf(TEXT("%s  %s"),
						bComplete ? TEXT("OBJ COMPLETE") : TEXT("OBJ"),
						*Objective.DisplayName.ToString()));
				}
			}
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
	if (MenuLayer == EAshlineMenuLayer::Settings)
	{
		SettingsCursor = (SettingsCursor + Delta + 12) % 12;
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::Paused && MenuLayer == EAshlineMenuLayer::PauseRoot)
	{
		MenuCursor = (MenuCursor + Delta + 3) % 3;
		return;
	}

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
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
		{
			Audio->PlayUI(this, true);
		}
	}

	if (MenuLayer == EAshlineMenuLayer::Settings)
	{
		if (SettingsCursor == 11)
		{
			MenuLayer = (PlayPhase == EAshlinePlayPhase::Paused) ? EAshlineMenuLayer::PauseRoot : EAshlineMenuLayer::None;
		}
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::Frontend)
	{
		const TArray<FAshlineMissionDefinition> Campaign = UAshlineMissionCatalog::BuildCampaign();
		if (Campaign.IsValidIndex(SelectedMissionIndex))
		{
			EnterBriefing(Campaign[SelectedMissionIndex].MissionId);
		}
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::Briefing)
	{
		DeployMission(ActiveMission);
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::Paused)
	{
		if (MenuCursor == 0)
		{
			PlayPhase = EAshlinePlayPhase::InMission;
			MenuLayer = EAshlineMenuLayer::None;
			ForEachLocalController([](APlayerController* PC)
			{
				PC->SetPause(false);
			});
		}
		else if (MenuCursor == 1)
		{
			MenuLayer = EAshlineMenuLayer::Settings;
			SettingsCursor = 0;
		}
		else
		{
			ForEachLocalController([](APlayerController* PC)
			{
				PC->SetPause(false);
			});
			ReturnToFrontend();
		}
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::MissionComplete || PlayPhase == EAshlinePlayPhase::CampaignComplete)
	{
		ReturnToFrontend();
	}
}

void AAshlineGameMode::MenuBack()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
		{
			Audio->PlayUI(this, false);
		}
	}

	if (MenuLayer == EAshlineMenuLayer::Settings)
	{
		MenuLayer = (PlayPhase == EAshlinePlayPhase::Paused) ? EAshlineMenuLayer::PauseRoot : EAshlineMenuLayer::None;
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::Frontend)
	{
		MenuLayer = EAshlineMenuLayer::Settings;
		SettingsCursor = 0;
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::Briefing)
	{
		ReturnToFrontend();
		return;
	}

	if (PlayPhase == EAshlinePlayPhase::InMission)
	{
		PlayPhase = EAshlinePlayPhase::Paused;
		MenuLayer = EAshlineMenuLayer::PauseRoot;
		MenuCursor = 0;
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
	if (MenuLayer == EAshlineMenuLayer::Settings)
	{
		AdjustSetting(Delta);
		return;
	}

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
	Character->RestoreAfterRespawn();
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

void AAshlineGameMode::EnterBriefing(EAshlineMissionId MissionId)
{
	if (MissionId == EAshlineMissionId::None || !IsMissionDeployable(MissionId))
	{
		UE_LOG(LogAshline, Warning, TEXT("Mission is locked."));
		return;
	}

	ActiveMission = MissionId;
	if (!UAshlineMissionCatalog::FindMission(MissionId, ActiveDefinition))
	{
		return;
	}
	PlayPhase = EAshlinePlayPhase::Briefing;
	MenuLayer = EAshlineMenuLayer::None;
}

void AAshlineGameMode::CycleGraphicsPreset(int32 Delta)
{
	static const EAshlineGraphicsPreset Order[] = {
		EAshlineGraphicsPreset::SteamDeck,
		EAshlineGraphicsPreset::Low,
		EAshlineGraphicsPreset::Medium,
		EAshlineGraphicsPreset::High,
		EAshlineGraphicsPreset::Epic,
		EAshlineGraphicsPreset::PC_Balanced,
		EAshlineGraphicsPreset::PC_Ultra
	};
	constexpr int32 Count = 7;
	int32 Index = 6;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineGraphicsSettings* Graphics = GI->GetSubsystem<UAshlineGraphicsSettings>())
		{
			const EAshlineGraphicsPreset Current = Graphics->GetState().Preset;
			for (int32 i = 0; i < Count; ++i)
			{
				if (Order[i] == Current)
				{
					Index = i;
					break;
				}
			}
			Index = (Index + Delta + Count) % Count;
			Graphics->ApplyPreset(Order[Index]);
			if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
			{
				User->NamedPreset = Order[Index];
				if (Order[Index] == EAshlineGraphicsPreset::SteamDeck)
				{
					User->Feel.bForceHandheldHUD = true;
					User->Feel.HUDScale = FMath::Max(User->Feel.HUDScale, 1.2f);
					User->Feel.SafeZone = FMath::Max(User->Feel.SafeZone, 0.08f);
				}
				User->ApplySettings(false);
			}
		}
	}
}

void AAshlineGameMode::AdjustSetting(int32 Delta)
{
	UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings();
	if (!User)
	{
		return;
	}

	FAshlineFeelSettings& Feel = User->Feel;
	switch (SettingsCursor)
	{
	case 0:
		CycleGraphicsPreset(Delta);
		break;
	case 1:
		Feel.HUDScale = FMath::Clamp(Feel.HUDScale + Delta * 0.1f, 0.85f, 1.5f);
		break;
	case 2:
		Feel.SafeZone = FMath::Clamp(Feel.SafeZone + Delta * 0.01f, 0.03f, 0.14f);
		break;
	case 3:
		{
			const int32 Next = (static_cast<int32>(Feel.ColorBlind) + Delta + 5) % 5;
			Feel.ColorBlind = static_cast<EAshlineColorBlindMode>(Next);
		}
		break;
	case 4:
		Feel.bCameraShake = !Feel.bCameraShake;
		break;
	case 5:
		Feel.bHitMarkers = !Feel.bHitMarkers;
		break;
	case 6:
		Feel.bDamageVignette = !Feel.bDamageVignette;
		break;
	case 7:
		Feel.HipFOV = FMath::Clamp(Feel.HipFOV + Delta * 2.f, 75.f, 110.f);
		break;
	case 8:
		Feel.MasterVolume = FMath::Clamp(Feel.MasterVolume + Delta * 0.1f, 0.f, 1.f);
		User->ApplyFeelToAudio();
		break;
	case 9:
		Feel.MusicVolume = FMath::Clamp(Feel.MusicVolume + Delta * 0.1f, 0.f, 1.f);
		User->ApplyFeelToAudio();
		break;
	case 10:
		Feel.bForceHandheldHUD = !Feel.bForceHandheldHUD;
		break;
	default:
		break;
	}
	User->ApplySettings(false);
}
