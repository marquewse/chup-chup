#include "ChupChupGameMode.h"

#include "ChupChupCharacter.h"
#include "ChupChupHUD.h"
#include "ChupChupPlayerController.h"
#include "ChupChupTrainingArena.h"

#include "Engine/World.h"
#include "EngineUtils.h"

AChupChupGameMode::AChupChupGameMode()
{
	DefaultPawnClass = AChupChupCharacter::StaticClass();
	PlayerControllerClass = AChupChupPlayerController::StaticClass();
	HUDClass = AChupChupHUD::StaticClass();
}

void AChupChupGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	if (!bSpawnTrainingArena)
	{
		return;
	}

	for (TActorIterator<AChupChupTrainingArena> It(GetWorld()); It; ++It)
	{
		TrainingArena = *It;
		break;
	}

	if (TrainingArena == nullptr)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		TrainingArena = GetWorld()->SpawnActor<AChupChupTrainingArena>(AChupChupTrainingArena::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
	}

	// Build now, before players log in, so the spawn point exists for ChoosePlayerStart.
	if (TrainingArena != nullptr)
	{
		TrainingArena->BuildArena();
	}
}

AActor* AChupChupGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (TrainingArena != nullptr)
	{
		if (AActor* SpawnPoint = TrainingArena->GetPlayerSpawnPoint())
		{
			return SpawnPoint;
		}
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}
