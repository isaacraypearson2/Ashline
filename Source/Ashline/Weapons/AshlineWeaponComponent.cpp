#include "Weapons/AshlineWeaponComponent.h"

#include "Ashline.h"
#include "AshlineDualSense.h"
#include "Camera/CameraComponent.h"
#include "CollisionQueryParams.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "Presentation/AshlineAudioDirector.h"
#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlineLoad.h"
#include "Presentation/AshlinePresentationLibrary.h"
#include "Presentation/AshlineWeaponVisual.h"
#include "Meta/AshlineMetaCatalog.h"
#include "Perception/AISense_Hearing.h"
#include "Weapons/AshlineWeaponCatalog.h"

UAshlineWeaponComponent::UAshlineWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAshlineWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	AttachVisuals();
	RefreshVisuals();
}

void UAshlineWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MuzzleFlashRemaining > 0.f)
	{
		MuzzleFlashRemaining -= DeltaTime;
		if (MuzzleLight)
		{
			MuzzleLight->SetVisibility(MuzzleFlashRemaining > 0.f);
			MuzzleLight->SetIntensity(MuzzleFlashRemaining > 0.f ? 8000.f * (MuzzleFlashRemaining / MuzzleFlashSeconds) : 0.f);
		}
	}

	FireCooldown = FMath::Max(0.f, FireCooldown - DeltaTime);
	if (bReloading)
	{
		ReloadRemaining -= DeltaTime;
		if (ReloadRemaining <= 0.f)
		{
			FAshlineRuntimeWeapon& Active = bUsingPrimary ? PrimaryWeapon : SecondaryWeapon;
			const int32 Need = Active.Stats.MagazineSize - Active.AmmoInMag;
			const int32 Taken = FMath::Min(Need, Active.Reserve);
			Active.AmmoInMag += Taken;
			Active.Reserve -= Taken;
			bReloading = false;
		}
		return;
	}

	if (bWantsFire && FireCooldown <= 0.f)
	{
		FireShot();
		if (!GetActiveWeapon().Stats.bAutomatic)
		{
			bWantsFire = false;
		}
	}

	TickCombatFeel(DeltaTime);
}

void UAshlineWeaponComponent::LoadFromLoadout(const FAshlineLoadoutSlot& Primary, const FAshlineLoadoutSlot& Secondary, int32 PrimaryUpgrade, int32 SecondaryUpgrade)
{
	auto Build = [](const FAshlineLoadoutSlot& Slot, int32 Upgrade) -> FAshlineRuntimeWeapon
	{
		FAshlineRuntimeWeapon Runtime;
		UAshlineWeaponCatalog::FindWeapon(Slot.WeaponId, Runtime.Definition);
		Runtime.Stats = UAshlineWeaponCatalog::ComposeStats(Runtime.Definition, Slot.Attachments);
		Runtime.UpgradeTier = Upgrade;
		Runtime.Stats.Damage += Upgrade * 1.5f;
		Runtime.Stats.RecoilPitch = FMath::Max(0.05f, Runtime.Stats.RecoilPitch - Upgrade * 0.04f);
		Runtime.AmmoInMag = Runtime.Stats.MagazineSize;
		Runtime.Reserve = Runtime.Stats.ReserveAmmo;
		Runtime.SkinId = Slot.SkinId.IsNone() ? FName(TEXT("SKIN_FACTORY")) : Slot.SkinId;
		return Runtime;
	};

	PrimaryWeapon = Build(Primary, PrimaryUpgrade);
	SecondaryWeapon = Build(Secondary, SecondaryUpgrade);
	bUsingPrimary = true;
	RefreshVisuals();
}

void UAshlineWeaponComponent::StartFire()
{
	bWantsFire = true;
}

void UAshlineWeaponComponent::StopFire()
{
	bWantsFire = false;
}

