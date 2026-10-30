#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ChupChupCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

/** Basic third-person character: walk, look around and jump. */
UCLASS()
class CHUPCHUP_API AChupChupCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AChupChupCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	/** Moves along the camera-facing forward axis (W/S, left stick Y). */
	void MoveForward(float Value);

	/** Moves along the camera-facing right axis (A/D, left stick X). */
	void MoveRight(float Value);

	/** Camera rotation rate for gamepad input, in degrees/second at full deflection. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookRate = 90.f;

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
};
