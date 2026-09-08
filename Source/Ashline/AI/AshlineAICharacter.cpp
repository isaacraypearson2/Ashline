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
#include "Presentation/AshlineMaterialFactory.h"
#include "Presentation/AshlinePresentationLibrary.h"
#include "Presentation/AshlinePresentationSettings.h"
#include "Progression/AshlineProgressionSubsystem.h"
#include "Animation/AnimInstance.h"

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
	Health = ArchetypeDef.MaxHealth * Tuning.AIHealthMul;
	GetCharacterMovement()->MaxWalkSpeed = ArchetypeDef.MoveSpeed;
}

void AAshlineAICharacter::ApplyPresentationMesh()
{
	USkeletalMesh* Body = BodyMeshOverride.LoadSynchronous();
	if (!Body)
	{
		if (const UAshlinePresentationSettings* Settings = GetDefault<UAshlinePresentationSettings>())
		{
			Body = Settings->DefaultAIMesh.LoadSynchronous();
		}
	}
	if (!Body)
	{
		if (UAshlineCharacterPresentation* Pres = UAshlinePresentationLibrary::FindCharacterPresentation(false, Archetype))
		{
			Body = Pres->BodyMesh.LoadSynchronous();
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

	FLinearColor Tint(0.45f, 0.12f, 0.1f);
	switch (Archetype)
	{
	case EAshlineAIArchetype::Officer: Tint = FLinearColor(0.15f, 0.16f, 0.22f); break;
	case EAshlineAIArchetype::Marksman: Tint = FLinearColor(0.22f, 0.2f, 0.12f); break;
	case EAshlineAIArchetype::Heavy: Tint = FLinearColor(0.12f, 0.12f, 0.12f); break;
	case EAshlineAIArchetype::Breacher: Tint = FLinearColor(0.28f, 0.12f, 0.08f); break;
	case EAshlineAIArchetype::Scout: Tint = FLinearColor(0.16f, 0.2f, 0.12f); break;
	case EAshlineAIArchetype::MachineGunner: Tint = FLinearColor(0.18f, 0.14f, 0.1f); break;
	case EAshlineAIArchetype::CivilianIrregular: Tint = FLinearColor(0.32f, 0.24f, 0.16f); break;
	default: break;
	}

	if (Body && GetMesh())
	{
		GetMesh()->SetSkeletalMeshAsset(Body);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetVisibility(true);
		GetMesh()->SetCastShadow(true);
		if (UAshlineCharacterPresentation* Pres = UAshlinePresentationLibrary::FindCharacterPresentation(false, Archetype))
		{
			if (TSubclassOf<UAnimInstance> Anim = Pres->AnimClass.LoadSynchronous())
			{
				GetMesh()->SetAnimInstanceClass(Anim);
			}
			if (UMaterialInterface* Override = Pres->BodyMaterialOverride.LoadSynchronous())
			{
				GetMesh()->SetMaterial(0, Override);
			}
			else if (UMaterialInterface* Skin = Pres->SkinMaterial.LoadSynchronous())
			{
				GetMesh()->SetMaterial(0, Skin);
			}
			else if (UMaterialInstanceDynamic* MID = UAshlineMaterialFactory::CreateCharacterInstance(this, Tint, Pres->BodyTextures))
			{
				GetMesh()->SetMaterial(0, MID);
			}
		}
		else if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeCharacterMaterial(this, Tint))
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
	LastDamageAt = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (AAshlineAIController* AICon = Cast<AAshlineAIController>(GetController()))
	{
		AICon->NotifyTookDamage(DamageCauser ? DamageCauser : (EventInstigator ? EventInstigator->GetPawn() : nullptr), Incoming);
	}
	if (Health <= 0.f)
	{
		bDead = true;
		Health = 0.f;
		if (AAshlineAIController* AICon = Cast<AAshlineAIController>(GetController()))
		{
			AICon->NotifyDied();
		}
		ApplyDeathPose();
		DetachFromControllerPendingDestroy();
		SetLifeSpan(8.f);
	}
	return Incoming;
}

void AAshlineAICharacter::ApplyDeathPose()
{
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->DisableMovement();
		Move->StopMovementImmediately();
	}
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	if (GetMesh() && GetMesh()->GetSkeletalMeshAsset())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetSimulatePhysics(true);
	}
	else
	{
		AddActorWorldRotation(FRotator(-78.f, 0.f, 12.f));
	}
}