void UAshlineWeaponComponent::Reload()
{
	FAshlineRuntimeWeapon& Active = bUsingPrimary ? PrimaryWeapon : SecondaryWeapon;
	if (bReloading || Active.AmmoInMag >= Active.Stats.MagazineSize || Active.Reserve <= 0)
	{
		return;
	}
	bReloading = true;
	bWantsFire = false;
	ReloadRemaining = Active.Stats.ReloadSeconds;
	PlayReloadAudio();
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(Active.Definition.WeaponId))
	{
		if (UNiagaraSystem* Niagara = AshlineLoad::Soft(Visual->ReloadFX))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Niagara, GetOwner()->GetActorLocation(), GetAimRotation());
		}
	}
}

void UAshlineWeaponComponent::SwapWeapon()
{
	if (bReloading)
	{
		return;
	}
	bUsingPrimary = !bUsingPrimary;
	bWantsFire = false;
	RefreshVisuals();
}

void UAshlineWeaponComponent::SetAiming(bool bInAiming)
{
	bAiming = bInAiming;
}

const FAshlineRuntimeWeapon& UAshlineWeaponComponent::GetActiveWeapon() const
{
	return bUsingPrimary ? PrimaryWeapon : SecondaryWeapon;
}

void UAshlineWeaponComponent::FireShot()
{
	FAshlineRuntimeWeapon& Active = bUsingPrimary ? PrimaryWeapon : SecondaryWeapon;
	if (Active.AmmoInMag <= 0)
	{
		Reload();
		return;
	}

	--Active.AmmoInMag;
	FireCooldown = 60.f / FMath::Max(10.f, Active.Stats.FireRateRPM);

	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	if (!World || !Owner)
	{
		return;
	}

	const int32 Pellets = FMath::Clamp(Active.Stats.PelletCount, 1, 12);
	const float DamageEach = Active.Stats.Damage / static_cast<float>(Pellets);
	const float Spread = (bAiming ? Active.Stats.ADSSpread : Active.Stats.HipFireSpread) + SpreadBloom;
	const FVector Start = GetMuzzleLocation();

	for (int32 i = 0; i < Pellets; ++i)
	{
		FRotator Aim = GetAimRotation();
		Aim.Yaw += FMath::FRandRange(-Spread, Spread) + RecoilYawAccum * 0.15f;
		Aim.Pitch += FMath::FRandRange(-Spread * 0.6f, Spread * 0.6f) + RecoilPitchAccum * 0.1f;

		FHitResult Hit;
		const FVector End = Start + Aim.Vector() * TraceDistance;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(AshlineShot), false, Owner);
		World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);

		if (Hit.bBlockingHit)
		{
			if (Hit.GetActor())
			{
				UGameplayStatics::ApplyDamage(Hit.GetActor(), DamageEach, Owner->GetInstigatorController(), Owner, nullptr);
			}
			SpawnImpact(Hit);
		}
	}

	SpawnMuzzleFX();
	PlayFireAudio();
	ApplyRecoil();
	ReportGunshotNoise();
	SpreadBloom = FMath::Min(4.5f, SpreadBloom + (bAiming ? 0.12f : 0.28f));
	KickOffset += FVector(-2.4f, FMath::FRandRange(-0.6f, 0.6f), 0.8f);

	if (UAshlineDualSense* DualSense = GEngine ? GEngine->GetEngineSubsystem<UAshlineDualSense>() : nullptr)
	{
		DualSense->TriggerWeaponFeedback(static_cast<uint8>(Active.Definition.Class), Active.Stats.RecoilPitch);
	}

	UE_LOG(LogAshline, Verbose, TEXT("Fired %s mag=%d reserve=%d"), *Active.Definition.WeaponId.ToString(), Active.AmmoInMag, Active.Reserve);
}

