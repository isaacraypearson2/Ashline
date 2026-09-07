#include "Player/AshlinePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "UI/AshlineTouchHUD.h"

AAshlinePlayerController::AAshlinePlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableTouchEvents = true;
}

void AAshlinePlayerController::BeginPlay()
{
	Super::BeginPlay();
	ApplyPlatformMappings();

#if PLATFORM_IOS || PLATFORM_TVOS
	if (TouchHUDClass)
	{
		TouchHUD = CreateWidget<UAshlineTouchHUD>(this, TouchHUDClass);
		if (TouchHUD)
		{
			TouchHUD->AddToViewport();
		}
	}
#endif
}

void AAshlinePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	ApplyPlatformMappings();
}

void AAshlinePlayerController::ApplyPlatformMappings()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (KeyboardMouseContext)
			{
				Subsystem->AddMappingContext(KeyboardMouseContext, 0);
			}
#if PLATFORM_MAC || PLATFORM_IOS
			if (GamepadContext)
			{
				Subsystem->AddMappingContext(GamepadContext, 1);
			}
#endif
#if PLATFORM_IOS || PLATFORM_TVOS
			if (TouchContext)
			{
				Subsystem->AddMappingContext(TouchContext, 2);
			}
#endif
		}
	}
}
