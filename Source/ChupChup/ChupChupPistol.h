#pragma once

#include "CoreMinimal.h"
#include "ChupChupWeapon.h"
#include "ChupChupPistol.generated.h"

/** Semi-auto sidearm: accurate first shot, punishes spamming. */
UCLASS()
class CHUPCHUP_API AChupChupPistol : public AChupChupWeapon
{
	GENERATED_BODY()

public:
	AChupChupPistol();
};