void UAshlineWeaponComponent::ApplyRecoil()
{
	if (!GetOwner())
	{
		return;
	}
	const FAshlineWeaponStats& Stats = GetActiveWeapon().Stats;
	const float AimMul = bAiming ? 0.55f : 1.f;
	RecoilPitchAccum = FMath::Min(8.f, RecoilPitchAccum + Stats.RecoilPitch * 0.22f * AimMul);
	RecoilYawAccum = FMath::Clamp(RecoilYawAccum + FMath::FRandRange(-Stats.RecoilYaw, Stats.RecoilYaw) * 0.35f * AimMul, -4.f, 4.f);
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		PC->AddPitchInput(-Stats.RecoilPitch * 0.12f * AimMul);
		PC->AddYawInput(FMath::FRandRange(-Stats.RecoilYaw, Stats.RecoilYaw) * 0.12f * AimMul);
	}
}

FVector UAshlineWeaponComponent::GetMuzzleLocation() const
{
	if (BarrelMesh)
	{
		return BarrelMesh->GetComponentLocation() + BarrelMesh->GetForwardVector() * 20.f;
	}
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		return Character->GetPawnViewLocation();
	}
	return GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
}

FRotator UAshlineWeaponComponent::GetAimRotation() const
{
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		return Character->GetBaseAimRotation();
	}
	return GetOwner() ? GetOwner()->GetActorRotation() : FRotator::ZeroRotator;
}

void UAshlineWeaponComponent::SpawnMuzzleFX()
{
	MuzzleFlashRemaining = MuzzleFlashSeconds;
	const FVector Muzzle = GetMuzzleLocation();

	if (MuzzleLight)
	{
		MuzzleLight->SetWorldLocation(Muzzle);
		MuzzleLight->SetVisibility(true);
		MuzzleLight->SetIntensity(9000.f);
	}

	UAshlineWeaponVisual* Visual = VisualOverride.Get();
	if (!Visual)
	{
		Visual = UAshlinePresentationLibrary::FindWeaponVisual(GetActiveWeapon().Definition.WeaponId);
	}
	if (Visual)
	{
		if (UNiagaraSystem* Niagara = AshlineLoad::Soft(Visual->MuzzleFX))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Niagara, Muzzle, GetAimRotation());
			return;
		}
		if (UParticleSystem* Cascade = AshlineLoad::Soft(Visual->MuzzleCascadeFX))
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Cascade, Muzzle, GetAimRotation());
			return;
		}
	}

	if (UNiagaraSystem* Niagara = AshlineLoad::Object<UNiagaraSystem>(UAshlineContentManifest::WeaponMuzzleFXPath(GetActiveWeapon().Definition.WeaponId)))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Niagara, Muzzle, GetAimRotation());
		return;
	}
	if (UParticleSystem* Cascade = AshlineLoad::Object<UParticleSystem>(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion")))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Cascade, Muzzle, GetAimRotation(), FVector(0.15f), true);
	}
}

void UAshlineWeaponComponent::SpawnImpact(const FHitResult& Hit)
{
	UMaterialInterface* DecalMat = nullptr;
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(GetActiveWeapon().Definition.WeaponId))
	{
		DecalMat = AshlineLoad::Soft(Visual->ImpactDecal);
	}
	if (!DecalMat)
	{
		DecalMat = UAshlinePresentationLibrary::ResolveImpactDecalMaterial();
	}
	if (DecalMat)
	{
		const FRotator DecalRot = Hit.ImpactNormal.Rotation();
		UGameplayStatics::SpawnDecalAtLocation(
			this,
			DecalMat,
			FVector(ImpactDecalSize, ImpactDecalSize, 4.f),
			Hit.ImpactPoint + Hit.ImpactNormal * 1.5f,
			DecalRot,
			12.f);
	}

	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
			{
				Audio->PlayHit(this, Hit.ImpactPoint);
			}
		}
	}
}

