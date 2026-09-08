#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AshlinePlayerController.generated.h"

class UInputMappingContext;
class UAshlineTouchHUD;
class UAshlineRuntimeInput;

UCLASS()
class ASHLINE_API AAshlinePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAshlinePlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Input")
	UAshlineRuntimeInput* GetOrCreateRuntimeInput();

	UFUNCTION(Exec)
	void AshUnlockAll();

	UFUNCTION(Exec)
	void AshDeploy(int32 MissionNumber);

	UFUNCTION(Exec)
	void AshComplete();

	UFUNCTION(Exec)
	void AshFrontend();

	UFUNCTION(Exec)
	void AshPCUltra();

	UFUNCTION(Exec)
	void AshPCBalanced();

	UFUNCTION(Exec)
	void AshDeck();

	UFUNCTION(Exec)
	void AshGrantCredits(int32 Amount);

	UFUNCTION(Exec)
	void AshSetRank(int32 Rank);

	UFUNCTION(Exec)
	void AshPrestige();

	UFUNCTION(Exec)
	void AshOpenCrate();

	UFUNCTION(Exec)
	void AshBuySkin(const FString& SkinId);

	UFUNCTION(Exec)
	void AshEquipSkin(const FString& WeaponId, const FString& SkinId);

	UFUNCTION(Exec)
	void AshBuyCosmetic(const FString& CosmeticId);

	UFUNCTION(Exec)
	void AshEquipCosmetic(const FString& SlotName, const FString& CosmeticId);

	UFUNCTION(Exec)
	void AshUnlockMeta();

	UFUNCTION(Exec)
	void AshListMeta();

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
	void BindMenuKeys();
	void MenuUp();
	void MenuDown();
	void MenuConfirm();
	void MenuBack();
	void MenuLeft();
	void MenuRight();

	UPROPERTY()
	TObjectPtr<UAshlineTouchHUD> TouchHUD;

	UPROPERTY()
	TObjectPtr<UAshlineRuntimeInput> RuntimeInput;
};
