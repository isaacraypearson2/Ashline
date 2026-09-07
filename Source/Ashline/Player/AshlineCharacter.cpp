#include "Player/AshlineCharacter.h"

#include "AI/AshlineAICatalog.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/GameInstance.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Game/AshlineGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/AshlineRuntimeInput.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Player/AshlinePlayerController.h"
#include "Progression/AshlineProgressionSubsystem.h"
#include "Meta/AshlineMetaCatalog.h"
#include "Presentation/AshlineAudioDirector.h"
#include "Presentation/AshlineCharacterPresentation.h"
#include "Presentation/AshlinePresentationLibrary.h"
#include "Presentation/AshlinePresentationSettings.h"
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
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(12.f, 0.f, 64.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	GrayboxBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrayboxBody"));
	GrayboxBody->SetupAttachment(GetCapsuleComponent());
	GrayboxBody->SetRelativeLocation(FVector(0.f, 0.f, -20.f));
	GrayboxBody->SetRelativeScale3D(FVector(0.55f, 0.55f, 1.35f));
	GrayboxBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GrayboxWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrayboxWeapon"));
	GrayboxWeapon->SetupAttachment(FirstPersonCamera);
	GrayboxWeapon->SetRelativeLocation(FVector(28.f, 14.f, -10.f));
	GrayboxWeapon->SetRelativeRotation(FRotator(0.f, 0.f, 8.f));
	GrayboxWeapon->SetRelativeScale3D(FVector(0.35f, 0.12f, 0.12f));
	GrayboxWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponComponent = CreateDefaultSubobject<UAshlineWeaponComponent>(TEXT("WeaponComponent"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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

	EnsureDefaultLoadout();
	UAshlinePresentationLibrary::HideCapsuleVisual(this);
	ApplyPresentationMesh();
	ApplyGrayboxMeshes();
	ApplyOperatorLook();
	Health = MaxHealth;
	SetCameraMode(CameraMode);
}

void AAshlineCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const float Target = bIsAiming ? 430.f * AimWalkMul : 430.f;
	GetCharacterMovement()->MaxWalkSpeed = Target;
	TickFootsteps(DeltaSeconds);
}

void AAshlineCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	ApplyRuntimeInputActions();

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
	else
	{
		BindLegacyKeys(PlayerInputComponent);
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
	if (GrayboxBody)
	{
		GrayboxBody->SetOwnerNoSee(bFPS);
	}
	if (GrayboxWeapon)
	{
		GrayboxWeapon->SetVisibility(bFPS);
	}

	TArray<UStaticMeshComponent*> Meshes;
	GetComponents<UStaticMeshComponent>(Meshes);
	for (UStaticMeshComponent* MeshComp : Meshes)
	{
		if (MeshComp && (MeshComp->GetName().StartsWith(TEXT("AshlineBlock_")) || MeshComp->GetName().StartsWith(TEXT("AshlineCloth_"))))
		{
			MeshComp->SetOwnerNoSee(bFPS);
		}
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

float AAshlineCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	float Incoming = Applied > 0.f ? Applied : DamageAmount;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				Incoming *= UAshlineAICatalog::GetDifficulty(Save->Difficulty).PlayerDamageTakenMul;
			}
		}
	}

	Health = FMath::Max(0.f, Health - Incoming);
	if (Health <= 0.f)
	{
		Health = MaxHealth;
		if (AAshlineGameMode* GameMode = Cast<AAshlineGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GameMode->RespawnPlayer(this);
		}
	}
	return Incoming;
}

void AAshlineCharacter::ApplyRuntimeInputActions()
{
	AAshlinePlayerController* PC = Cast<AAshlinePlayerController>(Controller);
	if (!PC)
	{
		PC = Cast<AAshlinePlayerController>(GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr);
	}
	if (!PC)
	{
		return;
	}

	if (UAshlineRuntimeInput* Input = PC->GetOrCreateRuntimeInput())
	{
		if (!MoveAction)
		{
			MoveAction = Input->Move;
		}
		if (!LookAction)
		{
			LookAction = Input->Look;
		}
		if (!JumpAction)
		{
			JumpAction = Input->Jump;
		}
		if (!FireAction)
		{
			FireAction = Input->Fire;
		}
		if (!AimAction)
		{
			AimAction = Input->Aim;
		}
		if (!ReloadAction)
		{
			ReloadAction = Input->Reload;
		}
		if (!CameraToggleAction)
		{
			CameraToggleAction = Input->CameraToggle;
		}
		if (!SwapWeaponAction)
		{
			SwapWeaponAction = Input->SwapWeapon;
		}
		if (!CrouchAction)
		{
			CrouchAction = Input->Crouch;
		}
	}
}

