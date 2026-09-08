#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AshlineTypes.h"
#include "Presentation/AshlinePresentationTypes.h"
#include "AshlineGrayboxBuilder.generated.h"

class ACameraActor;
class APlayerStart;
class UStaticMesh;
class UMaterialInterface;
class UAshlineEnvironmentKit;
class UDirectionalLightComponent;
class USceneComponent;

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
	void ApplyMissionMood(EAshlineMissionId MissionId);
	void SpawnFullAtmosphere();
	void SpawnPostProcess();
	void DressMission(EAshlineMissionId MissionId);
	void ConfigureDynamicWorldLighting();
	void MakeRuntimeMovable(USceneComponent* Component) const;
	void ConfigureSun(UDirectionalLightComponent* Light) const;
	void ConfigureSecondaryDirectional(UDirectionalLightComponent* Light) const;
	void Practical(const FVector& Location, const FLinearColor& Color, float Intensity, float Radius, bool bCastShadows = false);
	void Spot(const FVector& Location, const FRotator& Rotation, const FLinearColor& Color, float Intensity, float Radius, float InnerCone = 20.f, float OuterCone = 42.f, bool bCastShadows = false);
	void InteriorVolume(const FVector& Location, const FVector& Extent, float ExtraVignette, float ExposureBias);
	void DecalMark(const FVector& Location, const FRotator& Rotation, const FVector& Size);
	void Tree(const FVector& Location, float Height = 420.f);
	void Bush(const FVector& Location);
	void GrassPatch(const FVector& Center, float Radius, int32 Count);
	void Sandbag(const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator);
	void VehicleHull(const FVector& Location, const FRotator& Rotation, const FLinearColor& Color, float Length = 5.2f);
	void WindowStrip(const FVector& Location, int32 Count, float Spacing, const FLinearColor& Glow);
	void Doorway(const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator);
	void CoverLine(const FVector& From, const FVector& To, int32 Count);
	void TreeBelt(const FVector& From, const FVector& To, int32 Count, float Height = 380.f);
	void ScatterFoliage(const FVector& Center, float Radius, int32 Trees, int32 Bushes);
	void Watchtower(const FVector& Location, float Height = 900.f);
	void Building(const FVector& Location, const FVector2D& Footprint, float Height, const FLinearColor& Color);
	void WalledYard(const FVector& Center, const FVector2D& HalfExtent, float WallHeight, const FLinearColor& Color, int32 OpenSide, float GateWidth = 600.f);
	void RoadStrip(const FVector& From, const FVector& To, float Width = 700.f);
	void ScatterKitProps(const FVector& Center, float Radius, int32 Count);

	AActor* Box(const FVector& Location, const FVector& Scale, const FLinearColor& Color, bool bCollision = true, EAshlineSurface Surface = EAshlineSurface::Auto);
	AActor* Cylinder(const FVector& Location, const FVector& Scale, const FLinearColor& Color, EAshlineSurface Surface = EAshlineSurface::Auto);
	AActor* Sphere(const FVector& Location, const FVector& Scale, const FLinearColor& Color, EAshlineSurface Surface = EAshlineSurface::Auto);
	void Floor(const FVector& Center, const FVector2D& Size, const FLinearColor& Color);
	void Wall(const FVector& Location, const FVector& Scale, const FLinearColor& Color);
	void Cover(const FVector& Location);
	void PlayerStartAt(const FVector& Location, const FRotator& Rotation);
	void SpawnAI(const FVector& Location, EAshlineAIArchetype Archetype);
	void Objective(FName Id, const FVector& Location, bool bCompleteMission, const FLinearColor& Color = FLinearColor(1.f, 0.85f, 0.2f));
	void ExtraAIAround(const FVector& Origin, float Radius);
	void ApplySurfaceMaterial(UStaticMeshComponent* Mesh, AActor* MaterialOuter, const FLinearColor& Color, EAshlineSurface Surface);

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

	EAshlineMissionId ActiveMissionId = EAshlineMissionId::None;

	FAshlineLightingMood ActiveMood;

	UPROPERTY()
	TObjectPtr<UAshlineEnvironmentKit> ActiveKit;
};
