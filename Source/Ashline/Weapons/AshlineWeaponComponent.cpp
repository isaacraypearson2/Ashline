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
#include "GameFramework/CharacterMovementComponent.h"
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
	TickGunfeel(DeltaTime);
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
		const EAshlineFireMode Mode = GetActiveWeapon().Stats.FireMode;
		if (Mode == EAshlineFireMode::Semi)
		{
			bWantsFire = false;
		}
		else if (Mode == EAshlineFireMode::Burst)
		{
			--BurstRemaining;
			if (BurstRemaining <= 0)
			{
				bWantsFire = false;
			}
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
		UAshlineWeaponCatalog::ApplyAmmoType(Runtime.Stats, Slot.AmmoType);
		if (Runtime.Definition.SupportedFireModes.Contains(Slot.FireMode))
		{
			Runtime.Stats.FireMode = Slot.FireMode;
		}
		else if (Runtime.Definition.SupportedFireModes.Num() > 0)
		{
			Runtime.Stats.FireMode = Runtime.Definition.SupportedFireModes[0];
		}
		Runtime.Stats.bAutomatic = Runtime.Stats.FireMode == EAshlineFireMode::Auto;
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
	const FAshlineRuntimeWeapon& Active = GetActiveWeapon();
	if (Active.Stats.FireMode == EAshlineFireMode::Burst)
	{
		BurstRemaining = FMath::Max(1, Active.Stats.BurstCount);
	}
}

void UAshlineWeaponComponent::StopFire()
{
	bWantsFire = false;
	if (GetActiveWeapon().Stats.FireMode != EAshlineFireMode::Burst)
	{
		BurstRemaining = 0;
	}
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
	BurstRemaining = 0;
	ConsecutiveShots = 0;
	const bool bEmpty = Active.AmmoInMag <= 0;
	const float Mul = bEmpty ? Active.Stats.EmptyReloadMul : Active.Stats.TacticalReloadMul;
	ReloadRemaining = Active.Stats.ReloadSeconds * Mul;
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
	BurstRemaining = 0;
	ConsecutiveShots = 0;
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

	const int32 Pellets = FMath::Max(1, Active.Stats.PelletCount);
	const FRotator BaseAim = GetAimRotation();
	const FVector Start = GetMuzzleLocation();
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AshlineShot), false, Owner);

	for (int32 Pellet = 0; Pellet < Pellets; ++Pellet)
	{
		const float Spread = CurrentSpread();
		FRotator Aim = BaseAim;
		Aim.Yaw += FMath::FRandRange(-Spread, Spread);
		Aim.Pitch += FMath::FRandRange(-Spread * 0.6f, Spread * 0.6f);

		FHitResult Hit;
		const float MaxCm = FMath::Clamp(Active.Stats.RangeMeters * 150.f, 180.f, TraceDistance);
		const FVector End = Start + Aim.Vector() * MaxCm;
		World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);

		if (Hit.bBlockingHit)
		{
			const float DistM = Hit.Distance / 100.f;
			float Damage = UAshlineWeaponCatalog::DamageAtRange(Active.Stats, DistM);
			if (Pellets > 1)
			{
				Damage = Damage; // catalog damage is per-pellet for shotguns
			}
			if (Hit.GetActor())
			{
				UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, Owner->GetInstigatorController(), Owner, nullptr);
			}
			SpawnImpact(Hit);
		}
	}

	SpawnMuzzleFX();
	PlayFireAudio();
	ApplyRecoil();
	++ConsecutiveShots;

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
		const FVector2D Kick = UAshlineWeaponCatalog::RecoilKick(Stats, ConsecutiveShots, bAiming);
		const float Pitch = Kick.X * 0.14f;
		const float Yaw = Kick.Y * 0.14f + FMath::FRandRange(-Stats.RecoilYaw, Stats.RecoilYaw) * 0.04f;
		PC->AddPitchInput(-Pitch);
		PC->AddYawInput(Yaw);
		RecoilPitchAccum += Pitch;
		RecoilYawAccum += Yaw;
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

	if (UNiagaraSystem* Niagara = LoadObject<UNiagaraSystem>(nullptr, *UAshlineContentManifest::WeaponMuzzleFXPath(GetActiveWeapon().Definition.WeaponId), nullptr, LOAD_NoWarn | LOAD_Quiet))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Niagara, Muzzle, GetAimRotation());
		return;
	}
	if (UParticleSystem* Cascade = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"), nullptr, LOAD_NoWarn | LOAD_Quiet))
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
	UStaticMesh* Cube = UAshlinePresentationLibrary::LoadStaticMesh({ TEXT("/Engine/BasicShapes/Cube.Cube") });
	UStaticMesh* Cyl = UAshlinePresentationLibrary::LoadStaticMesh({ TEXT("/Engine/BasicShapes/Cylinder.Cylinder") });
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
	else if (Class == EAshlineWeaponClass::Launcher)
	{
		ReceiverScale = FVector(0.32f, 0.14f, 0.14f);
		BarrelScale = FVector(0.12f, 0.12f, 0.55f);
		StockScale = FVector(0.16f, 0.08f, 0.1f);
		MagScale = FVector(0.1f, 0.1f, 0.1f);
	}
	else if (Class == EAshlineWeaponClass::Melee)
	{
		ReceiverScale = FVector(0.04f, 0.03f, 0.18f);
		BarrelScale = FVector(0.03f, 0.01f, 0.28f);
		StockScale = FVector(0.05f, 0.03f, 0.08f);
		MagScale = FVector(0.02f, 0.02f, 0.02f);
	}

	WeaponMesh->SetStaticMesh(Cube);
	WeaponMesh->SetRelativeScale3D(ReceiverScale);
	WeaponMesh->SetVisibility(true);
	if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeWeaponMaterial(this, Class, FLinearColor(0.07f, 0.07f, 0.08f)))
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
		if (UStaticMesh* SkinMesh = UAshlinePresentationLibrary::ResolveSkinMesh(Def))
		{
			if (WeaponMesh)
			{
				WeaponMesh->SetStaticMesh(SkinMesh);
			}
		}
	}
	if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeSkinMaterialInstance(this, Def))
	{
		ApplyTintToWeaponMeshes(Tint, MID);
		return;
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
		CharmAsset = UAshlinePresentationLibrary::LoadStaticMesh({ TEXT("/Engine/BasicShapes/Cube.Cube") });
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

void UAshlineWeaponComponent::CycleFireMode()
{
	FAshlineRuntimeWeapon& Active = MutableActive();
	Active.Stats.FireMode = UAshlineWeaponCatalog::CycleFireMode(Active.Definition, Active.Stats.FireMode);
	Active.Stats.bAutomatic = Active.Stats.FireMode == EAshlineFireMode::Auto;
	BurstRemaining = 0;
	ConsecutiveShots = 0;
}

EAshlineFireMode UAshlineWeaponComponent::GetFireMode() const
{
	return GetActiveWeapon().Stats.FireMode;
}

EAshlineAmmoType UAshlineWeaponComponent::GetAmmoType() const
{
	return GetActiveWeapon().Stats.AmmoType;
}

FAshlineRuntimeWeapon& UAshlineWeaponComponent::MutableActive()
{
	return bUsingPrimary ? PrimaryWeapon : SecondaryWeapon;
}

float UAshlineWeaponComponent::CurrentSpread() const
{
	const FAshlineWeaponStats& Stats = GetActiveWeapon().Stats;
	float Spread = bAiming ? Stats.ADSSpread : Stats.HipFireSpread;
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		const float Speed = Character->GetVelocity().Size2D();
		if (Speed > 40.f)
		{
			Spread *= Stats.MovementSpreadMul;
		}
		if (Character->bIsCrouched)
		{
			Spread *= Stats.CrouchSpreadMul;
		}
	}
	Spread *= 1.f + ConsecutiveShots * 0.04f;
	return Spread;
}

