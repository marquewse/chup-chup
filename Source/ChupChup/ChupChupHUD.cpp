#include "ChupChupHUD.h"

#include "ChupChupCharacter.h"
#include "ChupChupHealthComponent.h"
#include "ChupChupWeapon.h"

#include "Camera/PlayerCameraManager.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void AChupChupHUD::DrawHUD()
{
	Super::DrawHUD();

	const AChupChupCharacter* Character = Cast<AChupChupCharacter>(GetOwningPawn());
	if (Canvas == nullptr || Character == nullptr)
	{
		return;
	}

	const float CenterX = Canvas->ClipX * 0.5f;
	const float CenterY = Canvas->ClipY * 0.5f;

	DrawCrosshair(*Character, CenterX, CenterY);
	DrawHitMarker(*Character, CenterX, CenterY, GetWorld()->GetTimeSeconds());
	DrawInfo(*Character);
}

void AChupChupHUD::DrawOutlinedLine(float X1, float Y1, float X2, float Y2, const FLinearColor& Color, float Thickness)
{
	DrawLine(X1, Y1, X2, Y2, FLinearColor(0.f, 0.f, 0.f, 0.8f), Thickness + 2.f);
	DrawLine(X1, Y1, X2, Y2, Color, Thickness);
}

void AChupChupHUD::DrawRightAlignedText(const FString& Text, const FLinearColor& Color, float RightX, float Y, UFont* Font, float Scale)
{
	float Width = 0.f;
	float Height = 0.f;
	GetTextSize(Text, Width, Height, Font, Scale);
	DrawText(Text, Color, RightX - Width, Y, Font, Scale);
}

void AChupChupHUD::DrawCrosshair(const AChupChupCharacter& Character, float CenterX, float CenterY)
{
	// The gap matches the real bullet cone on screen, so the crosshair "breathes" when moving or spraying.
	float Gap = 4.f;
	if (const AChupChupWeapon* Weapon = Character.GetCurrentWeapon())
	{
		const float FOV = (PlayerOwner != nullptr && PlayerOwner->PlayerCameraManager != nullptr) ? PlayerOwner->PlayerCameraManager->GetFOVAngle() : 90.f;
		const float SpreadRadians = FMath::DegreesToRadians(Weapon->GetCurrentSpread());
		const float HalfFOVRadians = FMath::DegreesToRadians(FOV * 0.5f);
		Gap = FMath::Clamp(FMath::Tan(SpreadRadians) / FMath::Tan(HalfFOVRadians) * CenterX, 4.f, 140.f);
	}

	const float Length = CrosshairLength;
	DrawOutlinedLine(CenterX - Gap - Length, CenterY, CenterX - Gap, CenterY, CrosshairColor, CrosshairThickness);
	DrawOutlinedLine(CenterX + Gap, CenterY, CenterX + Gap + Length, CenterY, CrosshairColor, CrosshairThickness);
	DrawOutlinedLine(CenterX, CenterY - Gap - Length, CenterX, CenterY - Gap, CrosshairColor, CrosshairThickness);
	DrawOutlinedLine(CenterX, CenterY + Gap, CenterX, CenterY + Gap + Length, CrosshairColor, CrosshairThickness);

	if (bCenterDot)
	{
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.8f), CenterX - 2.f, CenterY - 2.f, 4.f, 4.f);
		DrawRect(CrosshairColor, CenterX - 1.f, CenterY - 1.f, 2.f, 2.f);
	}
}

