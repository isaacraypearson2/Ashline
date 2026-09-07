#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AshlineHUD.generated.h"

UCLASS()
class ASHLINE_API AAshlineHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|HUD")
	bool bDrawDebugCombatHUD = true;
};
