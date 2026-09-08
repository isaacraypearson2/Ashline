using UnrealBuildTool;
using System.Collections.Generic;

public class AshlineTarget : TargetRules
{
	public AshlineTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		// Primary cook / play / Steam / Proton target is Win64 (DX12). Mac and iOS still compile.
		ExtraModuleNames.AddRange(new string[] { "Ashline", "AshlineApple" });

		if (Configuration == UnrealTargetConfiguration.Shipping)
		{
			bUseLoggingInShipping = false;
			bBuildDeveloperTools = false;
		}
	}
}
