#include "UI/AshlineHUD.h"

#include "Campaign/AshlineMissionCatalog.h"
#include "Campaign/AshlineObjectiveTrigger.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/GameInstance.h"
#include "Game/AshlineGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Meta/AshlineMetaCatalog.h"
#include "Player/AshlineCharacter.h"
#include "Progression/AshlineProgressionSubsystem.h"
#include "Settings/AshlineGameUserSettings.h"
#include "Settings/AshlineGraphicsSettings.h"
#include "UI/AshlineCombatFeedback.h"
#include "Weapons/AshlineWeaponCatalog.h"
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

	const TCHAR* ColorBlindName(EAshlineColorBlindMode Mode)
	{
		switch (Mode)
		{
		case EAshlineColorBlindMode::Protanopia: return TEXT("Protanopia");
		case EAshlineColorBlindMode::Deuteranopia: return TEXT("Deuteranopia");
		case EAshlineColorBlindMode::Tritanopia: return TEXT("Tritanopia");
		case EAshlineColorBlindMode::HighContrast: return TEXT("High Contrast");
		default: return TEXT("Off");
		}
	}

	const TCHAR* ClassShort(EAshlineWeaponClass Class)
	{
		switch (Class)
		{
		case EAshlineWeaponClass::AssaultRifle: return TEXT("AR");
		case EAshlineWeaponClass::SMG: return TEXT("SMG");
		case EAshlineWeaponClass::Sniper: return TEXT("SNIPER");
		case EAshlineWeaponClass::Shotgun: return TEXT("SHOTGUN");
		case EAshlineWeaponClass::Sidearm: return TEXT("PISTOL");
		case EAshlineWeaponClass::DMR: return TEXT("DMR");
		case EAshlineWeaponClass::LMG: return TEXT("LMG");
		case EAshlineWeaponClass::Launcher: return TEXT("GL");
		case EAshlineWeaponClass::Melee: return TEXT("MELEE");
		case EAshlineWeaponClass::BattleRifle: return TEXT("BR");
		case EAshlineWeaponClass::PDW: return TEXT("PDW");
		default: return TEXT("WPN");
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

	Metrics = BuildMetrics();
	Palette = BuildPalette();

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

	if (GameMode->MenuLayer == EAshlineMenuLayer::Settings && GameMode->PlayPhase == EAshlinePlayPhase::Frontend)
	{
		DrawFrontend(GameMode, Save);
		DrawSettings(GameMode);
		return;
	}

	if (GameMode->PlayPhase == EAshlinePlayPhase::Frontend)
	{
		DrawFrontend(GameMode, Save);
		return;
	}

	if (GameMode->PlayPhase == EAshlinePlayPhase::Briefing)
	{
		DrawBriefing(GameMode);
		return;
	}

	if (GameMode->PlayPhase == EAshlinePlayPhase::MissionComplete || GameMode->PlayPhase == EAshlinePlayPhase::CampaignComplete)
	{
		DrawMissionComplete(GameMode);
		return;
	}

	if (GameMode->PlayPhase == EAshlinePlayPhase::Paused)
	{
		if (bDrawDebugCombatHUD)
		{
			DrawCombat(GameMode);
		}
		DrawPause(GameMode);
		return;
	}

	if (bDrawDebugCombatHUD)
	{
		DrawCombat(GameMode);
	}
}

FAshlineFeelSettings AAshlineHUD::GetFeel() const
{
	if (UAshlineGameUserSettings* User = UAshlineGameUserSettings::GetAshlineSettings())
	{
		return User->Feel;
	}
	return FAshlineFeelSettings();
}

FAshlineHUDMetrics AAshlineHUD::BuildMetrics() const
{
	FAshlineHUDMetrics Out;
	const FAshlineFeelSettings Feel = GetFeel();
	const float H = Canvas ? static_cast<float>(Canvas->SizeY) : 800.f;
	const float W = Canvas ? static_cast<float>(Canvas->SizeX) : 1280.f;
	Out.bHandheld = Feel.bForceHandheldHUD || H <= 900.f || W <= 1280.f;
	Out.Scale = Feel.HUDScale * (Out.bHandheld ? 1.2f : 1.f);
	const float Safe = Out.bHandheld ? FMath::Max(Feel.SafeZone, 0.08f) : Feel.SafeZone;
	Out.Pad = FMath::Max(Out.bHandheld ? 48.f : 28.f, H * Safe);
	Out.AmmoScale = Out.bHandheld ? 1.55f : 1.28f;
	Out.PromptScale = Out.bHandheld ? 1.65f : 1.15f;
	Out.BarH = Out.bHandheld ? 14.f : 10.f;
	return Out;
}

FAshlineHUDPalette AAshlineHUD::BuildPalette() const
{
	FAshlineHUDPalette P;
	switch (GetFeel().ColorBlind)
	{
	case EAshlineColorBlindMode::Protanopia:
	case EAshlineColorBlindMode::Deuteranopia:
		P.Health = FLinearColor(0.95f, 0.85f, 0.2f);
		P.Armor = FLinearColor(0.35f, 0.55f, 0.95f);
		P.Danger = FLinearColor(0.95f, 0.7f, 0.15f);
		P.Kill = FLinearColor(1.f, 0.92f, 0.35f);
		break;
	case EAshlineColorBlindMode::Tritanopia:
		P.Health = FLinearColor(0.2f, 0.85f, 0.75f);
		P.Armor = FLinearColor(0.95f, 0.45f, 0.55f);
		P.Danger = FLinearColor(0.95f, 0.25f, 0.4f);
		break;
	case EAshlineColorBlindMode::HighContrast:
		P.Gold = FLinearColor::White;
		P.White = FLinearColor::White;
		P.Dim = FLinearColor(0.75f, 0.75f, 0.75f);
		P.Health = FLinearColor::White;
		P.Armor = FLinearColor(0.7f, 0.85f, 1.f);
		P.Danger = FLinearColor(1.f, 0.4f, 0.4f);
		P.Panel = FLinearColor(0.f, 0.f, 0.f, 0.82f);
		break;
	default:
		break;
	}
	return P;
}

UFont* AAshlineHUD::FontForScale(float Scale) const
{
	if (!GEngine)
	{
		return nullptr;
	}
	if (Scale >= 1.4f && GEngine->GetLargeFont())
	{
		return GEngine->GetLargeFont();
	}
	if (Scale >= 1.15f && GEngine->GetMediumFont())
	{
		return GEngine->GetMediumFont();
	}
	return GEngine->GetSmallFont();
}

void AAshlineHUD::DrawTextLine(const FString& Text, float X, float Y, const FLinearColor& Color)
{
	DrawTextScaled(Text, X, Y, Color, Metrics.Scale, true);
}

void AAshlineHUD::DrawTextScaled(const FString& Text, float X, float Y, const FLinearColor& Color, float Scale, bool bShadow)
{
	UFont* Font = FontForScale(Scale);
	if (!Font)
	{
		Font = GEngine->GetSmallFont();
	}
	FCanvasTextItem Item(FVector2D(X, Y), FText::FromString(Text), Font, Color);
	Item.Scale = FVector2D(Scale, Scale);
	if (bShadow)
	{
		Item.EnableShadow(FLinearColor(0.f, 0.f, 0.f, 0.85f));
	}
	Canvas->DrawItem(Item);
}

void AAshlineHUD::FillRect(float X, float Y, float W, float H, const FLinearColor& Color)
{
	DrawRect(Color, X, Y, W, H);
}

void AAshlineHUD::DrawBar(float X, float Y, float W, float H, float Alpha, const FLinearColor& Fill, const FLinearColor& Back)
{
	FillRect(X, Y, W, H, Back);
	FillRect(X, Y, W * FMath::Clamp(Alpha, 0.f, 1.f), H, Fill);
	DrawLine(X, Y, X + W, Y, Palette.White * 0.35f, 1.f);
	DrawLine(X, Y + H, X + W, Y + H, Palette.White * 0.35f, 1.f);
}

void AAshlineHUD::DrawWrapped(const FString& Text, float X, float Y, float MaxWidth, const FLinearColor& Color, float Scale)
{
	TArray<FString> Words;
	Text.ParseIntoArray(Words, TEXT(" "), true);
	FString Line;
	float CursorY = Y;
	const float CharW = 7.2f * Scale;
	for (const FString& Word : Words)
	{
		const FString Trial = Line.IsEmpty() ? Word : Line + TEXT(" ") + Word;
		if (Trial.Len() * CharW > MaxWidth && !Line.IsEmpty())
		{
			DrawTextScaled(Line, X, CursorY, Color, Scale);
			CursorY += 18.f * Scale;
			Line = Word;
		}
		else
		{
			Line = Trial;
		}
	}
	if (!Line.IsEmpty())
	{
		DrawTextScaled(Line, X, CursorY, Color, Scale);
	}
}

void AAshlineHUD::DrawFrontend(AAshlineGameMode* GameMode, UAshlineSaveGame* Save)
{
	const float Pad = Metrics.Pad;
	FillRect(0.f, 0.f, Canvas->SizeX, Canvas->SizeY, FLinearColor(0.f, 0.f, 0.f, 0.28f));
	FillRect(Pad - 12.f, 20.f, 420.f * Metrics.Scale, 52.f, Palette.Panel);
	DrawTextScaled(TEXT("ASHLINE"), Pad, 28.f, Palette.Gold, 1.55f * Metrics.Scale);
	DrawTextScaled(TEXT("SINGLE PLAYER CAMPAIGN"), Pad, 58.f, Palette.Dim, 0.95f * Metrics.Scale);

	const TArray<FAshlineMissionDefinition> Campaign = UAshlineMissionCatalog::BuildCampaign();
	const float RowH = Metrics.bHandheld ? 28.f : 22.f;
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
		const float Y = 100.f + i * RowH;
		if (bSelected)
		{
			FillRect(Pad - 8.f, Y - 2.f, 520.f * Metrics.Scale, RowH, FLinearColor(1.f, 0.85f, 0.3f, 0.16f));
		}
		const FString Line = FString::Printf(TEXT("%s %s  %s  [%s]"),
			bSelected ? TEXT(">") : TEXT(" "),
			*Campaign[i].CodeName,
			*Campaign[i].Title.ToString(),
			MissionStateName(State));
		const FLinearColor Color = bSelected
			? Palette.Gold
			: (State == EAshlineMissionState::Locked ? FLinearColor(0.35f, 0.35f, 0.35f) : Palette.White);
		DrawTextScaled(Line, Pad, Y, Color, (bSelected ? 1.05f : 0.95f) * Metrics.Scale);
	}

	if (Campaign.IsValidIndex(GameMode->SelectedMissionIndex))
	{
		const FAshlineMissionDefinition& Sel = Campaign[GameMode->SelectedMissionIndex];
		const float InfoX = Canvas->SizeX * 0.52f;
		FillRect(InfoX - 16.f, 96.f, Canvas->SizeX - InfoX - Pad, 220.f, Palette.Panel);
		DrawTextScaled(Sel.Title.ToString(), InfoX, 108.f, Palette.Gold, 1.15f * Metrics.Scale);
		DrawTextScaled(Sel.Location.ToString(), InfoX, 132.f, Palette.Dim, 0.95f * Metrics.Scale);
		DrawWrapped(Sel.Briefing.ToString(), InfoX, 160.f, Canvas->SizeX - InfoX - Pad - 12.f, Palette.White, 0.9f * Metrics.Scale);
	}

	const EAshlineDifficulty Difficulty = Save ? Save->Difficulty : EAshlineDifficulty::Regular;
	const int32 Rank = Save ? Save->Operator.Rank : 1;
	const int32 XP = Save ? Save->Operator.XP : 0;
	const int32 Tokens = Save ? Save->CrateTokens : 0;
	const int32 Credits = Save ? Save->Credits : 0;
	const int32 Prestige = Save ? Save->PrestigeLevel : 0;
	const FName CamoId = Save ? Save->Operator.CamoId : FName(TEXT("CAMO_FIELD"));
	const FName SkinId = Save ? Save->Primary.SkinId : FName(TEXT("SKIN_FACTORY"));
	FString SkinName = SkinId.ToString();
	FAshlineWeaponSkinDefinition SkinDef;
	if (UAshlineMetaCatalog::FindSkin(SkinId, SkinDef))
	{
		SkinName = SkinDef.DisplayName.ToString();
	}

	const float Bottom = Canvas->SizeY - Pad;
	FillRect(Pad - 8.f, Bottom - 88.f, Canvas->SizeX - Pad * 2.f, 96.f, Palette.Panel);
	DrawTextScaled(FString::Printf(TEXT("Difficulty  < %s >     Rank %d  P%d  XP %d  Credits %d  Crates %d"),
		DifficultyName(Difficulty), Rank, Prestige, XP, Credits, Tokens),
		Pad, Bottom - 80.f, Palette.Health, 0.95f * Metrics.Scale);
	DrawTextScaled(FString::Printf(TEXT("Operator %s   Camo %s   Primary skin %s   Lethal %s   Tactical %s"),
		Save ? *Save->Operator.Callsign : TEXT("ASH-0"),
		*CamoId.ToString(),
		*SkinName,
		Save ? *Save->Operator.LethalId.ToString() : TEXT("EQ_FRAG"),
		Save ? *Save->Operator.TacticalId.ToString() : TEXT("EQ_FLASH")),
		Pad, Bottom - 58.f, Palette.Dim, 0.9f * Metrics.Scale);

	FString PresetLine = TEXT("Graphics: Ashline_PC_Ultra");
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
	DrawTextScaled(PresetLine, Pad, Bottom - 36.f, Palette.Dim, 0.85f * Metrics.Scale);
	DrawTextScaled(TEXT("Up/Down select   Enter / A deploy   Left/Right difficulty   Esc settings"),
		Pad, Bottom - 16.f, Palette.Dim, 0.85f * Metrics.Scale);
}

