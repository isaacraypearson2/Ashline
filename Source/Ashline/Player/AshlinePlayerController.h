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
	void AshPCHigh();

	UFUNCTION(Exec)
	void AshPCBalanced();

	UFUNCTION(Exec)
	void AshPCPerf();

	UFUNCTION(Exec)
	void AshSteamDeck();

	UFUNCTION(Exec)
	void AshPCLow();

	UFUNCTION(Exec)
	void AshPCMed();

	UFUNCTION(Exec)
	void AshLaptop();

	UFUNCTION(Exec)
	void AshGfxAuto();

	UFUNCTION(Exec)
	void AshGfxCycle();

	UFUNCTION(Exec)
	void AshFPS(int32 Target);

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

	UFUNCTION(Exec)
	void AshBuyWeapon(const FString& WeaponId);

	UFUNCTION(Exec)
	void AshBuyAttachment(const FString& WeaponId, const FString& AttachmentId);

	UFUNCTION(Exec)
	void AshBuyEquipment(const FString& EquipmentId);

	UFUNCTION(Exec)
	void AshEquipEquipment(const FString& SlotName, const FString& EquipmentId);

	UFUNCTION(Exec)
	void AshListArmory();

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
