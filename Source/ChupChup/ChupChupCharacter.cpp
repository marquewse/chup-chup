#include "ChupChupCharacter.h"

#include "ChupChupHealthComponent.h"
#include "ChupChupPistol.h"
#include "ChupChupRifle.h"
#include "ChupChupWeapon.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

AChupChupCharacter::AChupChupCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);

	// First person: the whole pawn yaws with the mouse, the camera also pitches.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, EyeHeight));
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetFieldOfView(103.f);

	HealthComponent = CreateDefaultSubobject<UChupChupHealthComponent>(TEXT("Health"));

	// Snappy, competitive movement: high acceleration and braking so counter-strafing works.
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->MaxWalkSpeed = RunSpeed;
	Movement->MaxWalkSpeedCrouched = 180.f;
	Movement->MaxAcceleration = 4000.f;
	Movement->BrakingDecelerationWalking = 4000.f;
	Movement->GroundFriction = 8.f;
	Movement->BrakingFrictionFactor = 1.f;
	Movement->JumpZVelocity = 450.f;
	Movement->AirControl = 0.3f;
	Movement->GravityScale = 1.2f;
	Movement->GetNavAgentPropertiesRef().bCanCrouch = true;
	Movement->bCanWalkOffLedgesWhenCrouching = true;

	DefaultWeapons.Add(AChupChupRifle::StaticClass());
	DefaultWeapons.Add(AChupChupPistol::StaticClass());
}

void AChupChupCharacter::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (const TSubclassOf<AChupChupWeapon>& WeaponClass : DefaultWeapons)
	{
		if (!WeaponClass)
		{
			continue;
		}

		AChupChupWeapon* Weapon = GetWorld()->SpawnActor<AChupChupWeapon>(WeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		if (Weapon != nullptr)
		{
			Weapon->AttachToCharacter(this, FirstPersonCamera);
			Weapon->OnUnequipped();
			Weapons.Add(Weapon);
		}
	}

	EquipWeapon(0);
}

void AChupChupCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Ease the camera to eye height after the capsule changes size.
	FVector CameraLocation = FirstPersonCamera->GetRelativeLocation();
	const float CameraZ = static_cast<float>(CameraLocation.Z);
	if (!FMath::IsNearlyEqual(CameraZ, EyeHeight))
	{
		CameraLocation.Z = FMath::FInterpTo(CameraZ, EyeHeight, DeltaSeconds, CrouchCameraInterpSpeed);
		FirstPersonCamera->SetRelativeLocation(CameraLocation);
	}
}

void AChupChupCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// The capsule just moved down; keep the camera where it was and let Tick ease it down.
	FirstPersonCamera->AddRelativeLocation(FVector(0.f, 0.f, ScaledHalfHeightAdjust));
}

void AChupChupCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	FirstPersonCamera->AddRelativeLocation(FVector(0.f, 0.f, -ScaledHalfHeightAdjust));
}

void AChupChupCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Axis/action names are defined in Config/DefaultInput.ini.
	PlayerInputComponent->BindAxis("MoveForward", this, &AChupChupCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AChupChupCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AChupChupCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AChupChupCharacter::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AChupChupCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AChupChupCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AChupChupCharacter::OnFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AChupChupCharacter::OnFireReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AChupChupCharacter::OnReloadPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AChupChupCharacter::OnCrouchPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AChupChupCharacter::OnCrouchReleased);
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AChupChupCharacter::OnWalkPressed);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AChupChupCharacter::OnWalkReleased);
	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &AChupChupCharacter::OnEquipSlot1);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &AChupChupCharacter::OnEquipSlot2);
	PlayerInputComponent->BindAction("LastWeapon", IE_Pressed, this, &AChupChupCharacter::OnEquipLastWeapon);
}

void AChupChupCharacter::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AChupChupCharacter::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AChupChupCharacter::Turn(float Value)
{
	AddControllerYawInput(Value * MouseSensitivity);
}

void AChupChupCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value * MouseSensitivity);
}

void AChupChupCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseLookRate * GetWorld()->GetDeltaSeconds());
}

void AChupChupCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookRate * GetWorld()->GetDeltaSeconds());
}

void AChupChupCharacter::OnFirePressed()
{
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->StartFire();
	}
}

void AChupChupCharacter::OnFireReleased()
{
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->StopFire();
	}
}

void AChupChupCharacter::OnReloadPressed()
{
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->StartReload();
	}
}

void AChupChupCharacter::OnCrouchPressed()
{
	Crouch();
}

void AChupChupCharacter::OnCrouchReleased()
{
	UnCrouch();
}

void AChupChupCharacter::OnWalkPressed()
{
	bWalkHeld = true;
	UpdateMaxWalkSpeed();
}

void AChupChupCharacter::OnWalkReleased()
{
	bWalkHeld = false;
	UpdateMaxWalkSpeed();
}

void AChupChupCharacter::UpdateMaxWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = bWalkHeld ? WalkSpeed : RunSpeed;
}

void AChupChupCharacter::OnEquipSlot1()
{
	EquipWeapon(0);
}

void AChupChupCharacter::OnEquipSlot2()
{
	EquipWeapon(1);
}

void AChupChupCharacter::OnEquipLastWeapon()
{
	if (LastWeaponIndex != INDEX_NONE)
	{
		EquipWeapon(LastWeaponIndex);
	}
}

void AChupChupCharacter::EquipWeapon(int32 Index)
{
	if (!Weapons.IsValidIndex(Index) || Index == CurrentWeaponIndex)
	{
		return;
	}

	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->OnUnequipped();
	}

	LastWeaponIndex = CurrentWeaponIndex;
	CurrentWeaponIndex = Index;
	CurrentWeapon = Weapons[Index];
	CurrentWeapon->OnEquipped();
}

void AChupChupCharacter::NotifyShotResult(bool bHit, bool bHeadshot, bool bKill, float Damage)
{
	++ShotsFired;

	if (!bHit)
	{
		return;
	}

	++ShotsHit;
	Headshots += bHeadshot ? 1 : 0;
	Kills += bKill ? 1 : 0;

	LastHitTime = GetWorld()->GetTimeSeconds();
	LastHitDamage = Damage;
	bLastHitHeadshot = bHeadshot;
	bLastHitKill = bKill;
}