void UAshlineWeaponComponent::AttachVisuals()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return;
	}

	USceneComponent* AttachParent = Character->GetMesh();
	if (UCameraComponent* FPS = Character->FindComponentByClass<UCameraComponent>())
	{
		// Prefer first-person camera named FirstPersonCamera when present.
		TArray<UCameraComponent*> Cameras;
		Character->GetComponents<UCameraComponent>(Cameras);
		for (UCameraComponent* Cam : Cameras)
		{
			if (Cam && Cam->GetFName() == TEXT("FirstPersonCamera"))
			{
				AttachParent = Cam;
				break;
			}
		}
	}

	auto MakeMesh = [Character](const TCHAR* Name) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(Character, Name);
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->SetCastShadow(true);
		Comp->RegisterComponent();
		return Comp;
	};

	if (!WeaponMesh)
	{
		WeaponMesh = MakeMesh(TEXT("AshlineWeaponReceiver"));
	}
	if (!BarrelMesh)
	{
		BarrelMesh = MakeMesh(TEXT("AshlineWeaponBarrel"));
	}
	if (!StockMesh)
	{
		StockMesh = MakeMesh(TEXT("AshlineWeaponStock"));
	}
	if (!MagMesh)
	{
		MagMesh = MakeMesh(TEXT("AshlineWeaponMag"));
	}
	if (!MuzzleLight)
	{
		MuzzleLight = NewObject<UPointLightComponent>(Character, TEXT("AshlineMuzzleLight"));
		MuzzleLight->SetMobility(EComponentMobility::Movable);
		MuzzleLight->SetIntensity(0.f);
		MuzzleLight->SetAttenuationRadius(180.f);
		MuzzleLight->SetLightColor(FLinearColor(1.f, 0.72f, 0.35f));
		MuzzleLight->SetCastShadows(false);
		MuzzleLight->SetVisibility(false);
		MuzzleLight->RegisterComponent();
	}

	if (WeaponMesh)
	{
		WeaponMesh->AttachToComponent(AttachParent, FAttachmentTransformRules::KeepRelativeTransform);
		WeaponMesh->SetRelativeLocation(FVector(28.f, 14.f, -10.f));
		WeaponMesh->SetRelativeRotation(FRotator(0.f, 0.f, 8.f));
	}
	if (BarrelMesh)
	{
		BarrelMesh->AttachToComponent(WeaponMesh ? WeaponMesh : AttachParent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (StockMesh)
	{
		StockMesh->AttachToComponent(WeaponMesh ? WeaponMesh : AttachParent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (MagMesh)
	{
		MagMesh->AttachToComponent(WeaponMesh ? WeaponMesh : AttachParent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (MuzzleLight)
	{
		MuzzleLight->AttachToComponent(BarrelMesh ? BarrelMesh : AttachParent, FAttachmentTransformRules::KeepRelativeTransform);
		MuzzleLight->SetRelativeLocation(FVector(40.f, 0.f, 0.f));
	}
	if (!CharmMesh)
	{
		CharmMesh = MakeMesh(TEXT("AshlineWeaponCharm"));
		CharmMesh->AttachToComponent(WeaponMesh ? WeaponMesh : AttachParent, FAttachmentTransformRules::KeepRelativeTransform);
		CharmMesh->SetRelativeLocation(FVector(-16.f, 4.f, -6.f));
		CharmMesh->SetRelativeScale3D(FVector(0.04f, 0.04f, 0.07f));
		CharmMesh->SetVisibility(false);
	}
}

void UAshlineWeaponComponent::RefreshVisuals()
{
	if (!WeaponMesh)
	{
		AttachVisuals();
	}

	UAshlineWeaponVisual* Visual = AshlineLoad::Soft(VisualOverride);
	if (!Visual)
	{
		Visual = UAshlinePresentationLibrary::FindWeaponVisual(GetActiveWeapon().Definition.WeaponId);
	}
	if (Visual)
	{
		ApplyVisualAsset(Visual);
	}
	else
	{
		BuildCompoundPlaceholder();
	}
	ApplyEquippedSkin();
	ApplyCharm(EquippedCharmId);
}

void UAshlineWeaponComponent::ApplyVisualAsset(UAshlineWeaponVisual* Visual)
{
	if (!Visual || !WeaponMesh)
	{
		BuildCompoundPlaceholder();
		return;
	}

	UStaticMesh* Mesh = AshlineLoad::Soft(Visual->WorldMesh);
	if (!Mesh)
	{
		Mesh = UAshlinePresentationLibrary::LoadStaticMesh({
			UAshlineContentManifest::WeaponMeshPath(GetActiveWeapon().Definition.WeaponId)
		});
	}
	if (!Mesh)
	{
		Mesh = UAshlinePresentationLibrary::ResolveWeaponPlaceholderMesh();
	}
	if (Mesh)
	{
		WeaponMesh->SetStaticMesh(Mesh);
		WeaponMesh->SetRelativeLocation(Visual->FPSOffset);
		WeaponMesh->SetRelativeRotation(Visual->FPSRotation);
		WeaponMesh->SetRelativeScale3D(Visual->MeshScale);
		HipOffset = Visual->FPSOffset;
		AdsOffset = FVector(FMath::Lerp(Visual->FPSOffset.X, 18.f, 0.65f), FMath::Lerp(Visual->FPSOffset.Y, 2.f, 0.8f), Visual->FPSOffset.Z + 4.f);
		if (BarrelMesh)
		{
			BarrelMesh->SetVisibility(false);
		}
		if (StockMesh)
		{
			StockMesh->SetVisibility(false);
		}
		if (MagMesh)
		{
			MagMesh->SetVisibility(false);
		}
	}
	else
	{
		BuildCompoundPlaceholder();
	}
}

void UAshlineWeaponComponent::BuildCompoundPlaceholder()
{
	UStaticMesh* Cube = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* Cyl = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (!Cube || !WeaponMesh)
	{
		return;
	}

	const EAshlineWeaponClass Class = GetActiveWeapon().Definition.Class;
	FVector ReceiverScale(0.28f, 0.09f, 0.1f);
	FVector BarrelScale(0.08f, 0.08f, 0.42f);
	FVector StockScale(0.18f, 0.07f, 0.08f);
	FVector MagScale(0.08f, 0.05f, 0.14f);
	if (Class == EAshlineWeaponClass::SMG || Class == EAshlineWeaponClass::PDW)
	{
		ReceiverScale = FVector(0.22f, 0.08f, 0.09f);
		BarrelScale = FVector(0.06f, 0.06f, 0.28f);
	}
	else if (Class == EAshlineWeaponClass::Sniper || Class == EAshlineWeaponClass::DMR || Class == EAshlineWeaponClass::BattleRifle)
	{
		ReceiverScale = FVector(0.34f, 0.08f, 0.09f);
		BarrelScale = FVector(0.06f, 0.06f, 0.62f);
		StockScale = FVector(0.24f, 0.07f, 0.09f);
	}
	else if (Class == EAshlineWeaponClass::Shotgun)
	{
		ReceiverScale = FVector(0.3f, 0.1f, 0.11f);
		BarrelScale = FVector(0.09f, 0.09f, 0.4f);
	}
	else if (Class == EAshlineWeaponClass::Sidearm)
	{
		ReceiverScale = FVector(0.14f, 0.07f, 0.1f);
		BarrelScale = FVector(0.05f, 0.05f, 0.16f);
		StockScale = FVector(0.08f, 0.06f, 0.1f);
		MagScale = FVector(0.05f, 0.04f, 0.1f);
	}
	else if (Class == EAshlineWeaponClass::LMG)
	{
		ReceiverScale = FVector(0.36f, 0.12f, 0.12f);
		BarrelScale = FVector(0.08f, 0.08f, 0.5f);
		MagScale = FVector(0.16f, 0.1f, 0.18f);
	}

	WeaponMesh->SetStaticMesh(Cube);
	WeaponMesh->SetRelativeScale3D(ReceiverScale);
	WeaponMesh->SetVisibility(true);
	if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeWeaponMaterial(this, FLinearColor(0.07f, 0.07f, 0.08f), FAshlineTextureSet()))
	{
		WeaponMesh->SetMaterial(0, MID);
	}

	if (BarrelMesh && Cyl)
	{
		BarrelMesh->SetStaticMesh(Cyl);
		BarrelMesh->SetVisibility(true);
		BarrelMesh->SetRelativeLocation(FVector(28.f, 0.f, 2.f));
		BarrelMesh->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));
		BarrelMesh->SetRelativeScale3D(BarrelScale);
		if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeWeaponMaterial(this, FLinearColor(0.12f, 0.12f, 0.13f), FAshlineTextureSet()))
		{
			BarrelMesh->SetMaterial(0, MID);
		}
	}
	if (StockMesh)
	{
		StockMesh->SetStaticMesh(Cube);
		StockMesh->SetVisibility(true);
		StockMesh->SetRelativeLocation(FVector(-22.f, 0.f, -2.f));
		StockMesh->SetRelativeScale3D(StockScale);
		if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeWeaponMaterial(this, FLinearColor(0.05f, 0.05f, 0.05f), FAshlineTextureSet()))
		{
			StockMesh->SetMaterial(0, MID);
		}
	}
	if (MagMesh)
	{
		MagMesh->SetStaticMesh(Cube);
		MagMesh->SetVisibility(true);
		MagMesh->SetRelativeLocation(FVector(2.f, 0.f, -10.f));
		MagMesh->SetRelativeScale3D(MagScale);
		if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeWeaponMaterial(this, FLinearColor(0.1f, 0.1f, 0.08f), FAshlineTextureSet()))
		{
			MagMesh->SetMaterial(0, MID);
		}
	}
}

