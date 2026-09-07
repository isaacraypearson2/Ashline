#include "Settings/AshlineGameUserSettings.h"

#include "Ashline.h"
#include "Kismet/GameplayStatics.h"

void UAshlineGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
	NamedPreset = EAshlineGraphicsPreset::PC_Ultra;
	PreferredUpscaler = EAshlineUpscaler::FSR3;
	TargetResX = 2560;
	TargetResY = 1440;
	SetScreenResolution(FIntPoint(TargetResX, TargetResY));
	SetFullscreenMode(EWindowMode::Fullscreen);
	SetVSyncEnabled(false);
	SetFrameRateLimit(0.f);
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
	SetScreenResolution(FIntPoint(TargetResX, TargetResY));
	SetFullscreenMode(EWindowMode::Fullscreen);
	SetVSyncEnabled(false);
	SetFrameRateLimit(0.f);
	ApplySettings(false);
	UE_LOG(LogAshline, Log, TEXT("Applied target PC resolution %dx%d fullscreen, VSync off (high-refresh)."), TargetResX, TargetResY);
}
