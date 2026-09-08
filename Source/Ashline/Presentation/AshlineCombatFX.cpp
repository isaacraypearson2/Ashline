#include "Presentation/AshlineCombatFX.h"
#include "Presentation/AshlineLoad.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlinePresentationLibrary.h"
#include "Presentation/AshlineWeaponVisual.h"
#include "Sound/SoundBase.h"

AAshlineTracerStreak::AAshlineTracerStreak()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Streak"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCastShadow(false);
}

void AAshlineTracerStreak::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Remaining -= DeltaSeconds;
	if (Remaining <= 0.f)
	{
		Destroy();
	}
}

void AAshlineTracerStreak::Configure(const FVector& Start, const FVector& End, const FLinearColor& Color)
{
	const FVector Delta = End - Start;
	const float Length = Delta.Size();
	if (Length < 8.f || !Mesh)
	{
		Destroy();
		return;
	}

	UStaticMesh* Cube = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (Cube)
	{
		Mesh->SetStaticMesh(Cube);
	}
	SetActorLocation((Start + End) * 0.5f);
	SetActorRotation(Delta.Rotation());
	SetActorScale3D(FVector(Length / 100.f, 0.012f, 0.012f));

	UMaterialInterface* Emissive = AshlineLoad::Object<UMaterialInterface>(TEXT("/Engine/EngineMaterials/EmissiveMeshMaterial.EmissiveMeshMaterial"));
	if (!Emissive)
	{
		Emissive = AshlineLoad::Object<UMaterialInterface>(TEXT("/Engine/EngineMaterials/DefaultUnlitMaterial.DefaultUnlitMaterial"));
	}
	if (Emissive)
	{
		if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Emissive, this))
		{
			MID->SetVectorParameterValue(TEXT("Color"), Color);
			MID->SetVectorParameterValue(TEXT("EmissiveColor"), Color * 8.f);
			MID->SetScalarParameterValue(TEXT("EmissiveStrength"), 12.f);
			Mesh->SetMaterial(0, MID);
		}
		else
		{
			Mesh->SetMaterial(0, Emissive);
		}
	}
}

namespace AshlineFX
{
	template <typename T>
	static T* LoadFirst(const TArray<FString>& Paths)
	{
		for (const FString& Path : Paths)
		{
			if (T* Obj = AshlineLoad::Object<T>(Path))
			{
				return Obj;
			}
		}
		return nullptr;
	}
}

UNiagaraSystem* UAshlineCombatFX::ResolveMuzzleNiagara(FName WeaponId)
{
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(WeaponId))
	{
		if (UNiagaraSystem* FX = AshlineLoad::Soft(Visual->MuzzleFX))
		{
			return FX;
		}
	}
	return AshlineFX::LoadFirst<UNiagaraSystem>({
		UAshlineContentManifest::WeaponMuzzleFXPath(WeaponId),
		TEXT("/Game/Ashline/FX/Muzzle/NS_Muzzle_Default.NS_Muzzle_Default")
	});
}

UParticleSystem* UAshlineCombatFX::ResolveMuzzleCascade()
{
	return AshlineFX::LoadFirst<UParticleSystem>({
		TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"),
		TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire")
	});
}

UNiagaraSystem* UAshlineCombatFX::ResolveImpactNiagara(bool bFlesh)
{
	return AshlineFX::LoadFirst<UNiagaraSystem>({
		bFlesh ? TEXT("/Game/Ashline/FX/Impacts/NS_Blood.NS_Blood") : TEXT("/Game/Ashline/FX/Impacts/NS_Sparks.NS_Sparks"),
		TEXT("/Game/Ashline/FX/Impacts/NS_Impact_Default.NS_Impact_Default")
	});
}

UParticleSystem* UAshlineCombatFX::ResolveSparksCascade()
{
	return AshlineFX::LoadFirst<UParticleSystem>({
		TEXT("/Game/StarterContent/Particles/P_Sparks.P_Sparks"),
		TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"),
		TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire")
	});
}

UParticleSystem* UAshlineCombatFX::ResolveExplosionCascade()
{
	return AshlineFX::LoadFirst<UParticleSystem>({
		TEXT("/Game/Ashline/FX/Explosions/P_Explosion_Stub.P_Explosion_Stub"),
		TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"),
		TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire")
	});
}

UParticleSystem* UAshlineCombatFX::ResolveBloodCascade()
{
	return AshlineFX::LoadFirst<UParticleSystem>({
		TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"),
		TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion")
	});
}

UMaterialInterface* UAshlineCombatFX::ResolveImpactDecal()
{
	return UAshlinePresentationLibrary::ResolveImpactDecalMaterial();
}

