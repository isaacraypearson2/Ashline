#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AshlineTypes.h"
#include "Presentation/AshlinePresentationTypes.h"
#include "AshlineEnvironmentKit.generated.h"

class USoundBase;

/**
 * Content-driven mission kit. Soft-refs point at Fab / Megascans / StarterContent.
 * Runtime falls back to Engine materials + procedural lighting when assets are missing.
 */
UCLASS(BlueprintType)
class ASHLINE_API UAshlineEnvironmentKit : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	EAshlineMissionId MissionId = EAshlineMissionId::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	FName KitId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	FAshlineLightingMood Mood;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	TSoftObjectPtr<UMaterialInterface> GroundMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	TSoftObjectPtr<UMaterialInterface> WallMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	TSoftObjectPtr<UMaterialInterface> TrimMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	TSoftObjectPtr<UMaterialInterface> FoliageMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	TSoftObjectPtr<UMaterialInterface> DecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	TSoftObjectPtr<UMaterialInterface> GlassMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	TArray<FAshlineSoftMeshSlot> PropMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	TSoftObjectPtr<USoundBase> MusicBed;

	/** When true, builder uses Mood instead of the C++ MoodForMission baseline. Empty stubs stay false. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	bool bOverrideMood = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Kit")
	FString FabNotes;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("AshlineEnvironmentKit"), KitId.IsNone() ? GetFName() : KitId);
	}
};
