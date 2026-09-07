#pragma once

#include "CoreMinimal.h"

class ASHLINEAPPLE_API IAshlineMetalFX
{
public:
	virtual ~IAshlineMetalFX() = default;

	virtual bool IsMetalFXAvailable() const = 0;
	virtual bool IsHardwareRayTracingAvailable() const = 0;
	virtual FString GetCapabilitySummary() const = 0;

	/** 0 = off, 1 = spatial, 2 = temporal */
	virtual void ApplyUpscaler(int32 Mode) = 0;

	static IAshlineMetalFX* Get();
};
