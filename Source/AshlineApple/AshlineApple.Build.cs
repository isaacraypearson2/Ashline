using UnrealBuildTool;

public class AshlineApple : ModuleRules
{
	public AshlineApple(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"ApplicationCore",
			"RHI",
			"RenderCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"DeveloperSettings"
		});

		PublicIncludePaths.Add(ModuleDirectory + "/Public");
		PrivateIncludePaths.Add(ModuleDirectory + "/Private");

		if (Target.Platform == UnrealTargetPlatform.Mac || Target.Platform == UnrealTargetPlatform.IOS)
		{
			PublicFrameworks.AddRange(new string[]
			{
				"GameController",
				"Metal",
				"QuartzCore",
				"CoreHaptics"
			});

			// MetalFX.framework is weakly linked so the binary still loads on
			// Apple Silicon parts that expose Metal but not MetalFX Temporal.
			PublicWeakFrameworks.Add("MetalFX");

			PublicDefinitions.Add("ASHLINE_APPLE_NATIVE=1");
		}
		else
		{
			PublicDefinitions.Add("ASHLINE_APPLE_NATIVE=0");
		}
	}
}