void AAshlineHUD::DrawBriefing(AAshlineGameMode* GameMode)
{
	FillRect(0.f, 0.f, Canvas->SizeX, Canvas->SizeY, FLinearColor(0.01f, 0.015f, 0.02f, 0.78f));
	const float Pad = Metrics.Pad;
	DrawTextScaled(TEXT("MISSION BRIEF"), Pad, Pad, Palette.Gold, 1.4f * Metrics.Scale);
	DrawTextScaled(GameMode->ActiveDefinition.CodeName + TEXT("  —  ") + GameMode->ActiveDefinition.Title.ToString(),
		Pad, Pad + 36.f, Palette.White, 1.15f * Metrics.Scale);
	DrawTextScaled(GameMode->ActiveDefinition.Location.ToString(), Pad, Pad + 64.f, Palette.Dim, Metrics.Scale);
	DrawWrapped(GameMode->ActiveDefinition.Briefing.ToString(), Pad, Pad + 100.f, Canvas->SizeX * 0.62f, Palette.White, Metrics.Scale);

	float Y = Pad + 200.f;
	DrawTextScaled(TEXT("OBJECTIVES"), Pad, Y, Palette.Gold, Metrics.Scale);
	Y += 24.f;
	for (const FAshlineMissionObjective& Objective : GameMode->ActiveDefinition.Objectives)
	{
		DrawTextScaled(FString::Printf(TEXT("%s  %s"),
			Objective.bOptional ? TEXT("OPTIONAL") : TEXT("PRIMARY"),
			*Objective.DisplayName.ToString()),
			Pad, Y, Objective.bOptional ? Palette.Dim : Palette.White, Metrics.Scale);
		Y += 22.f * Metrics.Scale;
	}

	DrawTextScaled(TEXT("ENTER / A  —  DEPLOY"), Pad, Canvas->SizeY - Pad - 36.f, Palette.Gold, 1.2f * Metrics.Scale);
	DrawTextScaled(TEXT("ESC  —  abort"), Pad, Canvas->SizeY - Pad - 14.f, Palette.Dim, 0.9f * Metrics.Scale);
}

