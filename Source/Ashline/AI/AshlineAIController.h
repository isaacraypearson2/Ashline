#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AshlineAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class ASHLINE_API AAshlineAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAshlineAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|AI")
	TObjectPtr<UAIPerceptionComponent> Perception;

protected:
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UPROPERTY()
	TObjectPtr<AActor> FocusTarget;

	float BurstCooldown = 0.f;
};
