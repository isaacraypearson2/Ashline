#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AshlinePlayerController.generated.h"

class UInputMappingContext;
class UAshlineTouchHUD;

UCLASS()
class ASHLINE_API AAshlinePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAshlinePlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputMappingContext> KeyboardMouseContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputMappingContext> GamepadContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputMappingContext> TouchContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|UI")
	TSubclassOf<UAshlineTouchHUD> TouchHUDClass;

protected:
	void ApplyPlatformMappings();

	UPROPERTY()
	TObjectPtr<UAshlineTouchHUD> TouchHUD;
};
