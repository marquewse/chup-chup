#include "ChupChupGameMode.h"

#include "ChupChupCharacter.h"
#include "ChupChupPlayerController.h"

AChupChupGameMode::AChupChupGameMode()
{
	DefaultPawnClass = AChupChupCharacter::StaticClass();
	PlayerControllerClass = AChupChupPlayerController::StaticClass();
}
