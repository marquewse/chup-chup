#include "ChupChupPistol.h"

#include "Components/StaticMeshComponent.h"

AChupChupPistol::AChupChupPistol()
{
	WeaponName = TEXT("Pistol");
	RoundsPerMinute = 400.f;
	bFullAuto = false;
	BodyDamage = 30.f;
	HeadshotMultiplier = 3.5f;
	MagazineSize = 12;
	MaxReserveAmmo = 36;
	ReloadTime = 1.8f;
	EquipTime = 0.4f;

	BaseSpread = 0.08f;
	MovingSpread = 2.5f;
	AirSpread = 6.f;
	CrouchSpreadMultiplier = 0.7f;
	SprayingSpreadPerShot = 0.25f;
	MaxSprayingSpread = 1.5f;

	ViewKickFraction = 0.6f;
	RecoilRecoveryDelay = 0.08f;
	RecoilRecoveryRate = 6.f;

	SprayPattern = {
		FVector2D(0.0f, 0.0f), FVector2D(0.7f, 0.05f), FVector2D(1.4f, -0.1f), FVector2D(2.0f, 0.15f),
		FVector2D(2.5f, -0.05f), FVector2D(2.9f, 0.2f), FVector2D(3.2f, -0.2f), FVector2D(3.4f, 0.1f)
	};

	ViewOffset = FVector(30.f, 12.f, -14.f);
	BodyColor = FLinearColor(0.12f, 0.12f, 0.13f);
	AccentColor = FLinearColor(0.1f, 0.6f, 0.65f);

	// Reshape the default rifle parts into a handgun: short slide, no stock.
	BodyPart->SetRelativeLocation(FVector(4.f, 0.f, 0.f));
	BodyPart->SetRelativeScale3D(FVector(0.2f, 0.04f, 0.06f));

	BarrelPart->SetRelativeLocation(FVector(16.f, 0.f, 0.5f));
	BarrelPart->SetRelativeScale3D(FVector(0.02f, 0.02f, 0.06f));

	MagazinePart->SetRelativeLocation(FVector(-2.f, 0.f, -8.f));
	MagazinePart->SetRelativeRotation(FRotator(-12.f, 0.f, 0.f));
	MagazinePart->SetRelativeScale3D(FVector(0.045f, 0.035f, 0.12f));

	StockPart->SetVisibility(false);

	Muzzle->SetRelativeLocation(FVector(19.f, 0.f, 0.5f));
}
