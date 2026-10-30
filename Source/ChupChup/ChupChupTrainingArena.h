#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChupChupTrainingArena.generated.h"

class AChupChupTargetBot;
class ATargetPoint;
class UStaticMesh;
class UStaticMeshComponent;

/**
 * Procedural aim-training map built from engine cubes: spawn area, a mid wall with
 * two doors, crates for cover, a ramp to a sniper platform and bots at several ranges.
 * Placeholder art: swap it for a real level whenever you have one.
 */
UCLASS()
class CHUPCHUP_API AChupChupTrainingArena : public AActor
{
	GENERATED_BODY()

public:
	AChupChupTrainingArena();

	/** Creates the geometry, the player spawn point and the bots. Safe to call more than once. */
	void BuildArena();

	AActor* GetPlayerSpawnPoint() const;

protected:
	virtual void BeginPlay() override;

	UStaticMeshComponent* AddBlock(const FVector& Center, const FVector& Size, const FLinearColor& Color, const FRotator& Rotation = FRotator::ZeroRotator);

	/** Box resting on the floor; X/Y is the center of its footprint. */
	UStaticMeshComponent* AddBox(float X, float Y, const FVector& Size, const FLinearColor& Color, float BaseZ = 0.f);

	void SpawnBot(const FVector& FeetLocation, bool bStrafe);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> ArenaRoot;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<ATargetPoint> PlayerSpawnPoint;

	UPROPERTY()
	TArray<TObjectPtr<AChupChupTargetBot>> Bots;

private:
	bool bBuilt = false;
};
