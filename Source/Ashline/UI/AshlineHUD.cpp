#include "UI/AshlineHUD.h"

#include "Campaign/AshlineMissionCatalog.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Game/AshlineGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AshlineCharacter.h"
#include "Progression/AshlineProgressionSubsystem.h"
#include "Settings/AshlineGraphicsSettings.h"
#include "Weapons/AshlineWeaponComponent.h"

namespace
{
	const TCHAR* DifficultyName(EAshlineDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EAshlineDifficulty::Recruit: return TEXT("Recruit");
		case EAshlineDifficulty::Regular: return TEXT("Regular");
		case EAshlineDifficulty::Veteran: return TEXT("Veteran");
		case EAshlineDifficulty::Extreme: return TEXT("Extreme");
		default: return TEXT("Regular");
		}
	}

	const TCHAR* MissionStateName(EAshlineMissionState State)
	{
		switch (State)
		{
		case EAshlineMissionState::Locked: return TEXT("LOCKED");
		case EAshlineMissionState::Available: return TEXT("READY");
		case EAshlineMissionState::InProgress: return TEXT("ACTIVE");
		case EAshlineMissionState::Completed: return TEXT("DONE");
		default: return TEXT("?");
		}
	}
}

void AAshlineHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas || !GEngine)
	{
		return;
	}

	AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	UAshlineSaveGame* Save = nullptr;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			Save = Progression->GetSave();
		}
	}

	if (GameMode->PlayPhase == EAshlinePlayPhase::Frontend)
	{
		DrawFrontend(GameMode, Save);
		return;
	}

	if (GameMode->PlayPhase == EAshlinePlayPhase::MissionComplete || GameMode->PlayPhase == EAshlinePlayPhase::CampaignComplete)
	{
		DrawMissionComplete(GameMode);
		return;
	}

	if (GameMode->PlayPhase == EAshlinePlayPhase::Paused)
	{
		DrawPause();
	}

	if (bDrawDebugCombatHUD)
	{
		DrawCombat(GameMode);
	}
}

void AAshlineHUD::DrawFrontend(AAshlineGameMode* GameMode, UAshlineSaveGame* Save)
{
	DrawTextLine(TEXT("ASHLINE"), 48.f, 36.f, FLinearColor(0.95f, 0.86f, 0.55f));
	DrawTextLine(TEXT("Campaign select  —  Windows-first AAA blockout"), 48.f, 64.f, FLinearColor(0.75f, 0.75f, 0.7f));

	const TArray<FAshlineMissionDefinition> Campaign = UAshlineMissionCatalog::BuildCampaign();
	for (int32 i = 0; i < Campaign.Num(); ++i)
	{
		EAshlineMissionState State = EAshlineMissionState::Locked;
		if (Save)
		{
			if (const FAshlineMissionProgress* Progress = Save->FindMissionProgress(Campaign[i].MissionId))
			{
				State = Progress->State;
			}
		}
		else if (i == 0)
		{
			State = EAshlineMissionState::Available;
		}

		const bool bSelected = (i == GameMode->SelectedMissionIndex);
		const FString Line = FString::Printf(TEXT("%s %s  %s  [%s]"),
			bSelected ? TEXT(">") : TEXT(" "),
			*Campaign[i].CodeName,
			*Campaign[i].Title.ToString(),
			MissionStateName(State));
		const FLinearColor Color = bSelected
			? FLinearColor(1.f, 0.92f, 0.45f)
			: (State == EAshlineMissionState::Locked ? FLinearColor(0.35f, 0.35f, 0.35f) : FLinearColor::White);
		DrawTextLine(Line, 56.f, 110.f + i * 22.f, Color);
	}

	const EAshlineDifficulty Difficulty = Save ? Save->Difficulty : EAshlineDifficulty::Regular;
	const int32 Rank = Save ? Save->Operator.Rank : 1;
	const int32 XP = Save ? Save->Operator.XP : 0;
	const int32 Tokens = Save ? Save->CrateTokens : 0;
	DrawTextLine(FString::Printf(TEXT("Difficulty: < %s >     Rank %d  XP %d  Crates %d"), DifficultyName(Difficulty), Rank, XP, Tokens),
		48.f, Canvas->SizeY - 92.f, FLinearColor(0.8f, 0.85f, 0.7f));
	DrawTextLine(TEXT("Up/Down select   Enter deploy   Left/Right difficulty   Esc unused here"), 48.f, Canvas->SizeY - 68.f, FLinearColor(0.65f, 0.65f, 0.6f));
	FString PresetLine = TEXT("Graphics: Ashline_PC_Ultra (console: AshPCUltra / AshPCBalanced)");
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineGraphicsSettings* Graphics = GI->GetSubsystem<UAshlineGraphicsSettings>())
		{
			const FAshlineGraphicsState State = Graphics->GetState();
			PresetLine = FString::Printf(TEXT("Graphics: %s   RHI %s   RT %s   FSR3 %s"),
				*UAshlineGraphicsSettings::GetPresetDisplayName(State.Preset),
				*State.RHIName,
				State.bRayTracingEnabled ? TEXT("ON") : TEXT("off"),
				State.bFSR3Available ? TEXT("ready") : TEXT("TSR fallback"));
		}
	}
	DrawTextLine(PresetLine, 48.f, Canvas->SizeY - 44.f, FLinearColor(0.5f, 0.62f, 0.55f));
	DrawTextLine(TEXT("Console: AshUnlockAll  |  AshDeploy 3  |  AshComplete  |  AshFrontend  |  AshPCUltra"), 48.f, Canvas->SizeY - 24.f, FLinearColor(0.45f, 0.5f, 0.45f));
}

