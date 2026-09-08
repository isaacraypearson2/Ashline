#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AshlineCoverPoint.generated.h"

/**
 * Lightweight cover marker. GrayboxBuilder::Cover() drops one next to each cover box
 * without changing layout numbers. AI occupies these; authored maps can place them too.
 */
UCLASS()
class ASHLINE_API AAshlineCoverPoint : public AActor
{
	GENERATED_BODY()

public:
	AAshlineCoverPoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Cover")
	bool bCrouchCover = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Cover")
	float OccupiedRadius = 80.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Cover")
	TWeakObjectPtr<AActor> Occupant;

	UFUNCTION(BlueprintPure, Category = "Ashline|Cover")
	bool IsOccupied() const;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Cover")
	void Occupy(AActor* NewOccupant);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Cover")
	void Vacate(AActor* Who);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Cover")
	static TArray<AAshlineCoverPoint*> FindNearby(UWorld* World, const FVector& Origin, float Radius);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Cover")
	static AAshlineCoverPoint* PickBest(
		UWorld* World,
		const FVector& Origin,
		const FVector& ThreatLocation,
		AActor* Claimant,
		float SearchRadius = 1800.f);
};
