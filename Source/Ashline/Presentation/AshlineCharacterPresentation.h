#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AshlineTypes.h"
#include "AshlineCharacterPresentation.generated.h"

class USkeletalMesh;
class UAnimInstance;
class UMaterialInterface;

/**
 * Hero / AI mesh assignment. Point these at a MetaHuman or mannequin after Bridge import.
 * Runtime tries Engine / StarterContent mannequin paths when soft refs are empty.
 */
UCLASS(BlueprintType)
class ASHLINE_API UAshlineCharacterPresentation : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	FName PresentationId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	bool bHero = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	EAshlineAIArchetype AIArchetype = EAshlineAIArchetype::Rifleman;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	TSoftObjectPtr<USkeletalMesh> BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	TSoftObjectPtr<USkeletalMesh> FaceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	TSoftClassPtr<UAnimInstance> AnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	TSoftObjectPtr<UMaterialInterface> BodyMaterialOverride;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	FVector MeshRelativeLocation = FVector(0.f, 0.f, -96.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	FRotator MeshRelativeRotation = FRotator(0.f, -90.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	FVector MeshScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Character")
	FString MetaHumanNotes;
};
