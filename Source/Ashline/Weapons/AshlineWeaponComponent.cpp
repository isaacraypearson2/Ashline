#include "Weapons/AshlineWeaponComponent.h"

#include "Ashline.h"
#include "AshlineDualSense.h"
#include "Camera/CameraComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/AshlineWeaponCatalog.h"

UAshlineWeaponComponent::UAshlineWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAshlineWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
		return Runtime;
	};

	PrimaryWeapon = Build(Primary, PrimaryUpgrade);
	SecondaryWeapon = Build(Secondary, SecondaryUpgrade);
	bUsingPrimary = true;
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
}

void UAshlineWeaponComponent::SwapWeapon()
{
	if (bReloading)
	{
		return;
	}
	bUsingPrimary = !bUsingPrimary;
	bWantsFire = false;
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

	const float Spread = bAiming ? Active.Stats.ADSSpread : Active.Stats.HipFireSpread;
	FRotator Aim = GetAimRotation();
	Aim.Yaw += FMath::FRandRange(-Spread, Spread);
	Aim.Pitch += FMath::FRandRange(-Spread * 0.6f, Spread * 0.6f);

	FHitResult Hit;
	const FVector Start = GetMuzzleLocation();
	const FVector End = Start + Aim.Vector() * TraceDistance;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AshlineShot), false, Owner);
	World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);

	if (Hit.bBlockingHit && Hit.GetActor())
	{
		UGameplayStatics::ApplyDamage(Hit.GetActor(), Active.Stats.Damage, Owner->GetInstigatorController(), Owner, nullptr);
	}

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
