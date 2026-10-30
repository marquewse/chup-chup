#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChupChupGameMode.generated.h"

/** Default game mode: spawns AChupChupCharacter controlled by AChupChupPlayerController. */
UCLASS()
class CHUPCHUP_API AChupChupGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AChupChupGameMode();
};
