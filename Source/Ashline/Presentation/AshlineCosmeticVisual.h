#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AshlineTypes.h"
#include "AshlineCosmeticVisual.generated.h"

class USkeletalMesh;
class UStaticMesh;
class UMaterialInterface;

/**
 * Optional authored override for one locker cosmetic.
 * Soft refs win over the catalog's canonical ContentManifest paths when the asset exists.
 */
UCLASS(BlueprintType)
class ASHLINE_API UAshlineCosmeticVisual : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	FName CosmeticId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	EAshlineCosmeticSlot Slot = EAshlineCosmeticSlot::Camo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	TSoftObjectPtr<USkeletalMesh> MeshOverride;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	TSoftObjectPtr<UStaticMesh> PartMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	TSoftObjectPtr<UMaterialInterface> MaterialOverride;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	FVector RelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	FRotator RelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	FVector RelativeScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Cosmetic")
	FString FabNotes;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("AshlineCosmeticVisual"), CosmeticId.IsNone() ? GetFName() : CosmeticId);
	}
};
