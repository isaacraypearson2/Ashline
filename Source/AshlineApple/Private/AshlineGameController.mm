#include "AshlineApple.h"

#if ASHLINE_APPLE_NATIVE

#import <Foundation/Foundation.h>
#import <GameController/GameController.h>
#import <CoreHaptics/CoreHaptics.h>

#if __has_include(<MetalFX/MetalFX.h>)
#import <MetalFX/MetalFX.h>
#define ASHLINE_HAS_METALFX_HEADER 1
#else
#define ASHLINE_HAS_METALFX_HEADER 0
#endif

bool AshlineApple_ProbeMetalFXFramework()
{
#if ASHLINE_HAS_METALFX_HEADER
	if (@available(macOS 13.0, iOS 16.0, *))
	{
		Class Spatial = NSClassFromString(@"MTLFXSpatialScalerDescriptor");
		Class Temporal = NSClassFromString(@"MTLFXTemporalScalerDescriptor");
		return Spatial != nil || Temporal != nil;
	}
#endif
	return false;
}

static GCController* AshlineFindDualSense()
{
	for (GCController* Controller in GCController.controllers)
	{
		if (@available(macOS 11.0, iOS 14.5, *))
		{
			NSString* Category = Controller.productCategory;
			if (Category && ([Category containsString:@"DualSense"] || [Category containsString:@"DualShock"]))
			{
				return Controller;
			}
		}
		if (Controller.extendedGamepad)
		{
			return Controller;
		}
	}
	return nil;
}

extern "C" bool AshlineApple_HasGameController()
{
	return AshlineFindDualSense() != nil || GCController.controllers.count > 0;
}

extern "C" void AshlineApple_PulseHaptics(float Intensity)
{
	GCController* Controller = AshlineFindDualSense();
	if (!Controller)
	{
		return;
	}

	if (@available(macOS 11.0, iOS 14.0, *))
	{
		id<GCDeviceHaptics> Haptics = Controller.haptics;
		if (!Haptics)
		{
			return;
		}
		CHHapticEngine* Engine = [Haptics createEngineWithLocality:GCHapticsLocalityDefault];
		if (!Engine)
		{
			return;
		}
		NSError* Error = nil;
		[Engine startAndReturnError:&Error];
		if (Error)
		{
			UE_LOG(LogAshlineApple, Verbose, TEXT("Haptic engine start failed."));
			return;
		}

		const float Clamped = FMath::Clamp(Intensity, 0.05f, 1.f);
		CHHapticEventParameter* IntensityParam = [[CHHapticEventParameter alloc]
			initWithParameterID:CHHapticEventParameterIDHapticIntensity
						  value:Clamped];
		CHHapticEvent* Event = [[CHHapticEvent alloc]
			initWithEventType:CHHapticEventTypeHapticTransient
				   parameters:@[IntensityParam]
				 relativeTime:0];
		CHHapticPattern* Pattern = [[CHHapticPattern alloc] initWithEvents:@[Event] parameters:@[] error:&Error];
		if (Pattern)
		{
			id<CHHapticPatternPlayer> Player = [Engine createPlayerWithPattern:Pattern error:&Error];
			[Player startAtTime:0 error:&Error];
		}
	}
}

extern "C" void AshlineApple_SetAdaptiveTriggers(int32 WeaponClass)
{
	GCController* Controller = AshlineFindDualSense();
	if (!Controller)
	{
		return;
	}

	if (@available(macOS 12.3, iOS 15.4, *))
	{
		if ([Controller.extendedGamepad isKindOfClass:[GCDualSenseGamepad class]])
		{
			GCDualSenseGamepad* Dual = (GCDualSenseGamepad*)Controller.extendedGamepad;
			// Resistance maps: AR/LMG heavier right trigger, SMG/pistol lighter.
			float Start = 0.15f;
			float End = 0.55f;
			float Strength = 0.35f;
			switch (WeaponClass)
			{
			case 1: Strength = 0.2f; break;  // SMG
			case 2: Strength = 0.7f; Start = 0.4f; break; // Sniper
			case 3: Strength = 0.85f; Start = 0.25f; break; // Shotgun
			case 4: Strength = 0.18f; break; // Sidearm
			case 5: Strength = 0.45f; break; // DMR
			case 6: Strength = 0.6f; End = 0.8f; break; // LMG
			default: break;
			}
			[Dual.rightTrigger setModeFeedbackWithStartPosition:Start resistiveStrength:Strength];
			[Dual.leftTrigger setModeWeaponWithStartPosition:0.1f endPosition:End resistiveStrength:Strength * 0.5f];
		}
	}
}

#else

bool AshlineApple_ProbeMetalFXFramework()
{
	return false;
}

#endif