void AAshlineHUD::DrawCombat(AAshlineGameMode* GameMode)
{
	AAshlineCharacter* Character = Cast<AAshlineCharacter>(GetOwningPawn());
	UAshlineCombatFeedback* Feedback = GetWorld() ? GetWorld()->GetSubsystem<UAshlineCombatFeedback>() : nullptr;

	DrawVignette(Feedback);
	DrawObjectives(GameMode);
	DrawObjectiveMarker(GameMode);
	DrawVitals(Character);
	DrawWeaponPlate(Character);
	DrawCrosshair(Character, Feedback);
	DrawHitMarkers(Feedback);
	DrawKillFeed(Feedback);
	DrawInteract(Feedback);
	DrawSubtitle(Feedback);

	if (Character && Character->IsDowned())
	{
		FillRect(0.f, 0.f, Canvas->SizeX, Canvas->SizeY, FLinearColor(0.15f, 0.f, 0.f, 0.35f));
		DrawTextScaled(TEXT("KIA"), Canvas->SizeX * 0.5f - 40.f, Canvas->SizeY * 0.42f, Palette.Danger, 1.8f * Metrics.Scale);
	}
}

void AAshlineHUD::DrawVitals(AAshlineCharacter* Character)
{
	if (!Character)
	{
		return;
	}
	const float X = Metrics.Pad;
	const float Y = Canvas->SizeY - Metrics.Pad - 52.f;
	const float W = Metrics.bHandheld ? 280.f : 220.f;
	FillRect(X - 8.f, Y - 8.f, W + 16.f, 56.f, Palette.Panel);
	DrawBar(X, Y, W, Metrics.BarH, Character->MaxHealth > 0.f ? Character->Health / Character->MaxHealth : 0.f,
		Palette.Health, FLinearColor(0.08f, 0.1f, 0.08f, 0.7f));
	DrawBar(X, Y + Metrics.BarH + 6.f, W, Metrics.BarH, Character->MaxArmor > 0.f ? Character->Armor / Character->MaxArmor : 0.f,
		Palette.Armor, FLinearColor(0.06f, 0.08f, 0.12f, 0.7f));
	DrawTextScaled(FString::Printf(TEXT("HP %0.0f   ARM %0.0f"), Character->Health, Character->Armor),
		X, Y - 16.f, Palette.White, 0.85f * Metrics.Scale);
}

