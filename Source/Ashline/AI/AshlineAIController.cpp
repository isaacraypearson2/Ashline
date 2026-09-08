#include "AI/AshlineAIController.h"

#include "AI/AshlineAICatalog.h"
#include "AI/AshlineAICharacter.h"
#include "AI/AshlineCoverPoint.h"
#include "CollisionQueryParams.h"
#include "EngineUtils.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
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

	UAISenseConfig_Hearing* Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	Hearing->HearingRange = 4000.f;
	Hearing->DetectionByAffiliation.bDetectEnemies = true;
	Hearing->DetectionByAffiliation.bDetectFriendlies = true;
	Hearing->DetectionByAffiliation.bDetectNeutrals = true;
	Perception->ConfigureSense(*Hearing);

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
		if (UAISenseConfig_Hearing* Hearing = Cast<UAISenseConfig_Hearing>(Perception->GetSenseConfig(UAISense::GetSenseID<UAISense_Hearing>())))
		{
			Hearing->HearingRange = Bot->ArchetypeDef.HearingRange;
			Perception->RequestStimuliListenerUpdate();
		}
		CombatState = EAshlineAICombatState::Idle;
	}
}

void AAshlineAIController::OnUnPossess()
{
	VacateCover();
	Super::OnUnPossess();
}

void AAshlineAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!GetWorld())
	{
		return;
	}
	BurstCooldown = FMath::Max(0.f, BurstCooldown - DeltaSeconds);
	ShotTimer = FMath::Max(0.f, ShotTimer - DeltaSeconds);
	CoverReevaluate = FMath::Max(0.f, CoverReevaluate - DeltaSeconds);
	StateAge += DeltaSeconds;

	APawn* MyPawn = GetPawn();
	AAshlineAICharacter* Bot = Cast<AAshlineAICharacter>(MyPawn);
	if (!Bot || Bot->bDead)
	{
		CombatState = EAshlineAICombatState::Dead;
		return;
	}

	bSuppressed = (GetWorld()->GetTimeSeconds() - LastDamageTime) < 0.8f;
	if (bSuppressed && Bot->GetCharacterMovement())
	{
		Bot->Crouch();
	}
	else if (Bot->GetCharacterMovement() && Bot->bIsCrouched && CombatState != EAshlineAICombatState::TakeCover)
	{
		Bot->UnCrouch();
	}

	TryAcquireTarget();
	TickCombat(DeltaSeconds, Bot);
}

void AAshlineAIController::TickCombat(float DeltaSeconds, AAshlineAICharacter* Bot)
{
	(void)DeltaSeconds;
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	const bool bSeeTarget = FocusTarget && HasLineOfSightTo(FocusTarget);
	if (FocusTarget)
	{
		LastKnownLocation = FocusTarget->GetActorLocation();
		SetFocus(FocusTarget);
		const FVector ToTarget = FocusTarget->GetActorLocation() - MyPawn->GetActorLocation();
		SetControlRotation(ToTarget.Rotation());
	}

	if (bSeeTarget)
	{
		if (CombatState == EAshlineAICombatState::Idle || CombatState == EAshlineAICombatState::Investigate)
		{
			CombatState = EAshlineAICombatState::Engage;
			StateAge = 0.f;
		}

		const float Dist = FVector::Dist(MyPawn->GetActorLocation(), FocusTarget->GetActorLocation());
		if (CoverReevaluate <= 0.f && (bSuppressed || FMath::FRand() < Bot->ArchetypeDef.CoverSeekChance * 0.15f))
		{
			CoverReevaluate = 1.6f;
			SeekCover(Bot);
		}
		else if (Bot->ArchetypeDef.bFlanks && CombatState != EAshlineAICombatState::TakeCover && Dist < 2200.f && Dist > 500.f && StateAge > 2.5f && FMath::FRand() < 0.012f)
		{
			SeekFlank(Bot);
		}

		if (CombatState == EAshlineAICombatState::TakeCover && ClaimedCover)
		{
			const FVector CoverLoc = ClaimedCover->GetActorLocation();
			const float CoverDist = FVector::Dist(MyPawn->GetActorLocation(), CoverLoc);
			if (CoverDist > 70.f)
			{
				MoveToward(CoverLoc, 1.f);
			}
			else
			{
				if (ClaimedCover->bCrouchCover)
				{
					Bot->Crouch();
				}
				FireBurst(Bot);
			}
		}
		else if (CombatState == EAshlineAICombatState::Flank)
		{
			MoveToward(FlankLocation, 1.f);
			if (FVector::Dist(MyPawn->GetActorLocation(), FlankLocation) < 120.f)
			{
				CombatState = EAshlineAICombatState::Engage;
				StateAge = 0.f;
			}
			FireBurst(Bot);
		}
		else
		{
			if (!Bot->ArchetypeDef.bHoldsPosition)
			{
				const float Desired = Bot->ArchetypeDef.PreferredEngageDistance;
				if (Dist > Desired + 180.f)
				{
					MoveToward(FocusTarget->GetActorLocation(), 1.f);
				}
				else if (Dist < Desired - 280.f && Dist > 220.f)
				{
					MoveToward(MyPawn->GetActorLocation() - (FocusTarget->GetActorLocation() - MyPawn->GetActorLocation()).GetSafeNormal2D() * 200.f, 0.7f);
				}
			}
			FireBurst(Bot);
		}
		return;
	}

	if (FocusTarget)
	{
		CombatState = EAshlineAICombatState::Investigate;
		MoveToward(LastKnownLocation, 0.85f);
		if (FVector::Dist(MyPawn->GetActorLocation(), LastKnownLocation) < 120.f && StateAge > 4.f)
		{
			FocusTarget = nullptr;
			CombatState = EAshlineAICombatState::Idle;
		}
		return;
	}

	if ((GetWorld()->GetTimeSeconds() - LastHeardTime) < 6.f)
	{
		CombatState = EAshlineAICombatState::Investigate;
		MoveToward(InvestigateLocation, 0.8f);
		return;
	}

	CombatState = EAshlineAICombatState::Idle;
}