void UAshlineWeaponComponent::ApplyEquippedSkin()
{
	const FName SkinId = GetActiveWeapon().SkinId.IsNone() ? FName(TEXT("SKIN_FACTORY")) : GetActiveWeapon().SkinId;
	FAshlineWeaponSkinDefinition Def;
	UMaterialInterface* Override = nullptr;
	FLinearColor Tint = FLinearColor(0.07f, 0.07f, 0.08f);
	if (UAshlineMetaCatalog::FindSkin(SkinId, Def))
	{
		Tint = Def.Tint;
		Override = UAshlinePresentationLibrary::ResolveSkinMaterial(Def);
		if (!Override)
		{
			Override = AshlineLoad::Soft(Def.MaterialOverride);
		}
	}
	if (!Override)
	{
		if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(GetActiveWeapon().Definition.WeaponId))
		{
			Override = AshlineLoad::Soft(Visual->SkinMaterial);
			if (!Override && Visual->TextureSet.HasAnyAuthoredTexture())
			{
				if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeWeaponMaterial(this, Tint, Visual->TextureSet))
				{
					ApplyTintToWeaponMeshes(Tint, MID);
					return;
				}
			}
		}
	}
	ApplyTintToWeaponMeshes(Tint, Override);
}

void UAshlineWeaponComponent::ApplyCharm(FName CharmId)
{
	EquippedCharmId = CharmId;
	if (!CharmMesh)
	{
		AttachVisuals();
	}
	if (!CharmMesh)
	{
		return;
	}

	if (CharmId.IsNone())
	{
		CharmMesh->SetVisibility(false);
		return;
	}

	UStaticMesh* CharmAsset = nullptr;
	FAshlineCosmeticDefinition CharmDef;
	if (UAshlineMetaCatalog::FindCosmetic(CharmId, CharmDef))
	{
		CharmAsset = UAshlinePresentationLibrary::ResolveCosmeticPartMesh(CharmDef);
	}
	if (!CharmAsset)
	{
		CharmAsset = AshlineLoad::Object<UStaticMesh>(TEXT("/Engine/BasicShapes/Cube.Cube"));
	}
	if (CharmAsset)
	{
		CharmMesh->SetStaticMesh(CharmAsset);
	}
	const FLinearColor Tint = UAshlineMetaCatalog::CosmeticTint(CharmId, FLinearColor(0.7f, 0.55f, 0.2f));
	if (UAshlineMetaCatalog::FindCosmetic(CharmId, CharmDef))
	{
		if (UMaterialInterface* CharmMat = UAshlinePresentationLibrary::ResolveCosmeticMaterial(CharmDef))
		{
			CharmMesh->SetMaterial(0, CharmMat);
			CharmMesh->SetVisibility(true);
			return;
		}
	}
	if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeTintedMaterial(this, EAshlineSurface::Metal, Tint))
	{
		CharmMesh->SetMaterial(0, MID);
	}
	CharmMesh->SetVisibility(true);
}

