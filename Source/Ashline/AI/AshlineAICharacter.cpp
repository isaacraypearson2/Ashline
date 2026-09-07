#include "AI/AshlineAICharacter.h"

#include "AI/AshlineAICatalog.h"
#include "Engine/DamageEvents.h"
#include "Engine/GameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Progression/AshlineProgressionSubsystem.h"

AAshlineAICharacter::AAshlineAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCharacterMovement()->MaxWalkSpeed = 380.f;
}

void AAshlineAICharacter::BeginPlay()
{
	Super::BeginPlay();

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
	ApplyArchetype(Archetype, Difficulty);
}

void AAshlineAICharacter::ApplyArchetype(EAshlineAIArchetype InArchetype, EAshlineDifficulty Difficulty)
{
	Archetype = InArchetype;
	ArchetypeDef = UAshlineAICatalog::GetArchetype(Archetype);
	const FAshlineDifficultyTuning Tuning = UAshlineAICatalog::GetDifficulty(Difficulty);
	Health = ArchetypeDef.MaxHealth * Tuning.AIHealthMul;
	GetCharacterMovement()->MaxWalkSpeed = ArchetypeDef.MoveSpeed;
}

float AAshlineAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bDead)
	{
		return 0.f;
	}

	float Incoming = Applied > 0.f ? Applied : DamageAmount;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				Incoming *= UAshlineAICatalog::GetDifficulty(Save->Difficulty).PlayerDamageDealtMul;
			}
		}
	}

	Health -= Incoming;
	if (Health <= 0.f)
	{
		bDead = true;
		Health = 0.f;
		DetachFromControllerPendingDestroy();
		SetLifeSpan(8.f);
	}
	return Incoming;
}
