#pragma once

#include "CoreMinimal.h"

/**
 * Enhanced Input asset names expected after the first editor pass:
 *
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
 * Bindings are documented in Docs/CONTROLS.md and Config/DefaultInput.ini.
 */
namespace AshlineInput
{
	inline constexpr const TCHAR* KeyboardMouseContext = TEXT("IMC_Ashline_KBM");
	inline constexpr const TCHAR* GamepadContext = TEXT("IMC_Ashline_Gamepad");
	inline constexpr const TCHAR* TouchContext = TEXT("IMC_Ashline_Touch");
}
