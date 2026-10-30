#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChupChupTargetBot.generated.h"

class UChupChupHealthComponent;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;

/** Training dummy with a separate head hitbox. Can strafe side to side and respawns after dying. */
UCLASS()
class CHUPCHUP_API AChupChupTargetBot : public AActor
{
	GENERATED_BODY()

public:
	AChupChupTargetBot();

	virtual void Tick(float DeltaSeconds) override;

	void SetStrafe(bool bEnable, float Amplitude = 250.f, float Speed = 1.6f);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDamaged(UChupChupHealthComponent* DamagedHealth, float Damage, AActor* DamageCauser);

	UFUNCTION()
	void HandleDeath(UChupChupHealthComponent* DeadHealth, AActor* Killer);

	void Respawn();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> BotRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Body;

	/** Tagged "Head"; weapons apply their headshot multiplier when they hit it. */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Head;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UChupChupHealthComponent> Health;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float RespawnDelay = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Bot")
	bool bStrafe = false;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float StrafeAmplitude = 250.f;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float StrafeSpeed = 1.6f;

	UPROPERTY(EditAnywhere, Category = "Bot")
	FLinearColor BodyColor = FLinearColor(0.85f, 0.2f, 0.12f);

	UPROPERTY(EditAnywhere, Category = "Bot")
	FLinearColor HeadColor = FLinearColor(1.f, 0.8f, 0.15f);

private:
	void SetFlash(bool bFlashOn);

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BodyMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> HeadMaterial;

	FVector HomeLocation = FVector::ZeroVector;
	float StrafeTime = 0.f;
	float FlashEndTime = 0.f;
	bool bFlashing = false;
	FTimerHandle RespawnTimerHandle;
};
