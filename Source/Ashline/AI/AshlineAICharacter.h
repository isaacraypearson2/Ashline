#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI/AshlineAICatalog.h"
#include "AshlineTypes.h"
#include "AshlineAICharacter.generated.h"

class UStaticMeshComponent;
class USkeletalMesh;

UCLASS()
class ASHLINE_API AAshlineAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAshlineAICharacter();

	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|AI")
	void ApplyArchetype(EAshlineAIArchetype InArchetype, EAshlineDifficulty Difficulty);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	EAshlineAIArchetype Archetype = EAshlineAIArchetype::Rifleman;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|AI")
	FAshlineAIArchetypeDef ArchetypeDef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|AI")
	float Health = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|AI")
	bool bDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|AI")
	TObjectPtr<UStaticMeshComponent> GrayboxBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	TSoftObjectPtr<USkeletalMesh> BodyMeshOverride;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|AI")
	EAshlineAICombatState CombatState = EAshlineAICombatState::Idle;

protected:
	void ApplyPresentationMesh();
	void ApplyDeathPose();
};
