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
	if (bConsumed || !Cast<AAshlineCharacter>(OtherActor))
	{
		return;
	}

	if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->SetObjectiveComplete(ObjectiveId, true);
		if (bCompleteMissionOnEnter)
		{
			GameMode->CompleteActiveMission(StarsIfMissionComplete, true);
		}
	}
	bConsumed = true;
}
