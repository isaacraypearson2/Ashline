#include "Input/AshlineRuntimeInput.h"

#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

namespace
{
	UInputModifierSwizzleAxis* MakeSwizzleY(UObject* Outer)
	{
		UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(Outer);
		Swizzle->Order = EInputAxisSwizzle::YXZ;
		return Swizzle;
	}

	UInputModifierNegate* MakeNegate(UObject* Outer, bool bX, bool bY, bool bZ)
	{
		UInputModifierNegate* Neg = NewObject<UInputModifierNegate>(Outer);
		Neg->bX = bX;
		Neg->bY = bY;
		Neg->bZ = bZ;
		return Neg;
	}

	UInputModifierScalar* MakeScalar(UObject* Outer, const FVector& Value)
	{
		UInputModifierScalar* Scalar = NewObject<UInputModifierScalar>(Outer);
		Scalar->Scalar = Value;
		return Scalar;
	}

	UInputModifierDeadZone* MakeDeadZone(UObject* Outer, float Lower)
	{
		UInputModifierDeadZone* DZ = NewObject<UInputModifierDeadZone>(Outer);
		DZ->LowerThreshold = Lower;
		DZ->UpperThreshold = 1.f;
		DZ->Type = EDeadZoneType::Radial;
		return DZ;
	}
}

void UAshlineRuntimeInput::EnsureBuilt()
{
	if (bBuilt && Move && KeyboardMouseContext)
	{
		return;
	}

	Move = MakeAction(TEXT("IA_Ashline_Move"), static_cast<uint8>(EInputActionValueType::Axis2D));
	Look = MakeAction(TEXT("IA_Ashline_Look"), static_cast<uint8>(EInputActionValueType::Axis2D));
	Jump = MakeAction(TEXT("IA_Ashline_Jump"), static_cast<uint8>(EInputActionValueType::Boolean));
	Fire = MakeAction(TEXT("IA_Ashline_Fire"), static_cast<uint8>(EInputActionValueType::Boolean));
	Aim = MakeAction(TEXT("IA_Ashline_Aim"), static_cast<uint8>(EInputActionValueType::Boolean));
	Reload = MakeAction(TEXT("IA_Ashline_Reload"), static_cast<uint8>(EInputActionValueType::Boolean));
	CameraToggle = MakeAction(TEXT("IA_Ashline_Camera"), static_cast<uint8>(EInputActionValueType::Boolean));
	SwapWeapon = MakeAction(TEXT("IA_Ashline_Swap"), static_cast<uint8>(EInputActionValueType::Boolean));
	Crouch = MakeAction(TEXT("IA_Ashline_Crouch"), static_cast<uint8>(EInputActionValueType::Boolean));
	Sprint = MakeAction(TEXT("IA_Ashline_Sprint"), static_cast<uint8>(EInputActionValueType::Boolean));

	KeyboardMouseContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Ashline_KBM_Runtime"));
	GamepadContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Ashline_Gamepad_Runtime"));
	TouchContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Ashline_Touch_Runtime"));

	MapWASD();
	MapLook();
	MapButtons();
	MapGamepad();
	bBuilt = true;
}

UInputAction* UAshlineRuntimeInput::MakeAction(const TCHAR* Name, uint8 ValueType)
{
	UInputAction* Action = NewObject<UInputAction>(this, Name);
	Action->ValueType = static_cast<EInputActionValueType>(ValueType);
	return Action;
}

void UAshlineRuntimeInput::MapWASD()
{
	FEnhancedActionKeyMapping& W = KeyboardMouseContext->MapKey(Move, EKeys::W);
	W.Modifiers.Add(MakeSwizzleY(KeyboardMouseContext));

	FEnhancedActionKeyMapping& S = KeyboardMouseContext->MapKey(Move, EKeys::S);
	S.Modifiers.Add(MakeSwizzleY(KeyboardMouseContext));
	S.Modifiers.Add(MakeNegate(KeyboardMouseContext, true, true, true));

	KeyboardMouseContext->MapKey(Move, EKeys::D);

	FEnhancedActionKeyMapping& A = KeyboardMouseContext->MapKey(Move, EKeys::A);
	A.Modifiers.Add(MakeNegate(KeyboardMouseContext, true, false, false));
}

