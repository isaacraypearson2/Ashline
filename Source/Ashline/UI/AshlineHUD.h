#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AshlineTypes.h"
#include "AshlineHUD.generated.h"

class AAshlineGameMode;
class UAshlineSaveGame;
class UAshlineCombatFeedback;
class AAshlineCharacter;
class UFont;

struct FAshlineHUDPalette
{
	FLinearColor Gold = FLinearColor(0.95f, 0.86f, 0.55f);
	FLinearColor White = FLinearColor(0.92f, 0.93f, 0.9f);
	FLinearColor Dim = FLinearColor(0.55f, 0.56f, 0.52f);
	FLinearColor Health = FLinearColor(0.75f, 0.9f, 0.55f);
	FLinearColor Armor = FLinearColor(0.45f, 0.72f, 0.95f);
	FLinearColor Danger = FLinearColor(0.92f, 0.22f, 0.18f);
	FLinearColor Hit = FLinearColor(0.95f, 0.95f, 0.95f);
	FLinearColor Kill = FLinearColor(0.95f, 0.82f, 0.2f);
	FLinearColor Panel = FLinearColor(0.02f, 0.025f, 0.03f, 0.62f);
};

struct FAshlineHUDMetrics
{
	float Pad = 36.f;
	float Scale = 1.f;
	bool bHandheld = false;
	float AmmoScale = 1.35f;
	float PromptScale = 1.f;
	float BarH = 10.f;
};

UCLASS()
class ASHLINE_API AAshlineHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ashline|HUD")
	bool bDrawDebugCombatHUD = true;

protected:
	void DrawFrontend(AAshlineGameMode* GameMode, UAshlineSaveGame* Save);
	void DrawBriefing(AAshlineGameMode* GameMode);
	void DrawCombat(AAshlineGameMode* GameMode);
	void DrawMissionComplete(AAshlineGameMode* GameMode);
	void DrawPause(AAshlineGameMode* GameMode);
	void DrawSettings(AAshlineGameMode* GameMode);
	void DrawTextLine(const FString& Text, float X, float Y, const FLinearColor& Color);
	void DrawTextScaled(const FString& Text, float X, float Y, const FLinearColor& Color, float Scale, bool bShadow = true);
	void FillRect(float X, float Y, float W, float H, const FLinearColor& Color);
	void DrawBar(float X, float Y, float W, float H, float Alpha, const FLinearColor& Fill, const FLinearColor& Back);
	void DrawCrosshair(AAshlineCharacter* Character, UAshlineCombatFeedback* Feedback);
	void DrawWeaponPlate(AAshlineCharacter* Character);
	void DrawVitals(AAshlineCharacter* Character);
	void DrawObjectives(AAshlineGameMode* GameMode);
	void DrawObjectiveMarker(AAshlineGameMode* GameMode);
	void DrawHitMarkers(UAshlineCombatFeedback* Feedback);
	void DrawKillFeed(UAshlineCombatFeedback* Feedback);
	void DrawInteract(UAshlineCombatFeedback* Feedback);
	void DrawVignette(UAshlineCombatFeedback* Feedback);
	void DrawSubtitle(UAshlineCombatFeedback* Feedback);
	void DrawWrapped(const FString& Text, float X, float Y, float MaxWidth, const FLinearColor& Color, float Scale);

	FAshlineHUDMetrics BuildMetrics() const;
	FAshlineHUDPalette BuildPalette() const;
	FAshlineFeelSettings GetFeel() const;
	UFont* FontForScale(float Scale) const;

	FAshlineHUDMetrics Metrics;
	FAshlineHUDPalette Palette;
};
