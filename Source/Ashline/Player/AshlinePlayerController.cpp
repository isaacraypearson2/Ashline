#include "Player/AshlinePlayerController.h"

#include "Ashline.h"
#include "Blueprint/UserWidget.h"
#include "Campaign/AshlineMissionCatalog.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Game/AshlineGameMode.h"
#include "Input/AshlineRuntimeInput.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/AshlineGraphicsSettings.h"
#include "Meta/AshlineMetaCatalog.h"
#include "Progression/AshlineProgressionSubsystem.h"
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
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_IOS
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
	BindPaused(EKeys::Gamepad_FaceButton_Bottom, &AAshlinePlayerController::MenuConfirm);
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

void AAshlinePlayerController::AshPCUltra()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineGraphicsSettings* Graphics = GI->GetSubsystem<UAshlineGraphicsSettings>())
		{
			Graphics->ApplyPreset(EAshlineGraphicsPreset::PC_Ultra);
		}
	}
}

void AAshlinePlayerController::AshPCBalanced()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineGraphicsSettings* Graphics = GI->GetSubsystem<UAshlineGraphicsSettings>())
		{
			Graphics->ApplyPreset(EAshlineGraphicsPreset::PC_Balanced);
		}
	}
}

void AAshlinePlayerController::AshDeck()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineGraphicsSettings* Graphics = GI->GetSubsystem<UAshlineGraphicsSettings>())
		{
			Graphics->ApplyPreset(EAshlineGraphicsPreset::SteamDeck);
		}
	}
}

namespace
{
	UAshlineProgressionSubsystem* AshProgression(const AAshlinePlayerController* PC)
	{
		if (!PC)
		{
			return nullptr;
		}
		if (UGameInstance* GI = PC->GetGameInstance())
		{
			return GI->GetSubsystem<UAshlineProgressionSubsystem>();
		}
		return nullptr;
	}
}

void AAshlinePlayerController::AshGrantCredits(int32 Amount)
{
	if (UAshlineProgressionSubsystem* Progression = AshProgression(this))
	{
		const int32 Grant = Amount > 0 ? Amount : 1000;
		Progression->GrantCredits(Grant);
		UE_LOG(LogAshline, Log, TEXT("AshGrantCredits: +%d now %d"), Grant, Progression->GetCredits());
	}
}

void AAshlinePlayerController::AshSetRank(int32 Rank)
{
	if (UAshlineProgressionSubsystem* Progression = AshProgression(this))
	{
		Progression->SetRank(Rank);
		if (UAshlineSaveGame* Save = Progression->GetSave())
		{
			UE_LOG(LogAshline, Log, TEXT("AshSetRank: rank=%d prestige=%d"), Save->Operator.Rank, Save->PrestigeLevel);
		}
	}
}

void AAshlinePlayerController::AshPrestige()
{
	if (UAshlineProgressionSubsystem* Progression = AshProgression(this))
	{
		if (Progression->PrestigeReset())
		{
			UE_LOG(LogAshline, Log, TEXT("AshPrestige: prestige=%d gold skin + prestige camo equipped."),
				Progression->GetSave() ? Progression->GetSave()->PrestigeLevel : 0);
		}
		else
		{
			UE_LOG(LogAshline, Warning, TEXT("AshPrestige: need rank 50 (use AshSetRank 50)."));
		}
	}
}

void AAshlinePlayerController::AshOpenCrate()
{
	if (UAshlineProgressionSubsystem* Progression = AshProgression(this))
	{
		const FAshlineCrateGrant Grant = Progression->OpenPlayEarnedCrate();
		UE_LOG(LogAshline, Log, TEXT("AshOpenCrate: %s (%s)"), *Grant.DisplayName.ToString(), *Grant.ItemId.ToString());
	}
}

void AAshlinePlayerController::AshBuySkin(const FString& SkinId)
{
	if (UAshlineProgressionSubsystem* Progression = AshProgression(this))
	{
		const bool bOk = Progression->PurchaseSkin(FName(*SkinId));
		UE_LOG(LogAshline, Log, TEXT("AshBuySkin %s -> %s (credits %d)"), *SkinId, bOk ? TEXT("ok") : TEXT("fail"), Progression->GetCredits());
	}
}

void AAshlinePlayerController::AshEquipSkin(const FString& WeaponId, const FString& SkinId)
{
	if (UAshlineProgressionSubsystem* Progression = AshProgression(this))
	{
		const bool bOk = Progression->EquipSkin(FName(*WeaponId), FName(*SkinId));
		UE_LOG(LogAshline, Log, TEXT("AshEquipSkin %s on %s -> %s"), *SkinId, *WeaponId, bOk ? TEXT("ok") : TEXT("fail"));
	}
}

void AAshlinePlayerController::AshBuyCosmetic(const FString& CosmeticId)
{
	if (UAshlineProgressionSubsystem* Progression = AshProgression(this))
	{
		const bool bOk = Progression->PurchaseCosmetic(FName(*CosmeticId));
		UE_LOG(LogAshline, Log, TEXT("AshBuyCosmetic %s -> %s (credits %d)"), *CosmeticId, bOk ? TEXT("ok") : TEXT("fail"), Progression->GetCredits());
	}
}

void AAshlinePlayerController::AshEquipCosmetic(const FString& SlotName, const FString& CosmeticId)
{
	EAshlineCosmeticSlot Slot = EAshlineCosmeticSlot::Camo;
	if (!UAshlineMetaCatalog::SlotFromName(FName(*SlotName), Slot))
	{
		UE_LOG(LogAshline, Warning, TEXT("AshEquipCosmetic: unknown slot %s"), *SlotName);
		return;
	}
	if (UAshlineProgressionSubsystem* Progression = AshProgression(this))
	{
		const bool bOk = Progression->EquipCosmetic(Slot, FName(*CosmeticId));
		UE_LOG(LogAshline, Log, TEXT("AshEquipCosmetic %s in %s -> %s"), *CosmeticId, *SlotName, bOk ? TEXT("ok") : TEXT("fail"));
	}
}

void AAshlinePlayerController::AshUnlockMeta()
{
	if (UAshlineProgressionSubsystem* Progression = AshProgression(this))
	{
		Progression->UnlockAllMeta();
		UE_LOG(LogAshline, Log, TEXT("AshUnlockMeta: catalog owned (prestige-gated items still require prestige). Credits=%d"), Progression->GetCredits());
	}
}

void AAshlinePlayerController::AshListMeta()
{
	UE_LOG(LogAshline, Log, TEXT("=== Ashline cosmetics ==="));
	for (const FAshlineCosmeticDefinition& Item : UAshlineMetaCatalog::BuildCosmetics())
	{
		UE_LOG(LogAshline, Log, TEXT("  %s  %s  rank%d  %dcr  P%d"),
			*Item.CosmeticId.ToString(), *Item.DisplayName.ToString(), Item.UnlockRank, Item.CreditCost, Item.RequiredPrestige);
	}
	UE_LOG(LogAshline, Log, TEXT("=== Ashline weapon skins ==="));
	for (const FAshlineWeaponSkinDefinition& Item : UAshlineMetaCatalog::BuildWeaponSkins())
	{
		UE_LOG(LogAshline, Log, TEXT("  %s  %s  weapon=%s  %dcr"),
			*Item.SkinId.ToString(), *Item.DisplayName.ToString(),
			Item.WeaponId.IsNone() ? TEXT("*") : *Item.WeaponId.ToString(), Item.CreditCost);
	}
}
