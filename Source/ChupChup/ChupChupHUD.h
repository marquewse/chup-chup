#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ChupChupHUD.generated.h"

class AChupChupCharacter;

/** Canvas HUD: dynamic crosshair, hit markers, ammo, health and aim stats. */
UCLASS()
class CHUPCHUP_API AChupChupHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Crosshair")
	FLinearColor CrosshairColor = FLinearColor(0.f, 1.f, 0.6f);

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrosshairLength = 8.f;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrosshairThickness = 2.f;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	bool bCenterDot = true;

private:
	void DrawCrosshair(const AChupChupCharacter& Character, float CenterX, float CenterY);
	void DrawHitMarker(const AChupChupCharacter& Character, float CenterX, float CenterY, float Now);
	void DrawInfo(const AChupChupCharacter& Character);
	void DrawOutlinedLine(float X1, float Y1, float X2, float Y2, const FLinearColor& Color, float Thickness);
	void DrawRightAlignedText(const FString& Text, const FLinearColor& Color, float RightX, float Y, UFont* Font, float Scale);
};