void UAshlineWeaponComponent::TickGunfeel(float DeltaTime)
{
	const FAshlineWeaponStats& Stats = GetActiveWeapon().Stats;
	if (!bWantsFire || FireCooldown > 0.12f)
	{
		ConsecutiveShots = FMath::Max(0, ConsecutiveShots - 1);
		const float Recover = Stats.RecoilRecoveryPerSecond * DeltaTime;
		RecoilPitchAccum = FMath::Max(0.f, RecoilPitchAccum - Recover * 0.12f);
		RecoilYawAccum = FMath::Lerp(RecoilYawAccum, 0.f, FMath::Clamp(Recover * 0.08f, 0.f, 1.f));
		if (APlayerController* PC = Cast<APlayerController>(GetOwner() ? GetOwner()->GetInstigatorController() : nullptr))
		{
			if (RecoilPitchAccum > 0.f)
			{
				const float Step = FMath::Min(RecoilPitchAccum, Recover * 0.08f);
				PC->AddPitchInput(Step);
				RecoilPitchAccum -= Step;
			}
		}
	}
	ApplySwayAndAdsPose(DeltaTime);
}

void UAshlineWeaponComponent::ApplySwayAndAdsPose(float DeltaTime)
{
	if (!WeaponMesh)
	{
		return;
	}
	const FAshlineWeaponStats& Stats = GetActiveWeapon().Stats;
	const float AdsTarget = bAiming ? 1.f : 0.f;
	const float AdsSpeed = 1.f / FMath::Max(0.08f, Stats.ADSTimeSeconds);
	AdsAlpha = FMath::FInterpTo(AdsAlpha, AdsTarget, DeltaTime, AdsSpeed * 6.f);

	SwayTime += DeltaTime * Stats.SwaySpeed;
	const float SwayAmp = FMath::Lerp(Stats.SwayHipDegrees, Stats.SwayAdsDegrees, AdsAlpha);
	float MoveMul = 1.f;
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		MoveMul = 1.f + Character->GetVelocity().Size2D() / 600.f;
	}
	const float SwayY = FMath::Sin(SwayTime * 1.7f) * SwayAmp * 0.35f * MoveMul;
	const float SwayZ = FMath::Cos(SwayTime * 1.15f) * SwayAmp * 0.22f * MoveMul;

	const FVector Target = FMath::Lerp(HipOffset, AdsOffset, AdsAlpha) + FVector(0.f, SwayY, SwayZ);
	WeaponMesh->SetRelativeLocation(Target);
}