void AAshlineAIController::FireBurst(AAshlineAICharacter* Bot)
{
	if (!FocusTarget || !GetPawn() || !GetWorld())
	{
		return;
	}
	if (!HasLineOfSightTo(FocusTarget))
	{
		return;
	}

	if (ShotsLeftInBurst <= 0)
	{
		if (BurstCooldown > 0.f)
		{
			return;
		}
		ShotsLeftInBurst = FMath::Max(1, Bot->ArchetypeDef.BurstCount);
		BurstCooldown = FMath::Max(0.22f, Bot->ArchetypeDef.BurstPauseSeconds);
		ShotInterval = FMath::Max(0.08f, Bot->ArchetypeDef.ReactionSeconds * 0.25f);
		if (bSuppressed)
		{
			ShotsLeftInBurst = FMath::Max(1, ShotsLeftInBurst / 2);
			BurstCooldown += 0.35f;
		}
	}

	if (ShotTimer > 0.f)
	{
		return;
	}

	ShotTimer = ShotInterval;
	--ShotsLeftInBurst;

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

	const FVector Start = GetPawn()->GetActorLocation() + FVector(0.f, 0.f, 60.f);
	FVector AimPoint = FocusTarget->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const float DistMeters = FVector::Dist(Start, AimPoint) / 100.f;
	const float DistPenalty = FMath::Clamp(DistMeters / 80.f, 0.f, 0.45f);
	const float MovePenalty = GetPawn()->GetVelocity().Size2D() > 80.f ? 0.12f : 0.f;
	const float Acc = FMath::Clamp(Bot->ArchetypeDef.Accuracy * Tuning.AIAccuracyMul + SquadAccuracyBonus() - DistPenalty - MovePenalty, 0.05f, 0.95f);

	if (FMath::FRand() > Acc)
	{
		AimPoint += FVector(FMath::FRandRange(-40.f, 40.f), FMath::FRandRange(-40.f, 40.f), FMath::FRandRange(-20.f, 20.f));
	}

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AshlineAIShot), false, GetPawn());
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, AimPoint, ECC_Visibility, Params))
	{
		if (Hit.GetActor() == FocusTarget)
		{
			UGameplayStatics::ApplyDamage(FocusTarget, 8.f, this, GetPawn(), nullptr);
		}
	}
}

