#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AshlineHUD.generated.h"

class AAshlineGameMode;
class UAshlineSaveGame;

UCLASS()
class ASHLINE_API AAshlineHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|HUD")
	bool bDrawDebugCombatHUD = true;

protected:
	void DrawFrontend(AAshlineGameMode* GameMode, UAshlineSaveGame* Save);
	void DrawCombat(AAshlineGameMode* GameMode);
	void DrawMissionComplete(AAshlineGameMode* GameMode);
	void DrawPause();
	void DrawTextLine(const FString& Text, float X, float Y, const FLinearColor& Color);
};