void AAshlineHUD::DrawWeaponPlate(AAshlineCharacter* Character)
{
	if (!Character || !Character->WeaponComponent)
	{
		return;
	}
	const FAshlineRuntimeWeapon& Weapon = Character->WeaponComponent->GetActiveWeapon();
	FString SkinName = Weapon.SkinId.ToString();
	FAshlineWeaponSkinDefinition SkinDef;
	if (UAshlineMetaCatalog::FindSkin(Weapon.SkinId, SkinDef))
	{
		SkinName = SkinDef.DisplayName.ToString();
	}

	const FString FireMode = Character->WeaponComponent->GetFireModeLabel();
	const bool bReloading = Character->WeaponComponent->IsReloading();
	const bool bEmpty = Weapon.AmmoInMag <= 0 && Weapon.Reserve <= 0;
	const float PlateW = Metrics.bHandheld ? 340.f : 280.f;
	const float X = Canvas->SizeX - Metrics.Pad - PlateW;
	const float Y = Canvas->SizeY - Metrics.Pad - 70.f;
	FillRect(X - 10.f, Y - 10.f, PlateW + 20.f, 78.f, Palette.Panel);

	DrawTextScaled(Weapon.Definition.DisplayName.ToString(), X, Y, Palette.White, 1.05f * Metrics.Scale);
	DrawTextScaled(FString::Printf(TEXT("%s   %s   %s   %s"),
		ClassShort(Weapon.Definition.Class),
		*FireMode,
		*UAshlineWeaponCatalog::AmmoTypeName(Weapon.Stats.AmmoType),
		*SkinName),
		X, Y + 20.f, Palette.Dim, 0.85f * Metrics.Scale);

	const FLinearColor AmmoColor = bEmpty ? Palette.Danger : (Weapon.AmmoInMag <= 5 ? Palette.Gold : Palette.White);
	DrawTextScaled(FString::Printf(TEXT("%d"), Weapon.AmmoInMag), X, Y + 38.f, AmmoColor, Metrics.AmmoScale * Metrics.Scale);
	DrawTextScaled(FString::Printf(TEXT("/ %d"), Weapon.Reserve), X + 70.f * Metrics.AmmoScale, Y + 46.f, Palette.Dim, Metrics.Scale);

	if (bReloading)
	{
		DrawBar(X, Y + 64.f, PlateW, 4.f, Character->WeaponComponent->GetReloadAlpha(), Palette.Gold, FLinearColor(0.1f, 0.1f, 0.1f, 0.6f));
		DrawTextScaled(TEXT("RELOAD"), X + PlateW - 70.f, Y + 38.f, Palette.Gold, Metrics.Scale);
	}
}