void AAshlineAIController::SeekCover(AAshlineAICharacter* Bot)
{
	if (!GetWorld() || !FocusTarget)
	{
		return;
	}
	AAshlineCoverPoint* Best = AAshlineCoverPoint::PickBest(
		GetWorld(),
		GetPawn()->GetActorLocation(),
		FocusTarget->GetActorLocation(),
		Bot,
		1800.f);
	if (!Best)
	{
		return;
	}
	VacateCover();
	Best->Occupy(Bot);
	ClaimedCover = Best;
	CombatState = EAshlineAICombatState::TakeCover;
	StateAge = 0.f;
}

void AAshlineAIController::SeekFlank(AAshlineAICharacter* Bot)
{
	if (!FocusTarget || !GetPawn())
	{
		return;
	}
	const FVector ToThreat = (FocusTarget->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal2D();
	const FVector Side = FVector::CrossProduct(ToThreat, FVector::UpVector).GetSafeNormal() * (FMath::FRand() < 0.5f ? 1.f : -1.f);
	FlankLocation = FocusTarget->GetActorLocation() + Side * 900.f - ToThreat * 400.f;
	FlankLocation.Z = GetPawn()->GetActorLocation().Z;
	CombatState = EAshlineAICombatState::Flank;
	StateAge = 0.f;
	(void)Bot;
}

void AAshlineAIController::MoveToward(const FVector& Location, float Scale)
{
	if (APawn* MyPawn = GetPawn())
	{
		const FVector Delta = Location - MyPawn->GetActorLocation();
		MyPawn->AddMovementInput(Delta.GetSafeNormal2D(), Scale);
	}
}

bool AAshlineAIController::HasLineOfSightTo(AActor* Target) const
{
	if (!Target || !GetPawn() || !GetWorld())
	{
		return false;
	}
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AshlineAILos), false, GetPawn());
	const FVector Start = GetPawn()->GetActorLocation() + FVector(0.f, 0.f, 60.f);
	const FVector End = Target->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		return Hit.GetActor() == Target || Hit.GetActor() == nullptr;
	}
	return true;
}

float AAshlineAIController::SquadAccuracyBonus() const
{
	if (!GetWorld() || !GetPawn())
	{
		return 0.f;
	}
	for (TActorIterator<AAshlineAICharacter> It(GetWorld()); It; ++It)
	{
		AAshlineAICharacter* Other = *It;
		if (Other && !Other->bDead && Other->Archetype == EAshlineAIArchetype::Officer
			&& FVector::Dist(Other->GetActorLocation(), GetPawn()->GetActorLocation()) < 1600.f)
		{
			return 0.08f;
		}
	}
	return 0.f;
}

void AAshlineAIController::VacateCover()
{
	if (ClaimedCover)
	{
		ClaimedCover->Vacate(GetPawn());
		ClaimedCover = nullptr;
	}
}

void AAshlineAIController::TryAcquireTarget()
{
	if (FocusTarget)
	{
		const AAshlineCharacter* Player = Cast<AAshlineCharacter>(FocusTarget);
		if (Player && Player->Health > 0.f)
		{
			return;
		}
		if (!IsValid(FocusTarget))
		{
			FocusTarget = nullptr;
		}
	}

	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (HasLineOfSightTo(PlayerPawn))
		{
			FocusTarget = PlayerPawn;
		}
	}
}

void AAshlineAIController::NotifyTookDamage(AActor* InstigatorActor, float Damage)
{
	(void)Damage;
	LastDamageTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (InstigatorActor)
	{
		FocusTarget = InstigatorActor;
		LastKnownLocation = InstigatorActor->GetActorLocation();
		CombatState = EAshlineAICombatState::Engage;
		StateAge = 0.f;
	}
}

void AAshlineAIController::NotifyDied()
{
	VacateCover();
	CombatState = EAshlineAICombatState::Dead;
	FocusTarget = nullptr;
	ClearFocus(EAIFocusPriority::Gameplay);
}

void AAshlineAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		if (Cast<AAshlineCharacter>(Actor))
		{
			FocusTarget = Actor;
			LastKnownLocation = Actor->GetActorLocation();
			CombatState = EAshlineAICombatState::Engage;
			StateAge = 0.f;
			return;
		}
	}

	for (AActor* Actor : UpdatedActors)
	{
		if (!Actor)
		{
			continue;
		}
		LastHeardTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		InvestigateLocation = Actor->GetActorLocation();
		if (!FocusTarget)
		{
			CombatState = EAshlineAICombatState::Investigate;
			StateAge = 0.f;
		}
	}
}