void UAshlineRuntimeInput::MapLook()
{
	FEnhancedActionKeyMapping& Mouse = KeyboardMouseContext->MapKey(Look, EKeys::Mouse2D);
	Mouse.Modifiers.Add(MakeScalar(KeyboardMouseContext, FVector(0.12f, 0.12f, 1.f)));
	Mouse.Modifiers.Add(MakeNegate(KeyboardMouseContext, false, true, false));
}

void UAshlineRuntimeInput::MapButtons()
{
	KeyboardMouseContext->MapKey(Jump, EKeys::SpaceBar);
	KeyboardMouseContext->MapKey(Fire, EKeys::LeftMouseButton);
	KeyboardMouseContext->MapKey(Aim, EKeys::RightMouseButton);
	KeyboardMouseContext->MapKey(Reload, EKeys::R);
	KeyboardMouseContext->MapKey(CameraToggle, EKeys::V);
	KeyboardMouseContext->MapKey(SwapWeapon, EKeys::Q);
	KeyboardMouseContext->MapKey(SwapWeapon, EKeys::One);
	KeyboardMouseContext->MapKey(SwapWeapon, EKeys::Two);
	KeyboardMouseContext->MapKey(Crouch, EKeys::C);
	KeyboardMouseContext->MapKey(Crouch, EKeys::LeftControl);
	KeyboardMouseContext->MapKey(Sprint, EKeys::LeftShift);
}

void UAshlineRuntimeInput::MapGamepad()
{
	FEnhancedActionKeyMapping& LeftStick = GamepadContext->MapKey(Move, EKeys::Gamepad_Left2D);
	LeftStick.Modifiers.Add(MakeDeadZone(GamepadContext, 0.20f));

	FEnhancedActionKeyMapping& RightStick = GamepadContext->MapKey(Look, EKeys::Gamepad_Right2D);
	RightStick.Modifiers.Add(MakeDeadZone(GamepadContext, 0.18f));
	RightStick.Modifiers.Add(MakeScalar(GamepadContext, FVector(0.92f, 0.92f, 1.f)));
	RightStick.Modifiers.Add(MakeNegate(GamepadContext, false, true, false));

	GamepadContext->MapKey(Fire, EKeys::Gamepad_RightTrigger);
	GamepadContext->MapKey(Aim, EKeys::Gamepad_LeftTrigger);
	GamepadContext->MapKey(Jump, EKeys::Gamepad_FaceButton_Bottom);
	GamepadContext->MapKey(Crouch, EKeys::Gamepad_FaceButton_Right);
	GamepadContext->MapKey(Reload, EKeys::Gamepad_FaceButton_Left);
	GamepadContext->MapKey(SwapWeapon, EKeys::Gamepad_FaceButton_Top);
	GamepadContext->MapKey(SwapWeapon, EKeys::Gamepad_LeftShoulder);
	GamepadContext->MapKey(Sprint, EKeys::Gamepad_LeftThumbstick);
	GamepadContext->MapKey(CameraToggle, EKeys::Gamepad_Special_Left);
	GamepadContext->MapKey(CameraToggle, EKeys::Gamepad_DPad_Up);
	GamepadContext->MapKey(CameraToggle, EKeys::Gamepad_RightThumbstick);

	// Touch context mirrors fire/aim/jump so a later UMG HUD can inject the same actions.
	TouchContext->MapKey(Fire, EKeys::LeftMouseButton);
	TouchContext->MapKey(Aim, EKeys::RightMouseButton);
	TouchContext->MapKey(Jump, EKeys::SpaceBar);
	TouchContext->MapKey(CameraToggle, EKeys::V);
}
