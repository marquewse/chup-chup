#include "ChupChupWeapon.h"

#include "ChupChupCharacter.h"
#include "ChupChupHealthComponent.h"
#include "ChupChupVisuals.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AChupChupWeapon::AChupChupWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	SetRootComponent(WeaponRoot);

	WeaponPivot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPivot"));
	WeaponPivot->SetupAttachment(WeaponRoot);

	// Default layout is a rifle; subclasses reshape the parts in their constructors.
	// Basic shapes are 100 units across, so a scale of 0.1 means 10 cm.
	BodyPart = CreatePart(TEXT("Body"), CubeMesh.Object);
	BodyPart->SetRelativeScale3D(FVector(0.45f, 0.07f, 0.12f));

	BarrelPart = CreatePart(TEXT("Barrel"), CylinderMesh.Object);
	BarrelPart->SetRelativeLocation(FVector(38.f, 0.f, 2.f));
	BarrelPart->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	BarrelPart->SetRelativeScale3D(FVector(0.03f, 0.03f, 0.35f));

	MagazinePart = CreatePart(TEXT("Magazine"), CubeMesh.Object);
	MagazinePart->SetRelativeLocation(FVector(8.f, 0.f, -13.f));
	MagazinePart->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	MagazinePart->SetRelativeScale3D(FVector(0.07f, 0.05f, 0.18f));

	StockPart = CreatePart(TEXT("Stock"), CubeMesh.Object);
	StockPart->SetRelativeLocation(FVector(-30.f, 0.f, -3.f));
	StockPart->SetRelativeScale3D(FVector(0.2f, 0.06f, 0.1f));

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(WeaponPivot);
	Muzzle->SetRelativeLocation(FVector(56.f, 0.f, 2.f));

	MuzzleFlash = CreateDefaultSubobject<UPointLightComponent>(TEXT("MuzzleFlash"));
	MuzzleFlash->SetupAttachment(Muzzle);
	MuzzleFlash->SetIntensity(8000.f);
	MuzzleFlash->SetAttenuationRadius(400.f);
	MuzzleFlash->SetLightColor(FLinearColor(1.f, 0.65f, 0.3f));
	MuzzleFlash->SetCastShadows(false);
	MuzzleFlash->SetVisibility(false);
}

UStaticMeshComponent* AChupChupWeapon::CreatePart(FName Name, UStaticMesh* Mesh)
{
	UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
	Part->SetupAttachment(WeaponPivot);
	Part->SetStaticMesh(Mesh);
	Part->SetMobility(EComponentMobility::Movable);
	Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Part->SetCastShadow(false);
	return Part;
}

void AChupChupWeapon::BeginPlay()
{
	Super::BeginPlay();

	AmmoInMagazine = MagazineSize;
	ReserveAmmo = MaxReserveAmmo;

	ChupChupVisuals::ApplyColor(BodyPart, BodyColor);
	ChupChupVisuals::ApplyColor(StockPart, BodyColor);
	ChupChupVisuals::ApplyColor(BarrelPart, AccentColor);
	ChupChupVisuals::ApplyColor(MagazinePart, AccentColor);
}

void AChupChupWeapon::AttachToCharacter(AChupChupCharacter* NewOwner, USceneComponent* Parent)
{
	OwnerCharacter = NewOwner;
	SetOwner(NewOwner);

	if (Parent != nullptr)
	{
		AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SetActorRelativeLocation(ViewOffset);
	}
}

void AChupChupWeapon::OnEquipped()
{
	bEquipped = true;
	EquipStartTime = GetWorld()->GetTimeSeconds();
	ResetRecoil();
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
}

void AChupChupWeapon::OnUnequipped()
{
	bEquipped = false;
	bTriggerHeld = false;
	bReloading = false;
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	ResetRecoil();
	MuzzleFlash->SetVisibility(false);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void AChupChupWeapon::StartFire()
{
	bTriggerHeld = true;
}

void AChupChupWeapon::StopFire()
{
	bTriggerHeld = false;
}

void AChupChupWeapon::StartReload()
{
	const bool bHasReserve = bInfiniteReserveAmmo || ReserveAmmo > 0;
	if (!bEquipped || bReloading || AmmoInMagazine >= MagazineSize || !bHasReserve)
	{
		return;
	}

	bReloading = true;
	bTriggerHeld = false;
	ReloadStartTime = GetWorld()->GetTimeSeconds();
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AChupChupWeapon::FinishReload, ReloadTime, false);
}

