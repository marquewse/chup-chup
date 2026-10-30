#include "ChupChupHealthComponent.h"

UChupChupHealthComponent::UChupChupHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UChupChupHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	ResetHealth();
}

bool UChupChupHealthComponent::ApplyDamage(float Damage, AActor* DamageCauser)
{
	if (Damage <= 0.f || IsDead())
	{
		return false;
	}

	Health = FMath::Max(0.f, Health - Damage);
	OnDamaged.Broadcast(this, Damage, DamageCauser);

	if (IsDead())
	{
		OnDeath.Broadcast(this, DamageCauser);
		return true;
	}
	return false;
}

void UChupChupHealthComponent::ResetHealth()
{
	Health = MaxHealth;
}