void UAshlineWeaponComponent::ApplyTintToWeaponMeshes(const FLinearColor& Tint, UMaterialInterface* Override)
{
	auto Paint = [this, Tint, Override](UStaticMeshComponent* Mesh, EAshlineSurface Surface, const FLinearColor& Color)
	{
		if (!Mesh || !Mesh->GetStaticMesh())
		{
			return;
		}
		if (Override)
		{
			Mesh->SetMaterial(0, Override);
			return;
		}
		if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeWeaponMaterial(this, Color, FAshlineTextureSet()))
		{
			Mesh->SetMaterial(0, MID);
		}
	};

	Paint(WeaponMesh, EAshlineSurface::Metal, Tint);
	Paint(BarrelMesh, EAshlineSurface::Metal, Tint * 1.25f);
	Paint(StockMesh, EAshlineSurface::Plastic, Tint * 0.7f);
	Paint(MagMesh, EAshlineSurface::Metal, Tint * 0.9f);
}

void UAshlineWeaponComponent::PlayFireAudio()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
			{
				Audio->PlayFire(this, GetActiveWeapon().Definition.WeaponId, GetMuzzleLocation());
			}
		}
	}
}

void UAshlineWeaponComponent::PlayReloadAudio()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
			{
				Audio->PlayReload(this, GetActiveWeapon().Definition.WeaponId, GetOwner()->GetActorLocation());
			}
		}
	}
}

