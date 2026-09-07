using UnrealBuildTool;

public class Ashline : ModuleRules
{
	public Ashline(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"UMG",
			"Slate",
			"SlateCore",
			"GameplayTags",
			"DeveloperSettings",
			"AshlineApple"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"ApplicationCore",
			"RHI",
			"RenderCore",
			"EngineSettings"
		});

		PublicIncludePaths.AddRange(new string[]
		{
			ModuleDirectory
		});
	}
}