void AAshlineHUD::DrawObjectives(AAshlineGameMode* GameMode)
{
	const float X = Metrics.Pad;
	float Y = Metrics.Pad;
	DrawTextScaled(GameMode->ActiveDefinition.CodeName + TEXT("  ") + GameMode->ActiveDefinition.Title.ToString(),
		X, Y, Palette.Gold, Metrics.Scale);
	Y += 22.f * Metrics.Scale;
	for (const FAshlineMissionObjective& Objective : GameMode->ActiveDefinition.Objectives)
	{
		const FString Mark = Objective.bCompleted ? TEXT("[x]") : TEXT("[ ]");
		const FString Line = FString::Printf(TEXT("%s %s%s"),
			*Mark,
			*Objective.DisplayName.ToString(),
			Objective.bOptional ? TEXT("  OPT") : TEXT(""));
		DrawTextScaled(Line, X, Y, Objective.bCompleted ? Palette.Health : Palette.White, 0.95f * Metrics.Scale);
		Y += 18.f * Metrics.Scale;
	}
}

void AAshlineHUD::DrawObjectiveMarker(AAshlineGameMode* GameMode)
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	TArray<AActor*> Triggers;
	UGameplayStatics::GetAllActorsOfClass(this, AAshlineObjectiveTrigger::StaticClass(), Triggers);
	AAshlineObjectiveTrigger* Best = nullptr;
	for (AActor* Actor : Triggers)
	{
		AAshlineObjectiveTrigger* Trigger = Cast<AAshlineObjectiveTrigger>(Actor);
		if (!Trigger || Trigger->IsConsumed())
		{
			continue;
		}
		if (Trigger->bCompleteMissionOnEnter)
		{
			Best = Trigger;
			break;
		}
		if (!Best)
		{
			Best = Trigger;
		}
	}
	if (!Best)
	{
		return;
	}

	FVector2D Screen;
	if (!PC->ProjectWorldLocationToScreen(Best->GetActorLocation() + FVector(0.f, 0.f, 80.f), Screen, true))
	{
		return;
	}
	const float Edge = Metrics.Pad;
	Screen.X = FMath::Clamp(Screen.X, Edge, Canvas->SizeX - Edge);
	Screen.Y = FMath::Clamp(Screen.Y, Edge, Canvas->SizeY - Edge * 2.f);
	const float S = 7.f * Metrics.Scale;
	DrawLine(Screen.X - S, Screen.Y, Screen.X + S, Screen.Y, Palette.Gold, 2.f);
	DrawLine(Screen.X, Screen.Y - S, Screen.X, Screen.Y + S, Palette.Gold, 2.f);
	DrawTextScaled(Best->bCompleteMissionOnEnter ? TEXT("EXFIL") : Best->ObjectiveId.ToString(),
		Screen.X + 10.f, Screen.Y - 8.f, Palette.Gold, 0.85f * Metrics.Scale);
	(void)GameMode;
}

