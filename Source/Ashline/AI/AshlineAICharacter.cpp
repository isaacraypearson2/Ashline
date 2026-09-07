#include "AI/AshlineAICharacter.h"

#include "AI/AshlineAICatalog.h"
#include "AI/AshlineAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/GameInstance.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Progression/AshlineProgressionSubsystem.h"

AAshlineAICharacter::AAshlineAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAshlineAIController::StaticClass();
	GetCharacterMovement()->MaxWalkSpeed = 380.f;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GrayboxBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrayboxBody"));
	GrayboxBody->SetupAttachment(GetCapsuleComponent());
	GrayboxBody->SetRelativeLocation(FVector(0.f, 0.f, -20.f));
	GrayboxBody->SetRelativeScale3D(FVector(0.55f, 0.55f, 1.35f));
	GrayboxBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	if (GrayboxBody)
	{
		if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
		{
			GrayboxBody->SetStaticMesh(Cube);
		}
		if (UMaterialInterface* Shape = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
		{
			if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Shape, this))
			{
				MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.55f, 0.16f, 0.12f));
				GrayboxBody->SetMaterial(0, MID);
			}
		}
	}
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
