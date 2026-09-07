#pragma once

#include "CoreMinimal.h"
#include "AshlineTypes.h"
#include "AshlinePresentationTypes.generated.h"

class UStaticMesh;
class UMaterialInterface;

/** Per-mission lighting / grading used by the environment kit builder. */
USTRUCT(BlueprintType)
struct FAshlineLightingMood
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FLinearColor SunColor = FLinearColor(1.f, 0.95f, 0.85f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float SunIntensity = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FRotator SunRotation = FRotator(-46.f, -28.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FLinearColor FogColor = FLinearColor(0.25f, 0.28f, 0.32f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float FogDensity = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float FogHeightFalloff = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float SkyLightIntensity = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FLinearColor SkyZenith = FLinearColor(0.08f, 0.16f, 0.32f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FLinearColor SkyHorizon = FLinearColor(0.35f, 0.4f, 0.45f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float BloomIntensity = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float Vignette = 0.28f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float AmbientOcclusion = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float ColorTempKelvin = 6500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FVector4 ColorSaturation = FVector4(1.f, 1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FVector4 ColorContrast = FVector4(1.05f, 1.05f, 1.05f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float FilmGrain = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	bool bNight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FLinearColor GroundTint = FLinearColor(0.18f, 0.17f, 0.14f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FLinearColor WallTint = FLinearColor(0.22f, 0.2f, 0.17f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FLinearColor MetalTint = FLinearColor(0.12f, 0.13f, 0.14f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	EAshlineSurface DefaultGround = EAshlineSurface::Ground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float FogStartDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float VolumetricFogExtinction = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float VolumetricScattering = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float ChromaticAberration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float BloomThreshold = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float LocalExposureHighlight = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float LocalExposureShadow = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float DirtMaskIntensity = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FLinearColor FillLightColor = FLinearColor(0.45f, 0.55f, 0.75f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float FillLightIntensity = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FRotator FillLightRotation = FRotator(-20.f, 140.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FLinearColor MoonColor = FLinearColor(0.35f, 0.45f, 0.75f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float MoonIntensity = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	int32 FoliageDensity = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	float AutoExposureBias = 0.f;
}

/** Soft-ref slots so maps can pull Fab / Megascans / MetaHuman without baking binaries into git. */
USTRUCT(BlueprintType)
struct FAshlineSoftMeshSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FName SlotId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	TSoftObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	TSoftObjectPtr<UMaterialInterface> MaterialOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Kit")
	FVector Scale = FVector::OneVector;
};
