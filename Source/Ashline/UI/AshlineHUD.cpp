#include "UI/AshlineHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Game/AshlineGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AshlineCharacter.h"
#include "Weapons/AshlineWeaponComponent.h"

void AAshlineHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!bDrawDebugCombatHUD || !Canvas)
	{
		return;
	}

	AAshlineCharacter* Character = Cast<AAshlineCharacter>(GetOwningPawn());
	if (!Character || !Character->WeaponComponent)
	{
		return;
	}

	const FAshlineRuntimeWeapon& Weapon = Character->WeaponComponent->GetActiveWeapon();
	const FString WeaponLine = FString::Printf(
		TEXT("%s  %d / %d"),
		*Weapon.Definition.DisplayName.ToString(),
		Weapon.AmmoInMag,
		Weapon.Reserve);

	FCanvasTextItem WeaponItem(FVector2D(48.f, Canvas->SizeY - 84.f), FText::FromString(WeaponLine), GEngine->GetSmallFont(), FLinearColor::White);
	WeaponItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(WeaponItem);

	const FString Mode = Character->GetCameraMode() == EAshlineCameraMode::FirstPerson ? TEXT("FPS") : TEXT("TPS");
	FCanvasTextItem ModeItem(FVector2D(48.f, Canvas->SizeY - 60.f), FText::FromString(Mode), GEngine->GetSmallFont(), FLinearColor(0.8f, 0.85f, 0.7f));
	Canvas->DrawItem(ModeItem);

	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		FCanvasTextItem MissionItem(
			FVector2D(48.f, 36.f),
			FText::FromString(GameMode->ActiveDefinition.CodeName + TEXT("  ") + GameMode->ActiveDefinition.Title.ToString()),
			GEngine->GetSmallFont(),
			FLinearColor(0.95f, 0.86f, 0.55f));
		Canvas->DrawItem(MissionItem);
	}

	const float CX = Canvas->SizeX * 0.5f;
	const float CY = Canvas->SizeY * 0.5f;
	DrawLine(CX - 8.f, CY, CX + 8.f, CY, FLinearColor::White, 1.f);
	DrawLine(CX, CY - 8.f, CX, CY + 8.f, FLinearColor::White, 1.f);
}