void AAshlineHUD::DrawCrosshair(AAshlineCharacter* Character, UAshlineCombatFeedback* Feedback)
{
	const float CX = Canvas->SizeX * 0.5f;
	const float CY = Canvas->SizeY * 0.5f;
	const bool bADS = Character && Character->IsAiming();
	const bool bReload = Character && Character->WeaponComponent && Character->WeaponComponent->IsReloading();
	const bool bEmpty = Character && Character->WeaponComponent && Character->WeaponComponent->GetActiveWeapon().AmmoInMag <= 0;
	float Gap = bADS ? 3.f : 9.f;
	float Len = bADS ? 5.f : 8.f;
	if (Metrics.bHandheld)
	{
		Gap += 2.f;
		Len += 2.f;
	}
	FLinearColor Color = Palette.White;
	if (bEmpty)
	{
		Color = Palette.Danger;
	}
	else if (bReload)
	{
		Color = Palette.Gold;
	}
	if (Feedback && Feedback->HitMarkerRemaining > 0.f && Feedback->bLastHitWasKill)
	{
		Color = Palette.Kill;
	}

	DrawLine(CX - Gap - Len, CY, CX - Gap, CY, Color, bADS ? 1.f : 1.5f);
	DrawLine(CX + Gap, CY, CX + Gap + Len, CY, Color, bADS ? 1.f : 1.5f);
	DrawLine(CX, CY - Gap - Len, CX, CY - Gap, Color, bADS ? 1.f : 1.5f);
	DrawLine(CX, CY + Gap, CX, CY + Gap + Len, Color, bADS ? 1.f : 1.5f);
	if (!bADS)
	{
		FillRect(CX - 1.f, CY - 1.f, 2.f, 2.f, Color);
	}
}

void AAshlineHUD::DrawHitMarkers(UAshlineCombatFeedback* Feedback)
{
	if (!Feedback || !GetFeel().bHitMarkers || Feedback->HitMarkerRemaining <= 0.f)
	{
		return;
	}
	const float CX = Canvas->SizeX * 0.5f;
	const float CY = Canvas->SizeY * 0.5f;
	const float A = FMath::Clamp(Feedback->HitMarkerRemaining / 0.16f, 0.f, 1.f);
	const float S = (Feedback->bLastHitWasKill ? 14.f : 10.f) * Metrics.Scale;
	FLinearColor C = Feedback->bLastHitWasKill ? Palette.Kill : Palette.Hit;
	C.A = A;
	DrawLine(CX - S, CY - S, CX - S * 0.35f, CY - S * 0.35f, C, 2.f);
	DrawLine(CX + S, CY - S, CX + S * 0.35f, CY - S * 0.35f, C, 2.f);
	DrawLine(CX - S, CY + S, CX - S * 0.35f, CY + S * 0.35f, C, 2.f);
	DrawLine(CX + S, CY + S, CX + S * 0.35f, CY + S * 0.35f, C, 2.f);

	if (Feedback->KillConfirmRemaining > 0.f && GetFeel().bKillConfirm)
	{
		DrawTextScaled(FString::Printf(TEXT("KILLED  %s"), *Feedback->KillConfirmName),
			CX - 70.f * Metrics.Scale, CY + 28.f, Palette.Kill, 1.05f * Metrics.Scale);
	}
}

void AAshlineHUD::DrawKillFeed(UAshlineCombatFeedback* Feedback)
{
	if (!Feedback)
	{
		return;
	}
	float Y = Metrics.Pad;
	const float X = Canvas->SizeX - Metrics.Pad - 260.f * Metrics.Scale;
	for (const FAshlineKillFeedItem& Item : Feedback->KillFeed)
	{
		FLinearColor C = Palette.White;
		C.A = FMath::Clamp(Item.Remaining / 1.2f, 0.f, 1.f);
		DrawTextScaled(Item.Text, X, Y, C, 0.9f * Metrics.Scale);
		Y += 18.f * Metrics.Scale;
	}
}