void UAshlineCombatFX::SpawnMuzzle(UObject* WorldContext, FName WeaponId, const FVector& Location, const FRotator& Rotation)
{
	if (UNiagaraSystem* Niagara = ResolveMuzzleNiagara(WeaponId))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, Niagara, Location, Rotation);
		return;
	}
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(WeaponId))
	{
		if (UParticleSystem* Cascade = AshlineLoad::Soft(Visual->MuzzleCascadeFX))
		{
			UGameplayStatics::SpawnEmitterAtLocation(WorldContext, Cascade, Location, Rotation, FVector(0.18f), true);
			return;
		}
	}
	if (UParticleSystem* Cascade = ResolveMuzzleCascade())
	{
		UGameplayStatics::SpawnEmitterAtLocation(WorldContext, Cascade, Location, Rotation, FVector(0.12f), true);
	}
}

void UAshlineCombatFX::SpawnTracer(UObject* WorldContext, FName WeaponId, const FVector& Start, const FVector& End)
{
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(WeaponId))
	{
		if (UNiagaraSystem* Tracer = AshlineLoad::Soft(Visual->TracerFX))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, Tracer, Start, (End - Start).Rotation());
			return;
		}
	}
	if (UNiagaraSystem* Tracer = AshlineFX::LoadFirst<UNiagaraSystem>({
		TEXT("/Game/Ashline/FX/Tracers/NS_Tracer_Default.NS_Tracer_Default")
	}))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, Tracer, Start, (End - Start).Rotation());
		return;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (AAshlineTracerStreak* Streak = World->SpawnActor<AAshlineTracerStreak>(Start, FRotator::ZeroRotator, Params))
	{
		Streak->Configure(Start, End, FLinearColor(1.f, 0.82f, 0.35f));
	}
}

void UAshlineCombatFX::SpawnImpact(UObject* WorldContext, FName WeaponId, const FHitResult& Hit, bool bFlesh)
{
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(WeaponId))
	{
		if (bFlesh)
		{
			if (UNiagaraSystem* Blood = AshlineLoad::Soft(Visual->BloodFX))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, Blood, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}
		else if (UNiagaraSystem* Impact = AshlineLoad::Soft(Visual->ImpactFX))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, Impact, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}
		if (!bFlesh)
		{
			if (UParticleSystem* Sparks = AshlineLoad::Soft(Visual->SparksCascade))
			{
				UGameplayStatics::SpawnEmitterAtLocation(WorldContext, Sparks, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), FVector(0.35f), true);
			}
		}
	}

	if (UNiagaraSystem* Niagara = ResolveImpactNiagara(bFlesh))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, Niagara, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}
	else if (bFlesh)
	{
		if (UParticleSystem* Blood = ResolveBloodCascade())
		{
			UGameplayStatics::SpawnEmitterAtLocation(WorldContext, Blood, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), FVector(0.22f), true);
		}
	}
	else if (UParticleSystem* Sparks = ResolveSparksCascade())
	{
		UGameplayStatics::SpawnEmitterAtLocation(WorldContext, Sparks, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), FVector(0.28f), true);
	}

	UMaterialInterface* DecalMat = nullptr;
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(WeaponId))
	{
		DecalMat = AshlineLoad::Soft(Visual->ImpactDecal);
	}
	if (!DecalMat)
	{
		DecalMat = ResolveImpactDecal();
	}
	if (DecalMat)
	{
		const FLinearColor Tint = bFlesh ? FLinearColor(0.45f, 0.05f, 0.05f) : FLinearColor(0.12f, 0.12f, 0.1f);
		(void)Tint;
		UGameplayStatics::SpawnDecalAtLocation(
			WorldContext,
			DecalMat,
			FVector(bFlesh ? 18.f : 12.f, bFlesh ? 18.f : 12.f, 4.f),
			Hit.ImpactPoint + Hit.ImpactNormal * 1.5f,
			Hit.ImpactNormal.Rotation(),
			10.f);
	}
}

void UAshlineCombatFX::SpawnExplosion(UObject* WorldContext, const FVector& Location, float Scale)
{
	if (UNiagaraSystem* Niagara = AshlineFX::LoadFirst<UNiagaraSystem>({
		TEXT("/Game/Ashline/FX/Explosions/NS_Explosion_Default.NS_Explosion_Default")
	}))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, Niagara, Location, FRotator::ZeroRotator);
	}
	if (UParticleSystem* Cascade = ResolveExplosionCascade())
	{
		UGameplayStatics::SpawnEmitterAtLocation(WorldContext, Cascade, Location, FRotator::ZeroRotator, FVector(Scale), true);
	}
	UGameplayStatics::SpawnSoundAtLocation(
		WorldContext,
		AshlineFX::LoadFirst<USoundBase>({
			TEXT("/Game/Ashline/Audio/Weapons/SC_Explosion.SC_Explosion"),
			TEXT("/Game/StarterContent/Audio/Explosion01.Explosion01"),
			TEXT("/Game/StarterContent/Audio/Explosion_01.Explosion_01")
		}),
		Location,
		FRotator::ZeroRotator,
		0.85f);
}
