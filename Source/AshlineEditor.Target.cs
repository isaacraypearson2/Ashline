using UnrealBuildTool;
using System.Collections.Generic;

public class AshlineEditorTarget : TargetRules
{
	public AshlineEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		// Open Ashline.uproject on Windows with UE 5.8.2. Mac editor still supported.
		ExtraModuleNames.AddRange(new string[] { "Ashline", "AshlineApple" });
	}
}
