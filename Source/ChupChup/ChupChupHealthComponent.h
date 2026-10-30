#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChupChupHealthComponent.generated.h"

class UChupChupHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FChupChupOnDamaged, UChupChupHealthComponent*, HealthComponent, float, Damage, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChupChupOnDeath, UChupChupHealthComponent*, HealthComponent, AActor*, Killer);

/** Simple hit points for anything that can be shot. */
UCLASS(ClassGroup = (ChupChup), meta = (BlueprintSpawnableComponent))
class CHUPCHUP_API UChupChupHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UChupChupHealthComponent();

	/** Applies damage and returns true if this hit killed the owner. */
	bool ApplyDamage(float Damage, AActor* DamageCauser);

	void ResetHealth();

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	bool IsDead() const { return Health <= 0.f; }

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FChupChupOnDamaged OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FChupChupOnDeath OnDeath;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "1"))
	float MaxHealth = 100.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	float Health = 100.f;
};