void UAshlineWeaponComponent::TickCombatFeel(float DeltaTime)
{
	RecoilPitchAccum = FMath::FInterpTo(RecoilPitchAccum, 0.f, DeltaTime, 7.5f);
	RecoilYawAccum = FMath::FInterpTo(RecoilYawAccum, 0.f, DeltaTime, 8.5f);
	SpreadBloom = FMath::FInterpTo(SpreadBloom, 0.f, DeltaTime, bAiming ? 6.5f : 3.8f);
	KickOffset = FMath::VInterpTo(KickOffset, FVector::ZeroVector, DeltaTime, 14.f);
	SwayTime += DeltaTime;
	BobTime += DeltaTime;

	if (!WeaponMesh)
	{
		return;
	}

	const FVector TargetBase = bAiming ? AdsOffset : HipOffset;
	FVector Bob = FVector::ZeroVector;
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		const float Speed2D = Character->GetVelocity().Size2D();
		if (Speed2D > 40.f)
		{
			const float BobScale = bAiming ? 0.35f : 1.f;
			Bob.Z = FMath::Sin(BobTime * 10.f) * 0.55f * BobScale;
			Bob.Y = FMath::Cos(BobTime * 5.f) * 0.35f * BobScale;
		}
	}
	const float SwayScale = bAiming ? 0.15f : 0.45f;
	const FVector Sway(0.f, FMath::Sin(SwayTime * 1.15f) * 0.35f * SwayScale, FMath::Cos(SwayTime * 0.85f) * 0.22f * SwayScale);
	WeaponMesh->SetRelativeLocation(TargetBase + KickOffset + Bob + Sway);
	WeaponMesh->SetRelativeRotation(FRotator(-RecoilPitchAccum * 0.35f, RecoilYawAccum * 0.25f, 8.f));
}

void UAshlineWeaponComponent::ReportGunshotNoise()
{
	if (UWorld* World = GetWorld())
	{
		UAISense_Hearing::ReportNoiseEvent(World, GetMuzzleLocation(), 1.f, GetOwner(), 4500.f, TEXT("Gunshot"));
	}
}
