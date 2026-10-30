#include "ChupChupRifle.h"

AChupChupRifle::AChupChupRifle()
{
	WeaponName = TEXT("Rifle");
	RoundsPerMinute = 600.f;
	bFullAuto = true;
	BodyDamage = 36.f;
	HeadshotMultiplier = 4.f;
	MagazineSize = 30;
	MaxReserveAmmo = 90;
	ReloadTime = 2.4f;
	EquipTime = 0.8f;

	BaseSpread = 0.06f;
	MovingSpread = 4.5f;
	AirSpread = 9.f;
	CrouchSpreadMultiplier = 0.6f;
	SprayingSpreadPerShot = 0.05f;
	MaxSprayingSpread = 0.9f;

	ViewKickFraction = 0.5f;
	RecoilRecoveryDelay = 0.12f;
	RecoilRecoveryRate = 14.f;

	// Up for ~10 shots, then right, left, right. X = pitch, Y = yaw (degrees, cumulative).
	SprayPattern = {
		FVector2D(0.0f, 0.0f), FVector2D(0.3f, 0.0f), FVector2D(0.8f, 0.05f), FVector2D(1.5f, 0.1f),
		FVector2D(2.3f, 0.0f), FVector2D(3.1f, -0.2f), FVector2D(3.8f, -0.1f), FVector2D(4.3f, 0.3f),
		FVector2D(4.6f, 0.8f), FVector2D(4.8f, 1.3f), FVector2D(4.9f, 1.6f), FVector2D(5.0f, 1.2f),
		FVector2D(5.0f, 0.6f), FVector2D(5.1f, 0.0f), FVector2D(5.1f, -0.6f), FVector2D(5.2f, -1.2f),
		FVector2D(5.2f, -1.7f), FVector2D(5.3f, -1.9f), FVector2D(5.3f, -1.5f), FVector2D(5.4f, -0.9f),
		FVector2D(5.4f, -0.2f), FVector2D(5.5f, 0.5f), FVector2D(5.5f, 1.1f), FVector2D(5.6f, 1.4f),
		FVector2D(5.6f, 1.0f), FVector2D(5.7f, 0.3f), FVector2D(5.7f, -0.4f), FVector2D(5.8f, -1.0f),
		FVector2D(5.8f, -1.3f), FVector2D(5.9f, -0.8f)
	};

	ViewOffset = FVector(35.f, 14.f, -16.f);
	BodyColor = FLinearColor(0.07f, 0.07f, 0.08f);
	AccentColor = FLinearColor(0.9f, 0.35f, 0.1f);
}
