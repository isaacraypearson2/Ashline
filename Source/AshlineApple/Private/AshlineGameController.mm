#include "AshlineAppleNative.h"

#if ASHLINE_APPLE_NATIVE

#import <Foundation/Foundation.h>
#import <GameController/GameController.h>
#import <CoreHaptics/CoreHaptics.h>

bool AshlineApple_ProbeMetalFXFramework(void)
{
	if (@available(macOS 13.0, iOS 16.0, *))
	{
		Class Spatial = NSClassFromString(@"MTLFXSpatialScalerDescriptor");
		Class Temporal = NSClassFromString(@"MTLFXTemporalScalerDescriptor");
		return Spatial != nil || Temporal != nil;
	}
	return false;
}

static GCController* AshlineFindDualSense(void)
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

bool AshlineApple_HasGameController(void)
{
	return AshlineFindDualSense() != nil || GCController.controllers.count > 0;
}

void AshlineApple_PulseHaptics(float Intensity)
{
	GCController* Controller = AshlineFindDualSense();
	if (!Controller)
	{
		return;
	}

	if (@available(macOS 11.0, iOS 14.0, *))
	{
		GCDeviceHaptics* Haptics = Controller.haptics;
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
			return;
		}

		float Clamped = Intensity;
		if (Clamped < 0.05f)
		{
			Clamped = 0.05f;
		}
		if (Clamped > 1.f)
		{
			Clamped = 1.f;
		}

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

void AshlineApple_SetAdaptiveTriggers(int WeaponClass)
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
			float Start = 0.15f;
			float End = 0.55f;
			float Strength = 0.35f;
			switch (WeaponClass)
			{
			case 1: Strength = 0.2f; break;
			case 2: Strength = 0.7f; Start = 0.4f; break;
			case 3: Strength = 0.85f; Start = 0.25f; break;
			case 4: Strength = 0.18f; break;
			case 5: Strength = 0.45f; break;
			case 6: Strength = 0.6f; End = 0.8f; break;
			default: break;
			}
			[Dual.rightTrigger setModeFeedbackWithStartPosition:Start resistiveStrength:Strength];
			[Dual.leftTrigger setModeWeaponWithStartPosition:0.1f endPosition:End resistiveStrength:Strength * 0.5f];
		}
	}
}

#endif