void AAshlineHUD::DrawCombat(AAshlineGameMode* GameMode)
{
	AAshlineCharacter* Character = Cast<AAshlineCharacter>(GetOwningPawn());
	if (Character && Character->WeaponComponent)
	{
		const FAshlineRuntimeWeapon& Weapon = Character->WeaponComponent->GetActiveWeapon();
		const FString WeaponLine = FString::Printf(
			TEXT("%s  %d / %d"),
			*Weapon.Definition.DisplayName.ToString(),
			Weapon.AmmoInMag,
			Weapon.Reserve);
		DrawTextLine(WeaponLine, 48.f, Canvas->SizeY - 84.f, FLinearColor::White);

		const FString Mode = Character->GetCameraMode() == EAshlineCameraMode::FirstPerson ? TEXT("FPS") : TEXT("TPS");
		DrawTextLine(FString::Printf(TEXT("%s   HP %.0f"), *Mode, Character->Health), 48.f, Canvas->SizeY - 60.f, FLinearColor(0.8f, 0.85f, 0.7f));
	}

	DrawTextLine(GameMode->ActiveDefinition.CodeName + TEXT("  ") + GameMode->ActiveDefinition.Title.ToString(),
		48.f, 36.f, FLinearColor(0.95f, 0.86f, 0.55f));

	float Y = 64.f;
	for (const FAshlineMissionObjective& Objective : GameMode->ActiveDefinition.Objectives)
	{
		const FString Mark = Objective.bCompleted ? TEXT("[x]") : TEXT("[ ]");
		const FString Line = FString::Printf(TEXT("%s %s%s"),
			*Mark,
			*Objective.DisplayName.ToString(),
			Objective.bOptional ? TEXT(" (optional)") : TEXT(""));
		DrawTextLine(Line, 48.f, Y, Objective.bCompleted ? FLinearColor(0.45f, 0.8f, 0.45f) : FLinearColor(0.85f, 0.85f, 0.8f));
		Y += 20.f;
	}

	DrawTextLine(TEXT("WASD move  Mouse look  LMB fire  RMB aim  R reload  Space jump  C crouch  V FPS/TPS  Q swap  Esc pause"),
		48.f, Canvas->SizeY - 36.f, FLinearColor(0.5f, 0.5f, 0.48f));

	const float CX = Canvas->SizeX * 0.5f;
	const float CY = Canvas->SizeY * 0.5f;
	DrawLine(CX - 8.f, CY, CX + 8.f, CY, FLinearColor::White, 1.f);
	DrawLine(CX, CY - 8.f, CX, CY + 8.f, FLinearColor::White, 1.f);
}

void AAshlineHUD::DrawMissionComplete(AAshlineGameMode* GameMode)
{
	const bool bFinale = GameMode->PlayPhase == EAshlinePlayPhase::CampaignComplete;
	DrawTextLine(bFinale ? TEXT("ASHLINE CUT") : TEXT("MISSION COMPLETE"), 48.f, 80.f, FLinearColor(0.95f, 0.86f, 0.55f));
	DrawTextLine(GameMode->ActiveDefinition.CodeName + TEXT("  ") + GameMode->ActiveDefinition.Title.ToString(),
		48.f, 120.f, FLinearColor::White);
	DrawTextLine(FString::Printf(TEXT("Stars %d   XP +%d   Crate tokens +%d"),
		GameMode->LastAwardedStars, GameMode->LastAwardedXP, GameMode->LastAwardedCrates),
		48.f, 160.f, FLinearColor(0.8f, 0.85f, 0.7f));
	DrawTextLine(TEXT("Progress saved to slot AshlineCampaign."), 48.f, 196.f, FLinearColor(0.7f, 0.7f, 0.65f));
	DrawTextLine(TEXT("Enter  —  return to campaign select"), 48.f, 240.f, FLinearColor(0.85f, 0.8f, 0.5f));
}

void AAshlineHUD::DrawPause()
{
	DrawTextLine(TEXT("PAUSED"), 48.f, 80.f, FLinearColor(0.95f, 0.86f, 0.55f));
	DrawTextLine(TEXT("Enter  —  resume"), 48.f, 120.f, FLinearColor::White);
	DrawTextLine(TEXT("Esc    —  abort to campaign select"), 48.f, 144.f, FLinearColor::White);
}

void AAshlineHUD::DrawTextLine(const FString& Text, float X, float Y, const FLinearColor& Color)
{
	FCanvasTextItem Item(FVector2D(X, Y), FText::FromString(Text), GEngine->GetSmallFont(), Color);
	Item.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(Item);
}
