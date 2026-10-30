#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChupChupWeapon.generated.h"

class AChupChupCharacter;
class UPointLightComponent;
class USoundBase;
class UStaticMesh;
class UStaticMeshComponent;

/**
 * Hitscan weapon with CS-style recoil: a fixed, learnable spray pattern plus
 * spread that grows when moving, jumping or spraying, and shrinks when crouched.
 * The visible model is built from engine basic shapes and animated procedurally,
 * so it can be swapped for real art later without touching the gameplay code.
 */
UCLASS(Abstract)
class CHUPCHUP_API AChupChupWeapon : public AActor
{
	GENERATED_BODY()

public:
	AChupChupWeapon();

	virtual void Tick(float DeltaSeconds) override;

	/** Attaches the weapon in front of the character's camera. */
	void AttachToCharacter(AChupChupCharacter* NewOwner, USceneComponent* Parent);

	void OnEquipped();
	void OnUnequipped();

	void StartFire();
	void StopFire();
	void StartReload();

	/** Current cone half-angle in degrees, used for bullets and the dynamic crosshair. */
	float GetCurrentSpread() const;

	const FString& GetWeaponName() const { return WeaponName; }
	int32 GetAmmoInMagazine() const { return AmmoInMagazine; }
	int32 GetMagazineSize() const { return MagazineSize; }
	int32 GetReserveAmmo() const { return ReserveAmmo; }
	bool IsReloading() const { return bReloading; }

protected:
	virtual void BeginPlay() override;

	UStaticMeshComponent* CreatePart(FName Name, UStaticMesh* Mesh);

	// --- Stats -----------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FString WeaponName = TEXT("Weapon");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing", meta = (ClampMin = "1"))
	float RoundsPerMinute = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing")
	bool bFullAuto = true;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing")
	float BodyDamage = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing")
	float HeadshotMultiplier = 4.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing")
	float Range = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Firing")
	float ImpactImpulse = 30000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ammo", meta = (ClampMin = "1"))
	int32 MagazineSize = 30;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ammo")
	int32 MaxReserveAmmo = 90;

	/** Training mode: reloading never consumes reserve ammo. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ammo")
	bool bInfiniteReserveAmmo = true;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ammo")
	float ReloadTime = 2.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Ammo")
	float EquipTime = 0.6f;

	// --- Accuracy (degrees, cone half-angle) ------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Accuracy")
	float BaseSpread = 0.05f;

	/** Added at full run speed. Below 34% of run speed the weapon is fully accurate. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Accuracy")
	float MovingSpread = 4.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Accuracy")
	float AirSpread = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Accuracy")
	float CrouchSpreadMultiplier = 0.6f;

	/** Extra spread per shot in the current spray. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Accuracy")
	float SprayingSpreadPerShot = 0.06f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Accuracy")
	float MaxSprayingSpread = 1.f;

	// --- Recoil -----------------------------------------------------------

	/** Cumulative offset of each shot in a spray, in degrees. X = pitch (up), Y = yaw (right). */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil")
	TArray<FVector2D> SprayPattern;

	/** Fraction of the pattern applied to the camera; the rest only moves the bullets. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil", meta = (ClampMin = "0", ClampMax = "1"))
	float ViewKickFraction = 0.5f;

	/** How long after the last shot the spray starts to reset. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil")
	float RecoilRecoveryDelay = 0.12f;

	/** Spray shots recovered per second once recovery starts. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Recoil")
	float RecoilRecoveryRate = 14.f;

	// --- Presentation -----------------------------------------------------

	/** Offset from the camera (forward, right, up). */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Presentation")
	FVector ViewOffset = FVector(35.f, 14.f, -16.f);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Presentation")
	FLinearColor BodyColor = FLinearColor(0.08f, 0.08f, 0.09f);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Presentation")
	FLinearColor AccentColor = FLinearColor(0.9f, 0.35f, 0.1f);

	/** Optional; plays at the muzzle when set. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Presentation")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> WeaponRoot;

	/** Animated procedurally (kick, reload, equip, bob); the parts hang off it. */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> WeaponPivot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BodyPart;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BarrelPart;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MagazinePart;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StockPart;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Muzzle;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPointLightComponent> MuzzleFlash;

private:
	bool CanFire(float Now) const;
	void FireShot(float Now);
	void FinishReload();
	FVector2D GetPatternOffset(float ShotIndex) const;
	void UpdateRecoil(float DeltaSeconds, float Now);
	void UpdateProceduralAnimation(float DeltaSeconds, float Now);
	void ResetRecoil();

	UPROPERTY()
	TObjectPtr<AChupChupCharacter> OwnerCharacter;

	int32 AmmoInMagazine = 0;
	int32 ReserveAmmo = 0;

	bool bEquipped = false;
	bool bTriggerHeld = false;
	bool bReloading = false;

	float EquipStartTime = 0.f;
	float ReloadStartTime = 0.f;
	float NextFireTime = 0.f;
	float LastShotTime = -100.f;
	float MuzzleFlashEndTime = 0.f;

	/** Position in the spray pattern; fractional while recovering. */
	float RecoilIndex = 0.f;

	/** Part of the pattern currently applied to the control rotation. */
	FVector2D AppliedViewKick = FVector2D::ZeroVector;

	float KickAlpha = 0.f;
	float BobTime = 0.f;

	FTimerHandle ReloadTimerHandle;
};
