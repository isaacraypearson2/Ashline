#include "AI/AshlineAICharacter.h"

#include "AI/AshlineAICatalog.h"
#include "AI/AshlineAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/GameInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Presentation/AshlineCharacterPresentation.h"
#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlineLoad.h"
#include "Presentation/AshlinePresentationLibrary.h"
#include "Presentation/AshlinePresentationSettings.h"
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
	UAshlinePresentationLibrary::HideCapsuleVisual(this);
	ApplyPresentationMesh();
}

void AAshlineAICharacter::ApplyArchetype(EAshlineAIArchetype InArchetype, EAshlineDifficulty Difficulty)
{
	Archetype = InArchetype;
	ArchetypeDef = UAshlineAICatalog::GetArchetype(Archetype);
	const FAshlineDifficultyTuning Tuning = UAshlineAICatalog::GetDifficulty(Difficulty);
	Health = ArchetypeDef.MaxHealth * Tuning.AIHealthMul * FMath::Max(0.5f, ArchetypeDef.ArmorMul);
	GetCharacterMovement()->MaxWalkSpeed = ArchetypeDef.MoveSpeed;
}

void AAshlineAICharacter::ApplyPresentationMesh()
{
	USkeletalMesh* Body = AshlineLoad::Soft(BodyMeshOverride);
	if (!Body)
	{
		if (const UAshlinePresentationSettings* Settings = GetDefault<UAshlinePresentationSettings>())
		{
			Body = AshlineLoad::Soft(Settings->DefaultAIMesh);
		}
	}
	if (!Body)
	{
		if (UAshlineCharacterPresentation* Pres = UAshlinePresentationLibrary::FindCharacterPresentation(false, Archetype))
		{
			Body = AshlineLoad::Soft(Pres->BodyMesh);
		}
	}
	if (!Body)
	{
		Body = UAshlinePresentationLibrary::LoadSkeletalMesh({
			UAshlineContentManifest::AIMeshPath(Archetype)
		});
	}
	if (!Body)
	{
		Body = UAshlinePresentationLibrary::ResolveHumanoidMesh();
	}

	FLinearColor Tint = ArchetypeDef.BodyTint;
	if (Tint.A <= 0.f || Tint.Equals(FLinearColor::Black))
	{
		Tint = FLinearColor(0.45f, 0.12f, 0.1f);
	}

	if (Body && GetMesh())
	{
		GetMesh()->SetSkeletalMeshAsset(Body);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetVisibility(true);
		GetMesh()->SetCastShadow(true);
		if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeCharacterMaterial(this, Tint))
		{
			GetMesh()->SetMaterial(0, MID);
		}
		if (GrayboxBody)
		{
			GrayboxBody->SetVisibility(false);
		}
		return;
	}

	if (GrayboxBody)
	{
		GrayboxBody->SetVisibility(false);
	}
	UAshlinePresentationLibrary::ApplyHumanoidBlockout(this, Tint);
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
