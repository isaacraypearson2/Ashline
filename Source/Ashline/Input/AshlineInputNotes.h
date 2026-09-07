#pragma once

#include "CoreMinimal.h"

/**
 * Input no longer requires editor-authored assets for the first playthrough.
 *
 * Runtime (always):
 *   UAshlineRuntimeInput builds IA_* + IMC_* in memory and
 *   AAshlinePlayerController adds the mapping contexts on possess.
 *
 * Optional editor assets (Scripts/create_ashline_play_assets.py):
 *   /Game/Ashline/Input/IMC_Ashline_KBM
 *   /Game/Ashline/Input/IMC_Ashline_Gamepad
 *   /Game/Ashline/Input/IMC_Ashline_Touch
 *   /Game/Ashline/Input/IA_Move
 *   /Game/Ashline/Input/IA_Look
 *   /Game/Ashline/Input/IA_Jump
 *   /Game/Ashline/Input/IA_Fire
 *   /Game/Ashline/Input/IA_Aim
 *   /Game/Ashline/Input/IA_Reload
 *   /Game/Ashline/Input/IA_CameraToggle
 *   /Game/Ashline/Input/IA_SwapWeapon
 *   /Game/Ashline/Input/IA_Crouch
 *
 * If those objects are assigned on the character/controller, they win.
 * Otherwise the runtime objects are used. Bindings: Docs/CONTROLS.md.
 */
namespace AshlineInput
{
	inline constexpr const TCHAR* KeyboardMouseContext = TEXT("IMC_Ashline_KBM");
	inline constexpr const TCHAR* GamepadContext = TEXT("IMC_Ashline_Gamepad");
	inline constexpr const TCHAR* TouchContext = TEXT("IMC_Ashline_Touch");
}
