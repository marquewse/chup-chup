#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChupChupPlayerController.generated.h"

/** Basic player controller; starts in game-only input mode. */
UCLASS()
class CHUPCHUP_API AChupChupPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AChupChupPlayerController();

protected:
	virtual void BeginPlay() override;
};
