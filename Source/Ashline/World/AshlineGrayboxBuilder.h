#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AshlineTypes.h"
#include "AshlineGrayboxBuilder.generated.h"

class ACameraActor;
class APlayerStart;
class UStaticMesh;
class UMaterialInterface;

/** Runtime graybox world for the campaign frontend and ASH-01..12. */
UCLASS()
class ASHLINE_API AAshlineGrayboxBuilder : public AActor
{
	GENERATED_BODY()

public:
	AAshlineGrayboxBuilder();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graybox")
	void ClearBuiltActors();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graybox")
	void BuildFrontendHub();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Graybox")
	void BuildMission(EAshlineMissionId MissionId);

	UPROPERTY()
	TObjectPtr<ACameraActor> FrontendCamera;

	UPROPERTY()
	FVector LastPlayerStartLocation = FVector(-1800.f, 0.f, 120.f);

	UPROPERTY()
	FRotator LastPlayerStartRotation = FRotator::ZeroRotator;

protected:
	void LoadPrimitives();
	void SpawnAtmosphere(const FLinearColor& SunColor, float SunIntensity, const FLinearColor& FogColor, float FogDensity);

	AActor* Box(const FVector& Location, const FVector& Scale, const FLinearColor& Color, bool bCollision = true);
	AActor* Cylinder(const FVector& Location, const FVector& Scale, const FLinearColor& Color);
	AActor* Sphere(const FVector& Location, const FVector& Scale, const FLinearColor& Color);
	void Floor(const FVector& Center, const FVector2D& Size, const FLinearColor& Color);
	void Wall(const FVector& Location, const FVector& Scale, const FLinearColor& Color);
	void Cover(const FVector& Location);
	void PlayerStartAt(const FVector& Location, const FRotator& Rotation);
	void SpawnAI(const FVector& Location, EAshlineAIArchetype Archetype);
	void Objective(FName Id, const FVector& Location, bool bCompleteMission, const FLinearColor& Color = FLinearColor(1.f, 0.85f, 0.2f));
	void ExtraAIAround(const FVector& Origin, float Radius);

	void BuildWireCut();
	void BuildDustMarket();
	void BuildHoldfast();
	void BuildNightGlass();
	void BuildConvoyGhost();
	void BuildAshHarbor();
	void BuildWhiteout();
	void BuildCatacomb();
	void BuildRidgeWire();
	void BuildFalseFlag();
	void BuildLastTrain();
	void BuildAshlineFinale();

	UPROPERTY()
	TArray<TObjectPtr<AActor>> BuiltActors;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CylinderMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> SphereMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> ShapeMaterial;

	EAshlineDifficulty CachedDifficulty = EAshlineDifficulty::Regular;
};