void AAshlineHUD::DrawInteract(UAshlineCombatFeedback* Feedback)
{
	if (!Feedback || Feedback->InteractPrompt.IsEmpty())
	{
		return;
	}
	const float Scale = Metrics.PromptScale * Metrics.Scale;
	const FString Key = Metrics.bHandheld ? TEXT("[LB / F]") : TEXT("[F]");
	const FString Line = FString::Printf(TEXT("%s  %s"), *Key, *Feedback->InteractPrompt);
	const float W = 18.f * Line.Len() * Scale * 0.45f + 40.f;
	const float X = Canvas->SizeX * 0.5f - W * 0.5f;
	const float Y = Canvas->SizeY * 0.62f;
	FillRect(X, Y, W, 36.f * Scale, FLinearColor(0.f, 0.f, 0.f, 0.7f));
	DrawTextScaled(Line, X + 16.f, Y + 8.f, Palette.Gold, Scale);
}

void AAshlineHUD::DrawVignette(UAshlineCombatFeedback* Feedback)
{
	if (!Feedback || !GetFeel().bDamageVignette || Feedback->DamagePulse <= 0.02f)
	{
		return;
	}
	const float A = FMath::Clamp(Feedback->DamagePulse, 0.f, 0.7f);
	const float T = 90.f * Metrics.Scale;
	FillRect(0.f, 0.f, Canvas->SizeX, T, FLinearColor(0.4f, 0.f, 0.f, A * 0.55f));
	FillRect(0.f, Canvas->SizeY - T, Canvas->SizeX, T, FLinearColor(0.4f, 0.f, 0.f, A * 0.65f));
	FillRect(0.f, 0.f, T, Canvas->SizeY, FLinearColor(0.4f, 0.f, 0.f, A * 0.5f));
	FillRect(Canvas->SizeX - T, 0.f, T, Canvas->SizeY, FLinearColor(0.4f, 0.f, 0.f, A * 0.5f));
}

void AAshlineHUD::DrawSubtitle(UAshlineCombatFeedback* Feedback)
{
	if (!Feedback || Feedback->Subtitle.IsEmpty() || !GetFeel().bSubtitles)
	{
		return;
	}
	const float W = Canvas->SizeX * 0.7f;
	const float X = Canvas->SizeX * 0.15f;
	const float Y = Canvas->SizeY - Metrics.Pad - 120.f;
	FillRect(X, Y, W, 48.f, FLinearColor(0.f, 0.f, 0.f, 0.45f));
	DrawWrapped(Feedback->Subtitle, X + 12.f, Y + 10.f, W - 24.f, Palette.White, 0.95f * Metrics.Scale);
}

void AAshlineHUD::DrawMissionComplete(AAshlineGameMode* GameMode)
{
	FillRect(0.f, 0.f, Canvas->SizeX, Canvas->SizeY, FLinearColor(0.01f, 0.015f, 0.02f, 0.72f));
	const bool bFinale = GameMode->PlayPhase == EAshlinePlayPhase::CampaignComplete;
	const float Pad = Metrics.Pad;
	DrawTextScaled(bFinale ? TEXT("ASHLINE CUT") : TEXT("MISSION COMPLETE"), Pad, Pad + 20.f, Palette.Gold, 1.55f * Metrics.Scale);
	DrawTextScaled(GameMode->ActiveDefinition.CodeName + TEXT("  ") + GameMode->ActiveDefinition.Title.ToString(),
		Pad, Pad + 64.f, Palette.White, 1.1f * Metrics.Scale);

	FString Stars;
	for (int32 i = 0; i < 3; ++i)
	{
		Stars += i < GameMode->LastAwardedStars ? TEXT("★ ") : TEXT("☆ ");
	}
	DrawTextScaled(Stars, Pad, Pad + 100.f, Palette.Gold, 1.3f * Metrics.Scale);
	DrawTextScaled(FString::Printf(TEXT("XP +%d     Credits +%d     Crate tokens +%d     Kills %d"),
		GameMode->LastAwardedXP, GameMode->LastAwardedCredits, GameMode->LastAwardedCrates, GameMode->MissionKills),
		Pad, Pad + 140.f, Palette.Health, Metrics.Scale);
	DrawTextScaled(TEXT("Progress saved to slot AshlineCampaign."), Pad, Pad + 168.f, Palette.Dim, 0.9f * Metrics.Scale);
	DrawTextScaled(TEXT("ENTER / A  —  return to campaign select"), Pad, Pad + 210.f, Palette.Gold, 1.1f * Metrics.Scale);
}

