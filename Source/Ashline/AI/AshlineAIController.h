#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AshlineTypes.h"
#include "AshlineAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class AAshlineCoverPoint;
class AAshlineAICharacter;

UCLASS()
class ASHLINE_API AAshlineAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAshlineAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|AI")
	void NotifyTookDamage(AActor* InstigatorActor, float Damage);

	UFUNCTION(BlueprintCallable, Category = "Ashline|AI")
	void NotifyDied();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|AI")
	TObjectPtr<UAIPerceptionComponent> Perception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|AI")
	EAshlineAICombatState CombatState = EAshlineAICombatState::Idle;

protected:
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	void TickCombat(float DeltaSeconds, AAshlineAICharacter* Bot);
	void TryAcquireTarget();
	void MoveToward(const FVector& Location, float Scale);
	void FireBurst(AAshlineAICharacter* Bot);
	void SeekCover(AAshlineAICharacter* Bot);
	void SeekFlank(AAshlineAICharacter* Bot);
	bool HasLineOfSightTo(AActor* Target) const;
	float SquadAccuracyBonus() const;
	void VacateCover();

	UPROPERTY()
	TObjectPtr<AActor> FocusTarget;

	UPROPERTY()
	TObjectPtr<AAshlineCoverPoint> ClaimedCover;

	FVector LastKnownLocation = FVector::ZeroVector;
	FVector InvestigateLocation = FVector::ZeroVector;
	FVector FlankLocation = FVector::ZeroVector;

	float BurstCooldown = 0.f;
	int32 ShotsLeftInBurst = 0;
	float ShotInterval = 0.12f;
	float ShotTimer = 0.f;
	float CoverReevaluate = 0.f;
	float LastHeardTime = -100.f;
	float LastDamageTime = -100.f;
	float StateAge = 0.f;
	bool bSuppressed = false;
};
