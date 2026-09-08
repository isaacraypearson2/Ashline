#include "Campaign/AshlineObjectiveTrigger.h"

#include "Components/BoxComponent.h"
#include "Game/AshlineGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AshlineCharacter.h"

AAshlineObjectiveTrigger::AAshlineObjectiveTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	SetRootComponent(Volume);
	Volume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Volume->SetBoxExtent(FVector(200.f, 200.f, 160.f));
	Volume->OnComponentBeginOverlap.AddDynamic(this, &AAshlineObjectiveTrigger::OnOverlap);
}

void AAshlineObjectiveTrigger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	(void)OverlappedComponent;
	(void)OtherComp;
	(void)OtherBodyIndex;
	(void)bFromSweep;
	(void)SweepResult;
	CompleteFromInteract(OtherActor);
}

void AAshlineObjectiveTrigger::CompleteFromInteract(AActor* InstigatorActor)
{
	if (bConsumed || !Cast<AAshlineCharacter>(InstigatorActor))
	{
		return;
	}

	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->SetObjectiveComplete(ObjectiveId, true);
		if (bCompleteMissionOnEnter)
		{
			const bool bOptional = GameMode->AreRequiredObjectivesComplete();
			const int32 Stars = GameMode->EvaluateCompletionStars();
			GameMode->CompleteActiveMission(Stars > 0 ? Stars : StarsIfMissionComplete, bOptional);
		}
	}
	bConsumed = true;
}

FString AAshlineObjectiveTrigger::GetPromptText() const
{
	if (bCompleteMissionOnEnter)
	{
		return TEXT("EXTRACT");
	}
	return ObjectiveId.IsNone() ? TEXT("INTERACT") : ObjectiveId.ToString();
}
