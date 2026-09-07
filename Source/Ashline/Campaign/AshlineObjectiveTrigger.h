#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AshlineObjectiveTrigger.generated.h"

class UBoxComponent;

/** Place in graybox maps to complete a catalog objective when the player enters. */
UCLASS()
class ASHLINE_API AAshlineObjectiveTrigger : public AActor
{
	GENERATED_BODY()

public:
	AAshlineObjectiveTrigger();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Campaign")
	TObjectPtr<UBoxComponent> Volume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	FName ObjectiveId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	bool bCompleteMissionOnEnter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Campaign")
	int32 StarsIfMissionComplete = 2;

protected:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bConsumed = false;
};
