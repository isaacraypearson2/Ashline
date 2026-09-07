#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Templates/Function.h"
#include "AshlineTypes.h"
#include "AshlineGameMode.generated.h"

class AAshlineGrayboxBuilder;
class AAshlineCharacter;
class APlayerController;

UCLASS()
class ASHLINE_API AAshlineGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAshlineGameMode();

	virtual void StartPlay() override;
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void StartMission(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void DeployMission(EAshlineMissionId MissionId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void ReturnToFrontend();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void CompleteActiveMission(int32 Stars, bool bOptionalComplete);

	UFUNCTION(BlueprintPure, Category = "Ashline|Campaign")
	EAshlineMissionId GetActiveMission() const { return ActiveMission; }

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void SetObjectiveComplete(FName ObjectiveId, bool bComplete = true);

	UFUNCTION(BlueprintPure, Category = "Ashline|Campaign")
	bool AreRequiredObjectivesComplete() const;

	UFUNCTION(BlueprintPure, Category = "Ashline|Campaign")
	int32 EvaluateCompletionStars() const { return ComputeStars(); }

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void MenuMove(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void MenuConfirm();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void MenuBack();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void MenuCycleDifficulty(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void RespawnPlayer(AAshlineCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Campaign")
	void UnlockAllMissions();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	EAshlineMissionId ActiveMission = EAshlineMissionId::ASH_01_WireCut;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Campaign")
	FAshlineMissionDefinition ActiveDefinition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Campaign")
	EAshlinePlayPhase PlayPhase = EAshlinePlayPhase::Frontend;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Campaign")
	int32 SelectedMissionIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Campaign")
	int32 LastAwardedXP = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Campaign")
	int32 LastAwardedStars = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Campaign")
	int32 LastAwardedCrates = 0;

protected:
	AAshlineGrayboxBuilder* EnsureBuilder();
	void ApplyFrontendView(APlayerController* PC);
	void ApplyMissionView(APlayerController* PC);
	void SelectDefaultMission();
	int32 ComputeStars() const;
	bool IsMissionDeployable(EAshlineMissionId MissionId) const;
	void ForEachLocalController(TFunctionRef<void(APlayerController*)> Fn);

	UPROPERTY()
	TObjectPtr<AAshlineGrayboxBuilder> Builder;
};
