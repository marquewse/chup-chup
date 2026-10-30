#include "ChupChupPlayerController.h"

AChupChupPlayerController::AChupChupPlayerController()
{
	bShowMouseCursor = false;
}

void AChupChupPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
}
