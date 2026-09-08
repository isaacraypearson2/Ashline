#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformMisc.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

/**
 * Shipping hygiene. Debug execs (AshUnlockAll, AshGrantCredits, …) must be
 * inert in UE_BUILD_SHIPPING. Named graphics presets stay available.
 */
namespace AshlineShipping
{
	FORCEINLINE bool CheatsAllowed()
	{
#if UE_BUILD_SHIPPING
		return false;
#else
		return true;
#endif
	}

	FORCEINLINE bool IsSteamDeckHardware()
	{
		FString DeckEnv;
		FPlatformMisc::GetEnvironmentVariable(TEXT("SteamDeck"), DeckEnv);
		if (DeckEnv == TEXT("1") || DeckEnv.Equals(TEXT("true"), ESearchCase::IgnoreCase))
		{
			return true;
		}

		if (FParse::Param(FCommandLine::Get(), TEXT("steamdeck")))
		{
			return true;
		}

		FString SteamEnv;
		FPlatformMisc::GetEnvironmentVariable(TEXT("SteamEnv"), SteamEnv);
		if (SteamEnv.Contains(TEXT("steamdeck"), ESearchCase::IgnoreCase))
		{
			return true;
		}

		return false;
	}

	FORCEINLINE bool IsProton()
	{
		FString SteamRuntime;
		FPlatformMisc::GetEnvironmentVariable(TEXT("STEAM_COMPAT_DATA_PATH"), SteamRuntime);
		if (!SteamRuntime.IsEmpty())
		{
			return true;
		}
		FString Proton;
		FPlatformMisc::GetEnvironmentVariable(TEXT("PROTON_VERSION"), Proton);
		return !Proton.IsEmpty();
	}

	FORCEINLINE bool IsHandheldSafeRecommended()
	{
		return IsSteamDeckHardware() || IsProton();
	}
}