void AAshlineCharacter::BindLegacyKeys(UInputComponent* PlayerInputComponent)
{
	if (!PlayerInputComponent)
	{
		return;
	}

	PlayerInputComponent->BindAxisKey(EKeys::W, this, &AAshlineCharacter::LegacyMoveForward);
	PlayerInputComponent->BindAxisKey(EKeys::S, this, &AAshlineCharacter::LegacyMoveBack);
	PlayerInputComponent->BindAxisKey(EKeys::D, this, &AAshlineCharacter::LegacyMoveRight);
	PlayerInputComponent->BindAxisKey(EKeys::A, this, &AAshlineCharacter::LegacyMoveLeft);
	PlayerInputComponent->BindAxisKey(EKeys::MouseX, this, &AAshlineCharacter::LegacyLookYaw);
	PlayerInputComponent->BindAxisKey(EKeys::MouseY, this, &AAshlineCharacter::LegacyLookPitch);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AAshlineCharacter::StartFire);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AAshlineCharacter::StopFire);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AAshlineCharacter::StartAim);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &AAshlineCharacter::StopAim);
	PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AAshlineCharacter::Reload);
	PlayerInputComponent->BindKey(EKeys::V, IE_Pressed, this, &AAshlineCharacter::ToggleCameraMode);
	PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AAshlineCharacter::SwapWeapon);
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &AAshlineCharacter::SwapWeapon);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AAshlineCharacter::SwapWeapon);
	PlayerInputComponent->BindKey(EKeys::C, IE_Pressed, this, &AAshlineCharacter::StartCrouch);
	PlayerInputComponent->BindKey(EKeys::C, IE_Released, this, &AAshlineCharacter::StopCrouch);
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Pressed, this, &AAshlineCharacter::StartCrouch);
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Released, this, &AAshlineCharacter::StopCrouch);
}

void AAshlineCharacter::LegacyMoveForward(float Value)
{
	if (Controller && !FMath::IsNearlyZero(Value))
	{
		const FRotator Yaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
		AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X), Value);
	}
}

void AAshlineCharacter::LegacyMoveRight(float Value)
{
	if (Controller && !FMath::IsNearlyZero(Value))
	{
		const FRotator Yaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
		AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Value);
	}
}

void AAshlineCharacter::LegacyMoveBack(float Value)
{
	LegacyMoveForward(-Value);
}

void AAshlineCharacter::LegacyMoveLeft(float Value)
{
	LegacyMoveRight(-Value);
}

void AAshlineCharacter::LegacyLookYaw(float Value)
{
	AddControllerYawInput(Value);
}

void AAshlineCharacter::LegacyLookPitch(float Value)
{
	AddControllerPitchInput(Value);
}

void AAshlineCharacter::EnsureDefaultLoadout()
{
	if (!WeaponComponent)
	{
		return;
	}
	if (!WeaponComponent->GetActiveWeapon().Definition.WeaponId.IsNone())
	{
		return;
	}

	FAshlineLoadoutSlot Primary;
	FAshlineLoadoutSlot Secondary;
	Primary.WeaponId = TEXT("WPN_AR_ASH16");
	Primary.SkinId = TEXT("SKIN_FACTORY");
	Secondary.WeaponId = TEXT("WPN_PIS_M17A");
	Secondary.SkinId = TEXT("SKIN_FACTORY");
	WeaponComponent->LoadFromLoadout(Primary, Secondary, 0, 0);
}

