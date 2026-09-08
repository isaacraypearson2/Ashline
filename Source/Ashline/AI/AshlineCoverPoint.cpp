#include "AI/AshlineCoverPoint.h"

#include "EngineUtils.h"
#include "Engine/World.h"

AAshlineCoverPoint::AAshlineCoverPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	SetCanBeDamaged(false);
	bCollideWhenPlacing = false;
}

bool AAshlineCoverPoint::IsOccupied() const
{
	return Occupant.IsValid();
}

void AAshlineCoverPoint::Occupy(AActor* NewOccupant)
{
	Occupant = NewOccupant;
}

void AAshlineCoverPoint::Vacate(AActor* Who)
{
	if (!Who || Occupant.Get() == Who)
	{
		Occupant = nullptr;
	}
}

TArray<AAshlineCoverPoint*> AAshlineCoverPoint::FindNearby(UWorld* World, const FVector& Origin, float Radius)
{
	TArray<AAshlineCoverPoint*> Out;
	if (!World)
	{
		return Out;
	}
	const float RadiusSq = Radius * Radius;
	for (TActorIterator<AAshlineCoverPoint> It(World); It; ++It)
	{
		AAshlineCoverPoint* Point = *It;
		if (Point && FVector::DistSquared(Point->GetActorLocation(), Origin) <= RadiusSq)
		{
			Out.Add(Point);
		}
	}
	return Out;
}

AAshlineCoverPoint* AAshlineCoverPoint::PickBest(
	UWorld* World,
	const FVector& Origin,
	const FVector& ThreatLocation,
	AActor* Claimant,
	float SearchRadius)
{
	AAshlineCoverPoint* Best = nullptr;
	float BestScore = -MAX_FLT;
	for (AAshlineCoverPoint* Point : FindNearby(World, Origin, SearchRadius))
	{
		if (!Point)
		{
			continue;
		}
		if (Point->IsOccupied() && Point->Occupant.Get() != Claimant)
		{
			continue;
		}

		const FVector Loc = Point->GetActorLocation();
		const float DistToSelf = FVector::Dist(Origin, Loc);
		const float DistToThreat = FVector::Dist(ThreatLocation, Loc);
		const FVector ToThreat = (ThreatLocation - Loc).GetSafeNormal();
		const FVector ToSelf = (Origin - Loc).GetSafeNormal();
		const float SideDot = FVector::DotProduct(ToThreat, ToSelf);

		float Score = DistToThreat * 0.35f - DistToSelf - (SideDot > 0.35f ? 400.f : 0.f);
		if (Point->bCrouchCover)
		{
			Score += 80.f;
		}
		if (Score > BestScore)
		{
			BestScore = Score;
			Best = Point;
		}
	}
	return Best;
}
