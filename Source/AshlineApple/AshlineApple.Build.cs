using System.IO;
using UnrealBuildTool;

public class AshlineApple : ModuleRules
{
	public AshlineApple(ReadOnlyTargetRules Target) : base(Target)
	{
		// UE PCH + Foundation/CarbonCore collide on FVector. This module mixes
		// ObjC++ and must compile without engine PCHs.
		PCHUsage = PCHUsageMode.NoPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"ApplicationCore",
			"RHI",
			"RenderCore"
		});

		PublicIncludePaths.Add(ModuleDirectory + "/Public");
		PrivateIncludePaths.Add(ModuleDirectory + "/Private");

		// Only the cooked/game Apple platforms get native frameworks.
		// A Windows/Linux editor host must still compile this module as stubs
		// (Ashline depends on AshlineApple).
		if (IsAppleGamePlatform(Target))
		{
			PublicDefinitions.Add("ASHLINE_APPLE_NATIVE=1");

			// UBT 5.8 has no PrivateFrameworks on ModuleRules — PublicFrameworks
			// is the supported list for GameController / CoreHaptics.
			PublicFrameworks.AddRange(new string[]
			{
				"GameController",
				"CoreHaptics"
			});

			// Metal / QuartzCore are already linked by the engine MetalRHI.
			// Do not re-export them as public frameworks.

			// MetalFX is optional. Weak-link only when the SDK actually has
			// the framework. A missing Xcode Metal Toolchain must not fail
			// the AshlineApple link.
			if (TryAddWeakMetalFX(Target))
			{
				PublicDefinitions.Add("ASHLINE_METALFX_LINKED=1");
			}
			else
			{
				PublicDefinitions.Add("ASHLINE_METALFX_LINKED=0");
			}
		}
		else
		{
			PublicDefinitions.Add("ASHLINE_APPLE_NATIVE=0");
			PublicDefinitions.Add("ASHLINE_METALFX_LINKED=0");
		}
	}

	static bool IsAppleGamePlatform(ReadOnlyTargetRules Target)
	{
		return Target.Platform == UnrealTargetPlatform.Mac
			|| Target.Platform == UnrealTargetPlatform.IOS
			|| Target.Platform == UnrealTargetPlatform.TVOS;
	}

	bool TryAddWeakMetalFX(ReadOnlyTargetRules Target)
	{
		try
		{
			if (!MetalFXFrameworkExists())
			{
				System.Console.WriteLine("AshlineApple: MetalFX.framework not found in the active SDK — skipping weak link (install Xcode Settings → Components → Metal Toolchain to enable).");
				return false;
			}

			PublicWeakFrameworks.Add("MetalFX");
			return true;
		}
		catch (System.Exception Ex)
		{
			System.Console.WriteLine("AshlineApple: MetalFX weak-link skipped (" + Ex.Message + ").");
			return false;
		}
	}

	static bool MetalFXFrameworkExists()
	{
		string[] Roots =
		{
			AppleSdkFrameworkPath("macosx"),
			AppleSdkFrameworkPath("iphoneos"),
			"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/MetalFX.framework",
			"/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/System/Library/Frameworks/MetalFX.framework"
		};

		foreach (string Candidate in Roots)
		{
			if (!string.IsNullOrEmpty(Candidate) && Directory.Exists(Candidate))
			{
				return true;
			}
		}

		return false;
	}

	static string AppleSdkFrameworkPath(string SdkName)
	{
		try
		{
			System.Diagnostics.ProcessStartInfo Info = new System.Diagnostics.ProcessStartInfo
			{
				FileName = "/usr/bin/xcrun",
				Arguments = "--sdk " + SdkName + " --show-sdk-path",
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				UseShellExecute = false,
				CreateNoWindow = true
			};

			using (System.Diagnostics.Process Process = System.Diagnostics.Process.Start(Info))
			{
				if (Process == null)
				{
					return string.Empty;
				}

				string SdkPath = Process.StandardOutput.ReadToEnd().Trim();
				Process.WaitForExit();
				if (Process.ExitCode != 0 || string.IsNullOrEmpty(SdkPath))
				{
					return string.Empty;
				}

				return Path.Combine(SdkPath, "System", "Library", "Frameworks", "MetalFX.framework");
			}
		}
		catch
		{
			return string.Empty;
		}
	}
}