void AAshlineCharacter::ApplyPresentationMesh()
{
	USkeletalMesh* Hero = nullptr;
	FVector RelLoc(0.f, 0.f, -96.f);
	FRotator RelRot(0.f, -90.f, 0.f);
	FVector RelScale = FVector::OneVector;

	if (!HeroMeshOverride.IsNull())
	{
		Hero = HeroMeshOverride.LoadSynchronous();
	}
	if (!Hero)
	{
		if (const UAshlinePresentationSettings* Settings = GetDefault<UAshlinePresentationSettings>())
		{
			Hero = Settings->DefaultHeroMesh.LoadSynchronous();
			if (!Hero && Settings->HeroPresentation.IsValid())
			{
				if (UAshlineCharacterPresentation* Pres = Settings->HeroPresentation.LoadSynchronous())
				{
					Hero = Pres->BodyMesh.LoadSynchronous();
					RelLoc = Pres->MeshRelativeLocation;
					RelRot = Pres->MeshRelativeRotation;
					RelScale = Pres->MeshScale;
				}
			}
		}
	}
	if (!Hero)
	{
		if (UAshlineCharacterPresentation* Pres = UAshlinePresentationLibrary::FindCharacterPresentation(true, EAshlineAIArchetype::Rifleman))
		{
			Hero = Pres->BodyMesh.LoadSynchronous();
			RelLoc = Pres->MeshRelativeLocation;
			RelRot = Pres->MeshRelativeRotation;
			RelScale = Pres->MeshScale;
		}
	}
	if (!Hero)
	{
		Hero = UAshlinePresentationLibrary::ResolveHumanoidMesh();
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				const FName CamoId = UAshlineMetaCatalog::EquippedCosmeticId(Save->Operator, EAshlineCosmeticSlot::Camo);
				FAshlineCosmeticDefinition CamoDef;
				if (UAshlineMetaCatalog::FindCosmetic(CamoId, CamoDef))
				{
					if (USkeletalMesh* OverrideMesh = CamoDef.MeshOverride.LoadSynchronous())
					{
						Hero = OverrideMesh;
					}
				}
			}
		}
	}

	if (Hero && GetMesh())
	{
		GetMesh()->SetSkeletalMeshAsset(Hero);
		GetMesh()->SetRelativeLocation(RelLoc);
		GetMesh()->SetRelativeRotation(RelRot);
		GetMesh()->SetRelativeScale3D(RelScale);
		GetMesh()->SetVisibility(true);
		GetMesh()->SetCastShadow(true);
		if (GrayboxBody)
		{
			GrayboxBody->SetVisibility(false);
		}
	}
}

void AAshlineCharacter::TickFootsteps(float DeltaSeconds)
{
	const float Speed = GetVelocity().Size2D();
	if (Speed < 40.f || !GetCharacterMovement() || GetCharacterMovement()->IsFalling())
	{
		FootstepAccumulator = 0.f;
		return;
	}
	FootstepAccumulator += DeltaSeconds * (Speed / 280.f);
	if (FootstepAccumulator >= 1.f)
	{
		FootstepAccumulator = 0.f;
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UAshlineAudioDirector* Audio = GI->GetSubsystem<UAshlineAudioDirector>())
			{
				Audio->PlayFootstep(this, GetActorLocation());
			}
		}
	}
}

void AAshlineCharacter::ApplyGrayboxMeshes()
{
	const bool bHasHeroMesh = GetMesh() && GetMesh()->GetSkeletalMeshAsset() != nullptr;
	if (GrayboxBody)
	{
		GrayboxBody->SetVisibility(!bHasHeroMesh);
	}

	// Weapon visuals live on UAshlineWeaponComponent. Hide the old single-cube prop when present.
	if (GrayboxWeapon)
	{
		GrayboxWeapon->SetVisibility(false);
	}

	if (bHasHeroMesh)
	{
		return;
	}

	if (GrayboxBody)
	{
		GrayboxBody->SetVisibility(false);
	}
	UAshlinePresentationLibrary::ApplyHumanoidBlockout(this, FLinearColor(0.18f, 0.24f, 0.16f));
}

