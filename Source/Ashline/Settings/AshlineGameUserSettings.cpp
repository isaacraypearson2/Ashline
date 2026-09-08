#include "Settings/AshlineGameUserSettings.h"

#include "Ashline.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Presentation/AshlineAudioDirector.h"

void UAshlineGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
	NamedPreset = EAshlineGraphicsPreset::PC_Ultra;
	PreferredUpscaler = EAshlineUpscaler::FSR3;
	TargetResX = 2560;
	TargetResY = 1440;
	bAutoDetectPreset = true;
	FrameTarget = EAshlineFrameTarget::Unlimited;
	SafeZoneScale = 0.f;
	bHandheldLayout = false;
	Feel = FAshlineFeelSettings();
	if (IsSteamDeckHardware())
	{
		NamedPreset = EAshlineGraphicsPreset::SteamDeck;
		PreferredUpscaler = EAshlineUpscaler::TSR;
		TargetResX = 1280;
		TargetResY = 800;
		Feel.bForceHandheldHUD = true;
		Feel.HUDScale = 1.25f;
		Feel.SafeZone = 0.08f;
		SetVSyncEnabled(true);
		SetFrameRateLimit(60.f);
	}
	else
	{
		SetVSyncEnabled(false);
		SetFrameRateLimit(0.f);
	}
	SetScreenResolution(FIntPoint(TargetResX, TargetResY));
	SetFullscreenMode(EWindowMode::Fullscreen);
}

void UAshlineGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
}

UAshlineGameUserSettings* UAshlineGameUserSettings::GetAshlineSettings()
{
	return Cast<UAshlineGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UAshlineGameUserSettings::ApplyTargetPCResolution()
{
	if (IsSteamDeckHardware() || NamedPreset == EAshlineGraphicsPreset::SteamDeck)
	{
		TargetResX = 1280;
		TargetResY = 800;
		SetScreenResolution(FIntPoint(TargetResX, TargetResY));
		SetFullscreenMode(EWindowMode::Fullscreen);
		SetVSyncEnabled(true);
		SetFrameRateLimit(60.f);
		ApplySettings(false);
		UE_LOG(LogAshline, Log, TEXT("Applied Steam Deck resolution 1280x800, VSync on, 60 fps cap."));
		return;
	}

	SetScreenResolution(FIntPoint(TargetResX, TargetResY));
	SetFullscreenMode(EWindowMode::Fullscreen);
	SetVSyncEnabled(false);
	SetFrameRateLimit(0.f);
	ApplySettings(false);
	UE_LOG(LogAshline, Log, TEXT("Applied target PC resolution %dx%d fullscreen, VSync off (high-refresh)."), TargetResX, TargetResY);
}

void UAshlineGameUserSettings::ApplyFeelToAudio()
{
	if (!GEngine)
	{
		return;
	}
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		UWorld* World = Context.World();
		if (!World || !World->IsGameWorld())
		{
			continue;
		}
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
			{
				Audio->SetBusVolumes(Feel.MasterVolume, Feel.SFXVolume, Feel.MusicVolume);
			}
		}
	}
}

bool UAshlineGameUserSettings::IsSteamDeckHardware()
{
	FString DeckEnv;
	FParse::Value(FCommandLine::Get(), TEXT("steamdeck="), DeckEnv);
	if (DeckEnv == TEXT("1") || DeckEnv.Equals(TEXT("true"), ESearchCase::IgnoreCase))
	{
		return true;
	}

	FString Env = FPlatformMisc::GetEnvironmentVariable(TEXT("SteamDeck"));
	if (Env == TEXT("1"))
	{
		return true;
	}

	const FString Device = FPlatformMisc::GetDeviceMakeAndModel();
	if (Device.Contains(TEXT("Steam Deck")) || Device.Contains(TEXT("Jupiter")))
	{
		return true;
	}

	if (GEngine)
	{
		if (UGameUserSettings* User = UGameUserSettings::GetGameUserSettings())
		{
			const FIntPoint Res = User->GetScreenResolution();
			if (Res.X == 1280 && (Res.Y == 800 || Res.Y == 720))
			{
				return true;
			}
		}
	}
	return false;
}