void AChupChupHUD::DrawHitMarker(const AChupChupCharacter& Character, float CenterX, float CenterY, float Now)
{
	constexpr float Duration = 0.25f;
	const float Age = Now - Character.LastHitTime;
	if (Age < 0.f || Age > Duration)
	{
		return;
	}

	const float Alpha = 1.f - Age / Duration;
	FLinearColor Color = Character.bLastHitKill ? FLinearColor(1.f, 0.15f, 0.15f) : (Character.bLastHitHeadshot ? FLinearColor(1.f, 0.85f, 0.1f) : FLinearColor::White);
	Color.A = Alpha;

	constexpr float Inner = 10.f;
	constexpr float Outer = 18.f;
	DrawLine(CenterX - Inner, CenterY - Inner, CenterX - Outer, CenterY - Outer, Color, 2.f);
	DrawLine(CenterX + Inner, CenterY - Inner, CenterX + Outer, CenterY - Outer, Color, 2.f);
	DrawLine(CenterX - Inner, CenterY + Inner, CenterX - Outer, CenterY + Outer, Color, 2.f);
	DrawLine(CenterX + Inner, CenterY + Inner, CenterX + Outer, CenterY + Outer, Color, 2.f);

	FString Label = FString::Printf(TEXT("-%d"), FMath::RoundToInt(Character.LastHitDamage));
	if (Character.bLastHitHeadshot)
	{
		Label += TEXT("  HEADSHOT");
	}
	if (Character.bLastHitKill)
	{
		Label += TEXT("  KILL");
	}
	DrawText(Label, Color, CenterX + 24.f, CenterY + 12.f - Age * 40.f, GEngine->GetMediumFont(), 1.f);
}

void AChupChupHUD::DrawInfo(const AChupChupCharacter& Character)
{
	UFont* LargeFont = GEngine->GetLargeFont();
	UFont* SmallFont = GEngine->GetSmallFont();
	const float Margin = 32.f;
	const float Bottom = Canvas->ClipY - Margin;
	const float Right = Canvas->ClipX - Margin;
	const FLinearColor TextColor(0.95f, 0.95f, 0.95f);
	const FLinearColor Panel(0.f, 0.f, 0.f, 0.45f);

	// Health, bottom left.
	if (const UChupChupHealthComponent* Health = Character.GetHealthComponent())
	{
		DrawRect(Panel, Margin - 10.f, Bottom - 50.f, 170.f, 56.f);
		DrawText(FString::Printf(TEXT("+ %d"), FMath::CeilToInt(Health->GetHealth())), TextColor, Margin, Bottom - 44.f, LargeFont, 1.6f);
	}

	// Ammo and weapon, bottom right.
	if (const AChupChupWeapon* Weapon = Character.GetCurrentWeapon())
	{
		DrawRect(Panel, Right - 230.f, Bottom - 74.f, 240.f, 80.f);

		const bool bLow = Weapon->GetAmmoInMagazine() <= Weapon->GetMagazineSize() / 4;
		const FLinearColor AmmoColor = bLow ? FLinearColor(1.f, 0.3f, 0.25f) : TextColor;
		DrawRightAlignedText(FString::Printf(TEXT("%d / %d"), Weapon->GetAmmoInMagazine(), Weapon->GetReserveAmmo()), AmmoColor, Right, Bottom - 44.f, LargeFont, 1.6f);

		const FString Status = Weapon->IsReloading() ? FString::Printf(TEXT("%s  -  RELOADING"), *Weapon->GetWeaponName()) : Weapon->GetWeaponName();
		DrawRightAlignedText(Status, FLinearColor(0.75f, 0.75f, 0.8f), Right, Bottom - 68.f, SmallFont, 1.2f);
	}

	// Aim stats, top left.
	const float Accuracy = Character.ShotsFired > 0 ? 100.f * Character.ShotsHit / Character.ShotsFired : 0.f;
	const float HeadshotRate = Character.ShotsHit > 0 ? 100.f * Character.Headshots / Character.ShotsHit : 0.f;
	DrawRect(Panel, Margin - 10.f, Margin - 8.f, 220.f, 70.f);
	DrawText(FString::Printf(TEXT("Kills: %d"), Character.Kills), TextColor, Margin, Margin, SmallFont, 1.2f);
	DrawText(FString::Printf(TEXT("Accuracy: %.0f%%"), Accuracy), TextColor, Margin, Margin + 18.f, SmallFont, 1.2f);
	DrawText(FString::Printf(TEXT("Headshots: %.0f%%"), HeadshotRate), TextColor, Margin, Margin + 36.f, SmallFont, 1.2f);

	// Controls, top right.
	const FLinearColor HintColor(0.8f, 0.8f, 0.8f, 0.7f);
	DrawRightAlignedText(TEXT("WASD move | Shift walk | Ctrl crouch | Space jump"), HintColor, Right, Margin, SmallFont, 1.f);
	DrawRightAlignedText(TEXT("LMB fire | R reload | 1/2/Q weapons"), HintColor, Right, Margin + 16.f, SmallFont, 1.f);
}
