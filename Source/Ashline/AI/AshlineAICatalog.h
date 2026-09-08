#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.h"
#include "AshlineAICatalog.generated.h"

USTRUCT(BlueprintType)
struct FAshlineAIArchetypeDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	EAshlineAIArchetype Archetype = EAshlineAIArchetype::Rifleman;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	FText RoleDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	float Accuracy = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	float ReactionSeconds = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	float SightRange = 3500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	float MoveSpeed = 380.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	FName PreferredWeaponId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	bool bHoldsPosition = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	bool bFlanks = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	int32 BurstCount = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	float BurstPauseSeconds = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	float CoverSeekChance = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	float HearingRange = 3800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|AI")
	float PreferredEngageDistance = 1400.f;
};

UCLASS()
class ASHLINE_API UAshlineAICatalog : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ashline|AI")
	static TArray<FAshlineAIArchetypeDef> BuildArchetypes();

	UFUNCTION(BlueprintCallable, Category = "Ashline|AI")
	static FAshlineAIArchetypeDef GetArchetype(EAshlineAIArchetype Archetype);

	UFUNCTION(BlueprintCallable, Category = "Ashline|AI")
	static FAshlineDifficultyTuning GetDifficulty(EAshlineDifficulty Difficulty);
};