void AChupChupWeapon::FinishReload()
{
	const int32 Needed = MagazineSize - AmmoInMagazine;
	const int32 Loaded = bInfiniteReserveAmmo ? Needed : FMath::Min(Needed, ReserveAmmo);

	AmmoInMagazine += Loaded;
	if (!bInfiniteReserveAmmo)
	{
		ReserveAmmo -= Loaded;
	}
	bReloading = false;
}

float AChupChupWeapon::GetCurrentSpread() const
{
	float Spread = BaseSpread;

	if (OwnerCharacter != nullptr)
	{
		const UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
		const float RunSpeed = FMath::Max(OwnerCharacter->GetRunSpeed(), 1.f);
		const float AccurateSpeed = RunSpeed * 0.34f;
		const float Speed = OwnerCharacter->GetVelocity().Size2D();

		Spread += MovingSpread * FMath::Clamp((Speed - AccurateSpeed) / (RunSpeed - AccurateSpeed), 0.f, 1.f);

		if (Movement != nullptr)
		{
			if (Movement->IsFalling())
			{
				Spread += AirSpread;
			}
			else if (Movement->IsCrouching())
			{
				Spread *= CrouchSpreadMultiplier;
			}
		}
	}

	Spread += FMath::Min(RecoilIndex * SprayingSpreadPerShot, MaxSprayingSpread);
	return Spread;
}

bool AChupChupWeapon::CanFire(float Now) const
{
	return bEquipped && !bReloading && Now >= EquipStartTime + EquipTime && Now >= NextFireTime;
}

void AChupChupWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float Now = GetWorld()->GetTimeSeconds();

	if (bTriggerHeld && CanFire(Now))
	{
		FireShot(Now);
	}

	if (MuzzleFlash->IsVisible() && Now >= MuzzleFlashEndTime)
	{
		MuzzleFlash->SetVisibility(false);
	}

	UpdateRecoil(DeltaSeconds, Now);
	UpdateProceduralAnimation(DeltaSeconds, Now);
}

void AChupChupWeapon::FireShot(float Now)
{
	if (OwnerCharacter == nullptr || OwnerCharacter->GetController() == nullptr)
	{
		return;
	}

	if (AmmoInMagazine <= 0)
	{
		bTriggerHeld = false;
		StartReload();
		return;
	}

	// Keep a steady cadence while the trigger is held, but never "bank" shots.
	const float FireInterval = 60.f / RoundsPerMinute;
	NextFireTime = (Now - NextFireTime < FireInterval) ? NextFireTime + FireInterval : Now + FireInterval;

	if (!bFullAuto)
	{
		bTriggerHeld = false;
	}

	--AmmoInMagazine;

	FVector ViewLocation;
	FRotator ViewRotation;
	OwnerCharacter->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

	// The camera already carries ViewKickFraction of the pattern; the bullets get the rest.
	const FVector2D Pattern = GetPatternOffset(RecoilIndex) * (1.f - ViewKickFraction);
	const FRotator AimRotation = ViewRotation + FRotator(Pattern.X, Pattern.Y, 0.f);
	const FVector Direction = FMath::VRandCone(AimRotation.Vector(), FMath::DegreesToRadians(GetCurrentSpread()));

	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = TraceStart + Direction * Range;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ChupChupWeaponTrace), true, this);
	Params.AddIgnoredActor(OwnerCharacter);

	FHitResult Hit;
	const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
	const FVector ImpactPoint = bBlocked ? Hit.ImpactPoint : TraceEnd;

	bool bHitTarget = false;
	bool bHeadshot = false;
	bool bKill = false;
	float DamageDealt = 0.f;

	if (bBlocked)
	{
		AActor* HitActor = Hit.GetActor();
		UPrimitiveComponent* HitComponent = Hit.GetComponent();
		UChupChupHealthComponent* Health = HitActor != nullptr ? HitActor->FindComponentByClass<UChupChupHealthComponent>() : nullptr;

		if (Health != nullptr && !Health->IsDead())
		{
			bHeadshot = HitComponent != nullptr && HitComponent->ComponentHasTag(TEXT("Head"));
			DamageDealt = BodyDamage * (bHeadshot ? HeadshotMultiplier : 1.f);
			bKill = Health->ApplyDamage(DamageDealt, OwnerCharacter);
			bHitTarget = true;
		}
		else if (HitComponent != nullptr && HitComponent->IsSimulatingPhysics())
		{
			HitComponent->AddImpulseAtLocation(Direction * ImpactImpulse, ImpactPoint);
		}

		DrawDebugPoint(GetWorld(), ImpactPoint, 6.f, bHitTarget ? FColor::Red : FColor(40, 40, 40), false, 3.f);
	}

	DrawDebugLine(GetWorld(), Muzzle->GetComponentLocation(), ImpactPoint, FColor(255, 210, 120), false, 0.04f, 0, 0.6f);

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Muzzle->GetComponentLocation());
	}

	MuzzleFlash->SetVisibility(true);
	MuzzleFlashEndTime = Now + 0.05f;
	KickAlpha = FMath::Min(KickAlpha + 1.f, 1.5f);

	RecoilIndex += 1.f;
	LastShotTime = Now;

	OwnerCharacter->NotifyShotResult(bHitTarget, bHeadshot, bKill, DamageDealt);
}

