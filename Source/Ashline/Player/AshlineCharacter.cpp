#include "Player/AshlineCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/GameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Progression/AshlineProgressionSubsystem.h"
#include "Weapons/AshlineWeaponComponent.h"

AAshlineCharacter::AAshlineCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = false;
	MoveComp->MaxWalkSpeed = 430.f;
	MoveComp->NavAgentProps.bCanCrouch = true;
	MoveComp->JumpZVelocity = 420.f;
	MoveComp->AirControl = 0.2f;

	ThirdPersonArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonArm"));
	ThirdPersonArm->SetupAttachment(RootComponent);
	ThirdPersonArm->TargetArmLength = 220.f;
	ThirdPersonArm->SocketOffset = FVector(0.f, 55.f, 55.f);
	ThirdPersonArm->bUsePawnControlRotation = true;
	ThirdPersonArm->bDoCollisionTest = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(ThirdPersonArm, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head"));
	FirstPersonCamera->SetRelativeLocation(FVector(8.f, 0.f, 12.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	WeaponComponent = CreateDefaultSubobject<UAshlineWeaponComponent>(TEXT("WeaponComponent"));
}

void AAshlineCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				SetCameraMode(Save->PreferredCamera);

				int32 PrimaryUpgrade = 0;
				int32 SecondaryUpgrade = 0;
				for (const FAshlineOwnedWeapon& Owned : Save->Armory)
				{
					if (Owned.WeaponId == Save->Primary.WeaponId)
					{
						PrimaryUpgrade = Owned.UpgradeTier;
					}
					if (Owned.WeaponId == Save->Secondary.WeaponId)
					{
						SecondaryUpgrade = Owned.UpgradeTier;
					}
				}
				WeaponComponent->LoadFromLoadout(Save->Primary, Save->Secondary, PrimaryUpgrade, SecondaryUpgrade);
			}
		}
	}

	SetCameraMode(CameraMode);
}

void AAshlineCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const float Target = bIsAiming ? 430.f * AimWalkMul : 430.f;
	GetCharacterMovement()->MaxWalkSpeed = Target;
}

void AAshlineCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAshlineCharacter::Move);
		}
		if (LookAction)
		{
			EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAshlineCharacter::Look);
		}
		if (JumpAction)
		{
			EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (FireAction)
		{
			EIC->BindAction(FireAction, ETriggerEvent::Started, this, &AAshlineCharacter::StartFire);
			EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &AAshlineCharacter::StopFire);
		}
		if (AimAction)
		{
			EIC->BindAction(AimAction, ETriggerEvent::Started, this, &AAshlineCharacter::StartAim);
			EIC->BindAction(AimAction, ETriggerEvent::Completed, this, &AAshlineCharacter::StopAim);
		}
		if (ReloadAction)
		{
			EIC->BindAction(ReloadAction, ETriggerEvent::Started, this, &AAshlineCharacter::Reload);
		}
		if (CameraToggleAction)
		{
			EIC->BindAction(CameraToggleAction, ETriggerEvent::Started, this, &AAshlineCharacter::ToggleCameraMode);
		}
		if (SwapWeaponAction)
		{
			EIC->BindAction(SwapWeaponAction, ETriggerEvent::Started, this, &AAshlineCharacter::SwapWeapon);
		}
		if (CrouchAction)
		{
			EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &AAshlineCharacter::StartCrouch);
			EIC->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AAshlineCharacter::StopCrouch);
		}
	}
}

void AAshlineCharacter::SetCameraMode(EAshlineCameraMode NewMode)
{
	CameraMode = NewMode;
	const bool bFPS = CameraMode == EAshlineCameraMode::FirstPerson;
	if (FirstPersonCamera)
	{
		FirstPersonCamera->SetActive(bFPS);
	}
	if (ThirdPersonCamera)
	{
		ThirdPersonCamera->SetActive(!bFPS);
	}
	if (GetMesh())
	{
		GetMesh()->SetOwnerNoSee(bFPS);
	}
}

void AAshlineCharacter::ToggleCameraMode()
{
	SetCameraMode(CameraMode == EAshlineCameraMode::FirstPerson
		? EAshlineCameraMode::ThirdPerson
		: EAshlineCameraMode::FirstPerson);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				Save->PreferredCamera = CameraMode;
				Progression->SaveCampaign();
			}
		}
	}
}

void AAshlineCharacter::StartFire()
{
	if (WeaponComponent)
	{
		WeaponComponent->StartFire();
	}
}

void AAshlineCharacter::StopFire()
{
	if (WeaponComponent)
	{
		WeaponComponent->StopFire();
	}
}

void AAshlineCharacter::Reload()
{
	if (WeaponComponent)
	{
		WeaponComponent->Reload();
	}
}

void AAshlineCharacter::SetAiming(bool bNewAiming)
{
	bIsAiming = bNewAiming;
	if (WeaponComponent)
	{
		WeaponComponent->SetAiming(bNewAiming);
	}
	if (ThirdPersonArm)
	{
		ThirdPersonArm->TargetArmLength = bIsAiming ? 140.f : 220.f;
	}
}

void AAshlineCharacter::SwapWeapon()
{
	if (WeaponComponent)
	{
		WeaponComponent->SwapWeapon();
	}
}

void AAshlineCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator Yaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Forward = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);
		AddMovementInput(Forward, Axis.Y);
		AddMovementInput(Right, Axis.X);
	}
}

void AAshlineCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AAshlineCharacter::StartAim()
{
	SetAiming(true);
}

void AAshlineCharacter::StopAim()
{
	SetAiming(false);
}

void AAshlineCharacter::StartCrouch()
{
	Crouch();
}

void AAshlineCharacter::StopCrouch()
{
	UnCrouch();
}
