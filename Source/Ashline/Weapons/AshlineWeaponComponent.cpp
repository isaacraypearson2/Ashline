#include "Weapons/AshlineWeaponComponent.h"
#include "Presentation/AshlineLoad.h"

#include "AI/AshlineAICharacter.h"
#include "AI/AshlineAICatalog.h"
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
#include "Meta/AshlineMetaCatalog.h"
#include "Player/AshlineCharacter.h"
#include "Presentation/AshlineAudioDirector.h"
#include "Presentation/AshlineCombatFX.h"
#include "Presentation/AshlineContentManifest.h"
#include "Presentation/AshlinePresentationLibrary.h"
#include "Presentation/AshlineWeaponVisual.h"
#include "Perception/AISense_Hearing.h"
#include "UI/AshlineCombatFeedback.h"
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

	TickCombatFeel(DeltaTime);
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

float UAshlineWeaponComponent::GetReloadAlpha() const
{
	if (!bReloading)
	{
		return 0.f;
	}
	const float Total = FMath::Max(0.05f, GetActiveWeapon().Stats.ReloadSeconds);
	return FMath::Clamp(1.f - (ReloadRemaining / Total), 0.f, 1.f);
}

FString UAshlineWeaponComponent::GetFireModeLabel() const
{
	return UAshlineWeaponCatalog::FireModeName(GetActiveWeapon().Stats.FireMode);
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
	const FRotator BaseAim = GetAimRotation();
	const FVector Start = GetMuzzleLocation();
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AshlineShot), false, Owner);

	bool bFlesh = false;
	bool bKilled = false;
	AAshlineAICharacter* AI = nullptr;
	FHitResult LastHit;
	FVector LastEnd = Start;

	for (int32 i = 0; i < Pellets; ++i)
	{
		const float Spread = CurrentSpread() + SpreadBloom;
		FRotator Aim = BaseAim;
		Aim.Yaw += FMath::FRandRange(-Spread, Spread) + RecoilYawAccum * 0.15f;
		Aim.Pitch += FMath::FRandRange(-Spread * 0.6f, Spread * 0.6f) + RecoilPitchAccum * 0.1f;

		FHitResult Hit;
		const float MaxCm = FMath::Clamp(Active.Stats.RangeMeters * 150.f, 180.f, TraceDistance);
		const FVector End = Start + Aim.Vector() * MaxCm;
		LastEnd = End;
		World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);

		if (Hit.bBlockingHit)
		{
			LastHit = Hit;
			if (Hit.GetActor())
			{
				if (AAshlineAICharacter* HitAI = Cast<AAshlineAICharacter>(Hit.GetActor()))
				{
					AI = HitAI;
					bFlesh = true;
				}
				const float DistM = Hit.Distance / 100.f;
				const float Damage = UAshlineWeaponCatalog::DamageAtRange(Active.Stats, DistM);
				UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, Owner->GetInstigatorController(), Owner, nullptr);
				if (AI)
				{
					bKilled = AI->bDead;
				}
			}
			SpawnImpact(Hit);
		}
	}

	UAshlineCombatFX::SpawnTracer(this, Active.Definition.WeaponId, Start, LastHit.bBlockingHit ? LastHit.ImpactPoint : LastEnd);

	if (UWorld* W = GetWorld())
	{
		if (UAshlineCombatFeedback* Feedback = W->GetSubsystem<UAshlineCombatFeedback>())
		{
			if (LastHit.bBlockingHit && bFlesh)
			{
				Feedback->NotifyHitMarker(bKilled);
				if (bKilled)
				{
					Feedback->NotifyKillConfirm(AI ? AI->ArchetypeDef.DisplayName.ToString() : TEXT("HOSTILE"));
				}
			}
		}
		if (UGameInstance* GI = W->GetGameInstance())
		{
			if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
			{
				if (bFlesh)
				{
					Audio->NotifyCombat(this);
				}
			}
		}
	}

	if (bKilled && AI && AI->Archetype == EAshlineAIArchetype::Heavy)
	{
		UAshlineCombatFX::SpawnExplosion(this, LastHit.ImpactPoint, 0.45f);
	}

	SpawnMuzzleFX();
	PlayFireAudio();
	ApplyRecoil();
	ReportGunshotNoise();
	SpreadBloom = FMath::Min(4.5f, SpreadBloom + (bAiming ? 0.12f : 0.28f));
	KickOffset += FVector(-2.4f, FMath::FRandRange(-0.6f, 0.6f), 0.8f);
	++ConsecutiveShots;

	if (AAshlineCharacter* Shooter = Cast<AAshlineCharacter>(Owner))
	{
		Shooter->NotifyWeaponFired();
	}

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
		const FVector2D Kick = UAshlineWeaponCatalog::RecoilKick(Stats, ConsecutiveShots, bAiming);
		PC->AddPitchInput(-Kick.X * 0.14f);
		PC->AddYawInput(Kick.Y * 0.14f + FMath::FRandRange(-Stats.RecoilYaw, Stats.RecoilYaw) * 0.04f);
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

	UAshlineCombatFX::SpawnMuzzle(this, GetActiveWeapon().Definition.WeaponId, Muzzle, GetAimRotation());
}

void UAshlineWeaponComponent::SpawnImpact(const FHitResult& Hit)
{
	const bool bFlesh = Cast<AAshlineAICharacter>(Hit.GetActor()) != nullptr;
	UAshlineCombatFX::SpawnImpact(this, GetActiveWeapon().Definition.WeaponId, Hit, bFlesh);

	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
			{
				Audio->PlayHit(this, Hit.ImpactPoint, bFlesh);
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
		if (UStaticMesh* SkinMesh = UAshlinePresentationLibrary::ResolveSkinMesh(Def))
		{
			if (WeaponMesh)
			{
				WeaponMesh->SetStaticMesh(SkinMesh);
			}
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

	FVector Bob = FVector::ZeroVector;
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		const float Speed2D = Character->GetVelocity().Size2D();
		if (Speed2D > 40.f)
		{
			const float BobScale = FMath::Lerp(1.f, 0.35f, AdsAlpha);
			Bob.Z = FMath::Sin(BobTime * 10.f) * 0.55f * BobScale;
			Bob.Y = FMath::Cos(BobTime * 5.f) * 0.35f * BobScale;
		}
	}

	const float SwayAmp = FMath::Lerp(Stats.SwayHipDegrees, Stats.SwayAdsDegrees, AdsAlpha);
	float MoveMul = 1.f;
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		MoveMul = 1.f + Character->GetVelocity().Size2D() / 600.f;
	}
	const float SwayY = FMath::Sin(SwayTime * 1.7f) * SwayAmp * 0.35f * MoveMul;
	const float SwayZ = FMath::Cos(SwayTime * 1.15f) * SwayAmp * 0.22f * MoveMul;
	const FVector Target = FMath::Lerp(HipOffset, AdsOffset, AdsAlpha) + KickOffset + Bob + FVector(0.f, SwayY, SwayZ);
	WeaponMesh->SetRelativeLocation(Target);
	WeaponMesh->SetRelativeRotation(FRotator(-RecoilPitchAccum * 0.35f, RecoilYawAccum * 0.25f, 8.f));
}

void UAshlineWeaponComponent::ReportGunshotNoise()
{
	if (UWorld* World = GetWorld())
	{
		UAISense_Hearing::ReportNoiseEvent(World, GetMuzzleLocation(), 1.f, GetOwner(), 4500.f, TEXT("Gunshot"));
	}
}
