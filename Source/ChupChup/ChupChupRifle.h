#pragma once

#include "CoreMinimal.h"
#include "ChupChupWeapon.h"
#include "ChupChupRifle.generated.h"

/** Full-auto rifle with a heavy, learnable spray (AK-style): one-tap headshots. */
UCLASS()
class CHUPCHUP_API AChupChupRifle : public AChupChupWeapon
{
	GENERATED_BODY()

public:
	AChupChupRifle();
};
