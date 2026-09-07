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
#include "Presentation/AshlinePresentationLibrary.h"
#include "Presentation/AshlineWeaponVisual.h"
#include "Meta/AshlineMetaCatalog.h"
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
	if (WeaponMesh)
	{
		const FVector Base = WeaponMesh->GetRelativeLocation();
		WeaponMesh->SetRelativeLocation(bAiming ? FVector(22.f, 4.f, -6.f) : FVector(28.f, 14.f, -10.f));
		(void)Base;
	}
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

	const float Spread = bAiming ? Active.Stats.ADSSpread : Active.Stats.HipFireSpread;
	FRotator Aim = GetAimRotation();
	Aim.Yaw += FMath::FRandRange(-Spread, Spread);
	Aim.Pitch += FMath::FRandRange(-Spread * 0.6f, Spread * 0.6f);

	FHitResult Hit;
	const FVector Start = GetMuzzleLocation();
	const FVector End = Start + Aim.Vector() * TraceDistance;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AshlineShot), false, Owner);
	World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);

	if (Hit.bBlockingHit)
	{
		if (Hit.GetActor())
		{
			UGameplayStatics::ApplyDamage(Hit.GetActor(), Active.Stats.Damage, Owner->GetInstigatorController(), Owner, nullptr);
		}
		SpawnImpact(Hit);
	}

	SpawnMuzzleFX();
	PlayFireAudio();
	ApplyRecoil();

	if (UAshlineDualSense* DualSense = GEngine ? GEngine->GetEngineSubsystem<UAshlineDualSense>() : nullptr)
	{
		DualSense->TriggerWeaponFeedback(static_cast<uint8>(Active.Definition.Class), Active.Stats.RecoilPitch);
	}

	UE_LOG(LogAshline, Verbose, TEXT("Fired %s mag=%d reserve=%d"), *Active.Definition.WeaponId.ToString(), Active.AmmoInMag, Active.Reserve);
}

void UAshlineWeaponComponent::ApplyRecoil()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		const FAshlineWeaponStats& Stats = GetActiveWeapon().Stats;
		PC->AddPitchInput(-Stats.RecoilPitch * 0.15f);
		PC->AddYawInput(FMath::FRandRange(-Stats.RecoilYaw, Stats.RecoilYaw) * 0.15f);
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
		if (UNiagaraSystem* Niagara = Visual->MuzzleFX.LoadSynchronous())
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Niagara, Muzzle, GetAimRotation());
			return;
		}
		if (UParticleSystem* Cascade = Visual->MuzzleCascadeFX.LoadSynchronous())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Cascade, Muzzle, GetAimRotation());
			return;
		}
	}

	if (UNiagaraSystem* Niagara = LoadObject<UNiagaraSystem>(nullptr, *UAshlineContentManifest::WeaponMuzzleFXPath(GetActiveWeapon().Definition.WeaponId)))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Niagara, Muzzle, GetAimRotation());
		return;
	}
	if (UParticleSystem* Cascade = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion")))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Cascade, Muzzle, GetAimRotation(), FVector(0.15f), true);
	}
}

void UAshlineWeaponComponent::SpawnImpact(const FHitResult& Hit)
{
	UMaterialInterface* DecalMat = nullptr;
	if (UAshlineWeaponVisual* Visual = UAshlinePresentationLibrary::FindWeaponVisual(GetActiveWeapon().Definition.WeaponId))
	{
		DecalMat = Visual->ImpactDecal.LoadSynchronous();
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

	UAshlineWeaponVisual* Visual = VisualOverride.LoadSynchronous();
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

	UStaticMesh* Mesh = Visual->WorldMesh.LoadSynchronous();
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
	UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* Cyl = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (!Cube || !WeaponMesh)
	{
		return;
	}

	const EAshlineWeaponClass Class = GetActiveWeapon().Definition.Class;
	FVector ReceiverScale(0.28f, 0.09f, 0.1f);
	FVector BarrelScale(0.08f, 0.08f, 0.42f);
	FVector StockScale(0.18f, 0.07f, 0.08f);
	FVector MagScale(0.08f, 0.05f, 0.14f);
	if (Class == EAshlineWeaponClass::SMG)
	{
		ReceiverScale = FVector(0.22f, 0.08f, 0.09f);
		BarrelScale = FVector(0.06f, 0.06f, 0.28f);
	}
	else if (Class == EAshlineWeaponClass::Sniper || Class == EAshlineWeaponClass::DMR)
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
	if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeTintedMaterial(this, EAshlineSurface::Metal, FLinearColor(0.07f, 0.07f, 0.08f)))
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
		if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeTintedMaterial(this, EAshlineSurface::Metal, FLinearColor(0.12f, 0.12f, 0.13f)))
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
		if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeTintedMaterial(this, EAshlineSurface::Plastic, FLinearColor(0.05f, 0.05f, 0.05f)))
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
		if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeTintedMaterial(this, EAshlineSurface::Metal, FLinearColor(0.1f, 0.1f, 0.08f)))
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
			Override = Def.MaterialOverride.LoadSynchronous();
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
		CharmAsset = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
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
		if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeTintedMaterial(this, Surface, Color))
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