void AAshlineCharacter::ApplyOperatorLook()
{
	FAshlineOperatorProfile Profile;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAshlineProgressionSubsystem* Progression = GI->GetSubsystem<UAshlineProgressionSubsystem>())
		{
			if (UAshlineSaveGame* Save = Progression->GetSave())
			{
				Profile = Save->Operator;
			}
		}
	}

	const FName CamoId = UAshlineMetaCatalog::EquippedCosmeticId(Profile, EAshlineCosmeticSlot::Camo);
	const FName HelmetId = UAshlineMetaCatalog::EquippedCosmeticId(Profile, EAshlineCosmeticSlot::Helmet);
	const FName VestId = UAshlineMetaCatalog::EquippedCosmeticId(Profile, EAshlineCosmeticSlot::Vest);
	const FName PantsId = UAshlineMetaCatalog::EquippedCosmeticId(Profile, EAshlineCosmeticSlot::Pants);
	const FName GlovesId = UAshlineMetaCatalog::EquippedCosmeticId(Profile, EAshlineCosmeticSlot::Gloves);
	const FName BootsId = UAshlineMetaCatalog::EquippedCosmeticId(Profile, EAshlineCosmeticSlot::Boots);
	const FName FaceId = UAshlineMetaCatalog::EquippedCosmeticId(Profile, EAshlineCosmeticSlot::Face);
	const FName CharmId = UAshlineMetaCatalog::EquippedCosmeticId(Profile, EAshlineCosmeticSlot::Charm);

	const FLinearColor CamoTint = UAshlineMetaCatalog::CosmeticTint(CamoId, FLinearColor(0.18f, 0.24f, 0.16f));
	const FLinearColor HelmetTint = UAshlineMetaCatalog::CosmeticTint(HelmetId, CamoTint * 0.65f);
	const FLinearColor VestTint = UAshlineMetaCatalog::CosmeticTint(VestId, CamoTint);
	const FLinearColor PantsTint = UAshlineMetaCatalog::CosmeticTint(PantsId, CamoTint * 0.85f);
	const FLinearColor GlovesTint = UAshlineMetaCatalog::CosmeticTint(GlovesId, CamoTint * 0.55f);
	const FLinearColor BootsTint = UAshlineMetaCatalog::CosmeticTint(BootsId, FLinearColor(0.1f, 0.08f, 0.06f));
	const FLinearColor FaceTint = UAshlineMetaCatalog::CosmeticTint(FaceId, FLinearColor(0.45f, 0.34f, 0.26f));

	const bool bHasHeroMesh = GetMesh() && GetMesh()->GetSkeletalMeshAsset() != nullptr;
	if (bHasHeroMesh)
	{
		FAshlineCosmeticDefinition CamoDef;
		UMaterialInterface* Override = nullptr;
		if (UAshlineMetaCatalog::FindCosmetic(CamoId, CamoDef))
		{
			if (USkeletalMesh* CamoMesh = UAshlinePresentationLibrary::ResolveCosmeticMesh(CamoDef))
			{
				GetMesh()->SetSkeletalMeshAsset(CamoMesh);
			}
			Override = UAshlinePresentationLibrary::ResolveCosmeticMaterial(CamoDef);
		}
		if (Override)
		{
			GetMesh()->SetMaterial(0, Override);
		}
		else if (UMaterialInstanceDynamic* MID = UAshlinePresentationLibrary::MakeTintedMaterial(this, EAshlineSurface::Plastic, CamoTint))
		{
			GetMesh()->SetMaterial(0, MID);
		}
	}
	else
	{
		UAshlinePresentationLibrary::ApplyHumanoidBlockout(this, CamoTint);
		UAshlinePresentationLibrary::TintNamedStaticMesh(this, TEXT("AshlineBlock_Torso"), VestTint, EAshlineSurface::Plastic);
		UAshlinePresentationLibrary::TintNamedStaticMesh(this, TEXT("AshlineBlock_Helmet"), HelmetTint, EAshlineSurface::Plastic);
		UAshlinePresentationLibrary::TintNamedStaticMesh(this, TEXT("AshlineBlock_Head"), FaceTint, EAshlineSurface::Plastic);
		UAshlinePresentationLibrary::TintNamedStaticMesh(this, TEXT("AshlineBlock_ArmL"), GlovesTint, EAshlineSurface::Plastic);
		UAshlinePresentationLibrary::TintNamedStaticMesh(this, TEXT("AshlineBlock_ArmR"), GlovesTint, EAshlineSurface::Plastic);
		UAshlinePresentationLibrary::TintNamedStaticMesh(this, TEXT("AshlineBlock_LegL"), PantsTint, EAshlineSurface::Plastic);
		UAshlinePresentationLibrary::TintNamedStaticMesh(this, TEXT("AshlineBlock_LegR"), BootsTint, EAshlineSurface::Plastic);
	}

	UAshlinePresentationLibrary::ApplyClothingPart(this, EAshlineCosmeticSlot::Helmet, HelmetId, HelmetTint);
	UAshlinePresentationLibrary::ApplyClothingPart(this, EAshlineCosmeticSlot::Vest, VestId, VestTint);
	UAshlinePresentationLibrary::ApplyClothingPart(this, EAshlineCosmeticSlot::Pants, PantsId, PantsTint);
	UAshlinePresentationLibrary::ApplyClothingPart(this, EAshlineCosmeticSlot::Gloves, GlovesId, GlovesTint);
	UAshlinePresentationLibrary::ApplyClothingPart(this, EAshlineCosmeticSlot::Boots, BootsId, BootsTint);
	UAshlinePresentationLibrary::ApplyClothingPart(this, EAshlineCosmeticSlot::Face, FaceId, FaceTint);

	if (WeaponComponent)
	{
		WeaponComponent->ApplyCharm(CharmId);
	}
}
