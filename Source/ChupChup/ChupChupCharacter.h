#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ChupChupCharacter.generated.h"

class AChupChupWeapon;
class UCameraComponent;
class UChupChupHealthComponent;

/** First-person shooter character with CS/Valorant-style movement and a small weapon inventory. */
UCLASS()
class CHUPCHUP_API AChupChupCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AChupChupCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	UChupChupHealthComponent* GetHealthComponent() const { return HealthComponent; }
	AChupChupWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
	float GetRunSpeed() const { return RunSpeed; }

	/** Called by weapons after every shot, for HUD feedback and stats. */
	void NotifyShotResult(bool bHit, bool bHeadshot, bool bKill, float Damage);

	// Stats and last-hit feedback, read by the HUD.
	int32 ShotsFired = 0;
	int32 ShotsHit = 0;
	int32 Headshots = 0;
	int32 Kills = 0;
	float LastHitTime = -100.f;
	float LastHitDamage = 0.f;
	bool bLastHitHeadshot = false;
	bool bLastHitKill = false;

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void OnFirePressed();
	void OnFireReleased();
	void OnReloadPressed();
	void OnCrouchPressed();
	void OnCrouchReleased();
	void OnWalkPressed();
	void OnWalkReleased();
	void OnEquipSlot1();
	void OnEquipSlot2();
	void OnEquipLastWeapon();

	void EquipWeapon(int32 Index);
	void UpdateMaxWalkSpeed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 540.f;

	/** Shift-walk: quieter and slower. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 280.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MouseSensitivity = 1.f;

	/** Gamepad look rate in degrees/second at full deflection. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookRate = 120.f;

	/** Camera height above the capsule center. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float EyeHeight = 64.f;

	/** How fast the camera settles after crouching or standing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CrouchCameraInterpSpeed = 14.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<AChupChupWeapon>> DefaultWeapons;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UChupChupHealthComponent> HealthComponent;

	UPROPERTY()
	TArray<TObjectPtr<AChupChupWeapon>> Weapons;

	UPROPERTY()
	TObjectPtr<AChupChupWeapon> CurrentWeapon;

	int32 CurrentWeaponIndex = INDEX_NONE;
	int32 LastWeaponIndex = INDEX_NONE;
	bool bWalkHeld = false;
};
