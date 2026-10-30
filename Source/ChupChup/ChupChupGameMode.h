#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChupChupGameMode.generated.h"

class AChupChupTrainingArena;

/** Default game mode: FPS character, HUD and (optionally) the procedural training arena. */
UCLASS()
class CHUPCHUP_API AChupChupGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AChupChupGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	/** Builds the training arena at the world origin if the level does not already have one. */
	UPROPERTY(EditDefaultsOnly, Category = "Training")
	bool bSpawnTrainingArena = true;

	UPROPERTY()
	TObjectPtr<AChupChupTrainingArena> TrainingArena;
};
