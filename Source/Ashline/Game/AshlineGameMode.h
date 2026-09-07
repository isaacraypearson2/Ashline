#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AshlineTypes.h"
#include "AshlineGameMode.generated.h"

UCLASS()
class ASHLINE_API AAshlineGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAshlineGameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void StartMission(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void CompleteActiveMission(int32 Stars, bool bOptionalComplete);

	UFUNCTION(BlueprintPure, Category = "Ashline|Campaign")
	EAshlineMissionId GetActiveMission() const { return ActiveMission; }

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void SetObjectiveComplete(FName ObjectiveId, bool bComplete = true);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	EAshlineMissionId ActiveMission = EAshlineMissionId::ASH_01_WireCut;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Campaign")
	FAshlineMissionDefinition ActiveDefinition;
};
