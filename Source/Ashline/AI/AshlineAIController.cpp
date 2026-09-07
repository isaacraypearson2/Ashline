#include "AI/AshlineAIController.h"

#include "AI/AshlineAICatalog.h"
#include "AI/AshlineAICharacter.h"
#include "Engine/GameInstance.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/AshlineCharacter.h"
#include "Progression/AshlineProgressionSubsystem.h"

AAshlineAIController::AAshlineAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	SetPerceptionComponent(*Perception);

	UAISenseConfig_Sight* Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	Sight->SightRadius = 3500.f;
	Sight->LoseSightRadius = 4000.f;
	Sight->PeripheralVisionAngleDegrees = 70.f;
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = false;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;
	Perception->ConfigureSense(*Sight);
	Perception->SetDominantSense(Sight->GetSenseImplementation());
	Perception->OnPerceptionUpdated.AddDynamic(this, &AAshlineAIController::OnPerceptionUpdated);
}

void AAshlineAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AAshlineAICharacter* Bot = Cast<AAshlineAICharacter>(InPawn))
	{
		if (UAISenseConfig_Sight* Sight = Cast<UAISenseConfig_Sight>(Perception->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>())))
		{
			EAshlineDifficulty Difficulty = EAshlineDifficulty::Regular;
			if (UGameInstance* GI = GetGameInstance())
			{
				if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
				{
					if (UAshlineSaveGame* Save = Progression->GetSave())
					{
						Difficulty = Save->Difficulty;
					}
				}
			}
			const FAshlineDifficultyTuning Tuning = UAshlineAICatalog::GetDifficulty(Difficulty);
			Sight->SightRadius = Bot->ArchetypeDef.SightRange * Tuning.AIPerceptionRangeMul;
			Sight->LoseSightRadius = Sight->SightRadius + 600.f;
			Perception->RequestStimuliListenerUpdate();
		}
	}
}

void AAshlineAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	BurstCooldown = FMath::Max(0.f, BurstCooldown - DeltaSeconds);

	if (!FocusTarget || !GetPawn())
	{
		return;
	}

	SetFocus(FocusTarget);
	if (AAshlineAICharacter* Bot = Cast<AAshlineAICharacter>(GetPawn()))
	{
		if (Bot->bDead)
		{
			return;
		}
		if (!Bot->ArchetypeDef.bHoldsPosition)
		{
			MoveToActor(FocusTarget, Bot->ArchetypeDef.bFlanks ? 250.f : 420.f);
		}
	}
}

void AAshlineAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		if (Cast<AAshlineCharacter>(Actor))
		{
			FocusTarget = Actor;
			return;
		}
	}
}