void AAshlineHUD::DrawPause(AAshlineGameMode* GameMode)
{
	FillRect(0.f, 0.f, Canvas->SizeX, Canvas->SizeY, FLinearColor(0.f, 0.f, 0.f, 0.55f));
	if (GameMode->MenuLayer == EAshlineMenuLayer::Settings)
	{
		DrawSettings(GameMode);
		return;
	}

	const float X = Metrics.Pad;
	const float Y = Canvas->SizeY * 0.28f;
	DrawTextScaled(TEXT("PAUSED"), X, Y, Palette.Gold, 1.5f * Metrics.Scale);
	const TCHAR* Rows[3] = { TEXT("Resume"), TEXT("Settings"), TEXT("Abort to campaign select") };
	for (int32 i = 0; i < 3; ++i)
	{
		const bool bSel = GameMode->MenuCursor == i;
		if (bSel)
		{
			FillRect(X - 8.f, Y + 48.f + i * 28.f, 360.f, 24.f, FLinearColor(1.f, 0.85f, 0.3f, 0.14f));
		}
		DrawTextScaled(FString::Printf(TEXT("%s %s"), bSel ? TEXT(">") : TEXT(" "), Rows[i]),
			X, Y + 48.f + i * 28.f, bSel ? Palette.Gold : Palette.White, Metrics.Scale);
	}
	DrawTextScaled(TEXT("Up/Down  Enter confirm  Esc abort"), X, Y + 150.f, Palette.Dim, 0.9f * Metrics.Scale);
}

void AAshlineHUD::DrawSettings(AAshlineGameMode* GameMode)
{
	FillRect(0.f, 0.f, Canvas->SizeX, Canvas->SizeY, FLinearColor(0.f, 0.f, 0.f, 0.72f));
	const float X = Metrics.Pad;
	float Y = Metrics.Pad;
	DrawTextScaled(TEXT("SETTINGS"), X, Y, Palette.Gold, 1.4f * Metrics.Scale);
	Y += 36.f;

	FString Preset = TEXT("Ashline_PC_Ultra");
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineGraphicsSettings* Graphics = GI->GetSubsystem<UAshlineGraphicsSettings>())
		{
			Preset = UAshlineGraphicsSettings::GetPresetDisplayName(Graphics->GetState().Preset);
		}
	}
	const FAshlineFeelSettings Feel = GetFeel();
	const TArray<FString> Rows = {
		FString::Printf(TEXT("Graphics preset     < %s >"), *Preset),
		FString::Printf(TEXT("HUD scale           < %.2f >"), Feel.HUDScale),
		FString::Printf(TEXT("Safe zone           < %.2f >"), Feel.SafeZone),
		FString::Printf(TEXT("Colorblind          < %s >"), ColorBlindName(Feel.ColorBlind)),
		FString::Printf(TEXT("Camera shake        < %s >"), Feel.bCameraShake ? TEXT("ON") : TEXT("off")),
		FString::Printf(TEXT("Hit markers         < %s >"), Feel.bHitMarkers ? TEXT("ON") : TEXT("off")),
		FString::Printf(TEXT("Damage vignette     < %s >"), Feel.bDamageVignette ? TEXT("ON") : TEXT("off")),
		FString::Printf(TEXT("Hip FOV             < %.0f >"), Feel.HipFOV),
		FString::Printf(TEXT("Master volume       < %.0f%% >"), Feel.MasterVolume * 100.f),
		FString::Printf(TEXT("Music volume        < %.0f%% >"), Feel.MusicVolume * 100.f),
		FString::Printf(TEXT("Handheld HUD        < %s >"), Feel.bForceHandheldHUD ? TEXT("ON") : TEXT("off")),
		TEXT("Back")
	};

	const float RowH = Metrics.bHandheld ? 28.f : 22.f;
	for (int32 i = 0; i < Rows.Num(); ++i)
	{
		const bool bSel = GameMode->SettingsCursor == i;
		if (bSel)
		{
			FillRect(X - 8.f, Y - 2.f, 560.f * Metrics.Scale, RowH, FLinearColor(1.f, 0.85f, 0.3f, 0.14f));
		}
		DrawTextScaled(FString::Printf(TEXT("%s %s"), bSel ? TEXT(">") : TEXT(" "), *Rows[i]),
			X, Y, bSel ? Palette.Gold : Palette.White, Metrics.Scale);
		Y += RowH;
	}
	DrawTextScaled(TEXT("Up/Down move   Left/Right change   Enter/Esc back"),
		X, Canvas->SizeY - Metrics.Pad - 18.f, Palette.Dim, 0.9f * Metrics.Scale);
}
