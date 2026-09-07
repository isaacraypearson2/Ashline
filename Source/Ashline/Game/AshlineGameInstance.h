#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AshlineGameInstance.generated.h"

UCLASS()
class ASHLINE_API UAshlineGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Ashline")
	void OpenFrontend();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void TravelToMission(FName MapAsset);
};