FVector2D AChupChupWeapon::GetPatternOffset(float ShotIndex) const
{
	if (SprayPattern.Num() == 0)
	{
		return FVector2D::ZeroVector;
	}

	const int32 LastIndex = SprayPattern.Num() - 1;
	const int32 Index0 = FMath::Clamp(FMath::FloorToInt(ShotIndex), 0, LastIndex);
	const int32 Index1 = FMath::Min(Index0 + 1, LastIndex);
	const float Alpha = FMath::Clamp(ShotIndex - static_cast<float>(Index0), 0.f, 1.f);
	return FMath::Lerp(SprayPattern[Index0], SprayPattern[Index1], Alpha);
}

void AChupChupWeapon::UpdateRecoil(float DeltaSeconds, float Now)
{
	if (Now - LastShotTime > RecoilRecoveryDelay)
	{
		RecoilIndex = FMath::Max(0.f, RecoilIndex - RecoilRecoveryRate * DeltaSeconds);
	}

	// Move the camera by the change in kick, so recovery brings the view back down.
	const FVector2D DesiredKick = GetPatternOffset(RecoilIndex) * ViewKickFraction;
	const FVector2D Delta = DesiredKick - AppliedViewKick;
	AppliedViewKick = DesiredKick;

	AController* Controller = OwnerCharacter != nullptr ? OwnerCharacter->GetController() : nullptr;
	if (Controller != nullptr && !Delta.IsNearlyZero(KINDA_SMALL_NUMBER))
	{
		FRotator ControlRotation = Controller->GetControlRotation();
		ControlRotation.Pitch += Delta.X;
		ControlRotation.Yaw += Delta.Y;
		Controller->SetControlRotation(ControlRotation);
	}
}

void AChupChupWeapon::ResetRecoil()
{
	RecoilIndex = 0.f;
	AppliedViewKick = FVector2D::ZeroVector;
	KickAlpha = 0.f;
}

void AChupChupWeapon::UpdateProceduralAnimation(float DeltaSeconds, float Now)
{
	KickAlpha = FMath::FInterpTo(KickAlpha, 0.f, DeltaSeconds, 18.f);

	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;

	// Fire kick: slide back and tip up.
	Location.X -= 3.f * KickAlpha;
	Rotation.Pitch += 2.5f * KickAlpha;

	// Reload: dip and roll the weapon, then bring it back.
	if (bReloading && ReloadTime > 0.f)
	{
		const float Alpha = FMath::Sin(PI * FMath::Clamp((Now - ReloadStartTime) / ReloadTime, 0.f, 1.f));
		Location.Z -= 8.f * Alpha;
		Rotation.Pitch -= 30.f * Alpha;
		Rotation.Roll += 25.f * Alpha;
	}

	// Equip: raise from below the screen.
	if (EquipTime > 0.f)
	{
		const float T = FMath::Clamp((Now - EquipStartTime) / EquipTime, 0.f, 1.f);
		const float Alpha = FMath::Square(1.f - T);
		Location.Z -= 25.f * Alpha;
		Rotation.Pitch -= 35.f * Alpha;
	}

	// Walk bob, scaled by ground speed.
	if (OwnerCharacter != nullptr)
	{
		const UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
		const bool bGrounded = Movement != nullptr && Movement->IsMovingOnGround();
		const float SpeedAlpha = bGrounded ? FMath::Clamp(OwnerCharacter->GetVelocity().Size2D() / FMath::Max(OwnerCharacter->GetRunSpeed(), 1.f), 0.f, 1.f) : 0.f;

		BobTime += DeltaSeconds * (4.f + 6.f * SpeedAlpha);
		Location.Z += FMath::Sin(BobTime * 2.f) * 0.6f * SpeedAlpha;
		Location.Y += FMath::Sin(BobTime) * 0.9f * SpeedAlpha;
	}

	WeaponPivot->SetRelativeLocationAndRotation(Location, Rotation);
}
