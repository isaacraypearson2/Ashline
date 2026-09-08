#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformMisc.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

/** Shipping hygiene. Debug execs are inert in UE_BUILD_SHIPPING. Graphics presets stay live. */
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
}
