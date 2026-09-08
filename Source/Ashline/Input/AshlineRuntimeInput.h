#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AshlineRuntimeInput.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * Builds Enhanced Input actions + mapping contexts in memory so keyboard/mouse
 * (and DualSense) work without hand-authored IA/IMC .uasset files.
 */
UCLASS()
class ASHLINE_API UAshlineRuntimeInput : public UObject
{
	GENERATED_BODY()

public:
	void EnsureBuilt();

	UPROPERTY()
	TObjectPtr<UInputAction> Move;

	UPROPERTY()
	TObjectPtr<UInputAction> Look;

	UPROPERTY()
	TObjectPtr<UInputAction> Jump;

	UPROPERTY()
	TObjectPtr<UInputAction> Fire;

	UPROPERTY()
	TObjectPtr<UInputAction> Aim;

	UPROPERTY()
	TObjectPtr<UInputAction> Reload;

	UPROPERTY()
	TObjectPtr<UInputAction> CameraToggle;

	UPROPERTY()
	TObjectPtr<UInputAction> SwapWeapon;

	UPROPERTY()
	TObjectPtr<UInputAction> Crouch;

	UPROPERTY()
	TObjectPtr<UInputAction> Interact;

	UPROPERTY()
	TObjectPtr<UInputAction> Sprint;

	UPROPERTY()
	TObjectPtr<UInputAction> FireMode;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> KeyboardMouseContext;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> GamepadContext;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> TouchContext;

	bool bBuilt = false;

private:
	UInputAction* MakeAction(const TCHAR* Name, uint8 ValueType);
	void MapWASD();
	void MapLook();
	void MapButtons();
	void MapGamepad();
};
