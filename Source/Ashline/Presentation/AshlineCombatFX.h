#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AshlineCombatFX.generated.h"

class UNiagaraSystem;
class UParticleSystem;
class UMaterialInterface;
class USoundBase;
class UStaticMeshComponent;

/** Short-lived emissive streak used when Niagara tracers are missing. */
UCLASS()
class ASHLINE_API AAshlineTracerStreak : public AActor
{
	GENERATED_BODY()

public:
	AAshlineTracerStreak();
	virtual void Tick(float DeltaSeconds) override;

	void Configure(const FVector& Start, const FVector& End, const FLinearColor& Color);

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Mesh;

	float Remaining = 0.08f;
};

/** Engine-backed muzzle / tracer / impact / explosion resolver. Soft refs first, then StarterContent, then primitives. */
UCLASS()
class ASHLINE_API UAshlineCombatFX : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ashline|FX", meta = (WorldContext = "WorldContext"))
	static void SpawnMuzzle(UObject* WorldContext, FName WeaponId, const FVector& Location, const FRotator& Rotation);

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX", meta = (WorldContext = "WorldContext"))
	static void SpawnTracer(UObject* WorldContext, FName WeaponId, const FVector& Start, const FVector& End);

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX", meta = (WorldContext = "WorldContext"))
	static void SpawnImpact(UObject* WorldContext, FName WeaponId, const FHitResult& Hit, bool bFlesh);

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX", meta = (WorldContext = "WorldContext"))
	static void SpawnExplosion(UObject* WorldContext, const FVector& Location, float Scale = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX")
	static UNiagaraSystem* ResolveMuzzleNiagara(FName WeaponId);

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX")
	static UParticleSystem* ResolveMuzzleCascade();

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX")
	static UNiagaraSystem* ResolveImpactNiagara(bool bFlesh);

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX")
	static UParticleSystem* ResolveSparksCascade();

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX")
	static UParticleSystem* ResolveExplosionCascade();

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX")
	static UParticleSystem* ResolveBloodCascade();

	UFUNCTION(BlueprintCallable, Category = "Ashline|FX")
	static UMaterialInterface* ResolveImpactDecal();
};
