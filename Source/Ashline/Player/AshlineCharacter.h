#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AshlineTypes.h"
#include "AshlineCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UAshlineWeaponComponent;
class UStaticMeshComponent;
class USkeletalMesh;
struct FInputActionValue;

UCLASS()
class ASHLINE_API AAshlineCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAshlineCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Ashline|Camera")
	void SetCameraMode(EAshlineCameraMode NewMode);

	UFUNCTION(BlueprintPure, Category = "Ashline|Camera")
	EAshlineCameraMode GetCameraMode() const { return CameraMode; }

	UFUNCTION(BlueprintCallable, Category = "Ashline|Camera")
	void ToggleCameraMode();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Combat")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Combat")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Combat")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Ashline|Combat")
	void SetAiming(bool bNewAiming);

	UFUNCTION(BlueprintCallable, Category = "Ashline|Combat")
	void SwapWeapon();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Camera")
	TObjectPtr<USpringArmComponent> ThirdPersonArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Camera")
	TObjectPtr<UCameraComponent> ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Combat")
	TObjectPtr<UAshlineWeaponComponent> WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Mesh")
	TObjectPtr<UStaticMeshComponent> GrayboxBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Mesh")
	TObjectPtr<UStaticMeshComponent> GrayboxWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputAction> CameraToggleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputAction> SwapWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ashline|Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Movement")
	float AimWalkMul = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Combat")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Combat")
	float Health = 100.f;

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartAim();
	void StopAim();
	void StartCrouch();
	void StopCrouch();
	void ApplyRuntimeInputActions();
	void BindLegacyKeys(UInputComponent* PlayerInputComponent);
	void LegacyMoveForward(float Value);
	void LegacyMoveRight(float Value);
	void LegacyMoveBack(float Value);
	void LegacyMoveLeft(float Value);
	void LegacyLookYaw(float Value);
	void LegacyLookPitch(float Value);
	void EnsureDefaultLoadout();
	void ApplyGrayboxMeshes();
	void ApplyPresentationMesh();
	void ApplyOperatorLook();
	void TickFootsteps(float DeltaSeconds);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|Mesh")
	TSoftObjectPtr<USkeletalMesh> HeroMeshOverride;

	float FootstepAccumulator = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Camera")
	EAshlineCameraMode CameraMode = EAshlineCameraMode::FirstPerson;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ashline|Combat")
	bool bIsAiming = false;
};
