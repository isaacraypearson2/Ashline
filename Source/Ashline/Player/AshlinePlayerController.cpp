#include "Player/AshlinePlayerController.h"

#include "Ashline.h"
#include "Blueprint/UserWidget.h"
#include "Campaign/AshlineMissionCatalog.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Game/AshlineGameMode.h"
#include "Input/AshlineRuntimeInput.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
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
	GetOrCreateRuntimeInput();
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
	GetOrCreateRuntimeInput();
	ApplyPlatformMappings();
	BindMenuKeys();
}

UAshlineRuntimeInput* AAshlinePlayerController::GetOrCreateRuntimeInput()
{
	if (!RuntimeInput)
	{
		RuntimeInput = NewObject<UAshlineRuntimeInput>(this, TEXT("AshlineRuntimeInput"));
	}
	if (RuntimeInput)
	{
		RuntimeInput->EnsureBuilt();
		if (!KeyboardMouseContext)
		{
			KeyboardMouseContext = RuntimeInput->KeyboardMouseContext;
		}
		if (!GamepadContext)
		{
			GamepadContext = RuntimeInput->GamepadContext;
		}
		if (!TouchContext)
		{
			TouchContext = RuntimeInput->TouchContext;
		}
	}
	return RuntimeInput;
}

void AAshlinePlayerController::ApplyPlatformMappings()
{
	GetOrCreateRuntimeInput();

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

void AAshlinePlayerController::BindMenuKeys()
{
	if (!InputComponent)
	{
		return;
	}

	auto BindPaused = [this](const FKey& Key, void (AAshlinePlayerController::*Fn)())
	{
		FInputKeyBinding& Binding = InputComponent->BindKey(Key, IE_Pressed, this, Fn);
		Binding.bExecuteWhenPaused = true;
	};

	BindPaused(EKeys::Up, &AAshlinePlayerController::MenuUp);
	BindPaused(EKeys::Down, &AAshlinePlayerController::MenuDown);
	BindPaused(EKeys::Enter, &AAshlinePlayerController::MenuConfirm);
	BindPaused(EKeys::Escape, &AAshlinePlayerController::MenuBack);
	BindPaused(EKeys::Left, &AAshlinePlayerController::MenuLeft);
	BindPaused(EKeys::Right, &AAshlinePlayerController::MenuRight);
	BindPaused(EKeys::Gamepad_DPad_Up, &AAshlinePlayerController::MenuUp);
	BindPaused(EKeys::Gamepad_DPad_Down, &AAshlinePlayerController::MenuDown);
	BindPaused(EKeys::Gamepad_DPad_Left, &AAshlinePlayerController::MenuLeft);
	BindPaused(EKeys::Gamepad_DPad_Right, &AAshlinePlayerController::MenuRight);
	BindPaused(EKeys::Gamepad_Special_Right, &AAshlinePlayerController::MenuBack);
}

void AAshlinePlayerController::MenuUp()
{
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->MenuMove(-1);
	}
}

void AAshlinePlayerController::MenuDown()
{
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->MenuMove(1);
	}
}

void AAshlinePlayerController::MenuConfirm()
{
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->MenuConfirm();
	}
}

void AAshlinePlayerController::MenuBack()
{
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->MenuBack();
	}
}

void AAshlinePlayerController::MenuLeft()
{
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->MenuCycleDifficulty(-1);
	}
}

void AAshlinePlayerController::MenuRight()
{
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->MenuCycleDifficulty(1);
	}
}

void AAshlinePlayerController::AshUnlockAll()
{
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->UnlockAllMissions();
		UE_LOG(LogAshline, Log, TEXT("AshUnlockAll: every mission is Available or already complete."));
	}
}

void AAshlinePlayerController::AshDeploy(int32 MissionNumber)
{
	const int32 Index = FMath::Clamp(MissionNumber, 1, 12) - 1;
	const EAshlineMissionId Id = static_cast<EAshlineMissionId>(
		static_cast<uint8>(EAshlineMissionId::ASH_01_WireCut) + Index);
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->UnlockAllMissions();
		GameMode->DeployMission(Id);
	}
}

void AAshlinePlayerController::AshComplete()
{
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->CompleteActiveMission(GameMode->EvaluateCompletionStars(), true);
	}
}

void AAshlinePlayerController::AshFrontend()
{
	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->ReturnToFrontend();
	}
}
