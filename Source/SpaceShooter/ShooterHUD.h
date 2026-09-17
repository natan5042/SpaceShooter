#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class AShooterGameMode;
class UFont;

// Tout le jeu est dessiné ici sur le canvas : pas besoin d'assets.
UCLASS()
class SPACESHOOTER_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	// Petits outils de dessin.
	void DrawBox(float X, float Y, float Width, float Height, const FLinearColor& Color);
	void DrawCircle(float X, float Y, float Radius, const FLinearColor& Color);
	void DrawTextCentered(const FString& Text, const FLinearColor& Color, float X, float Y, float Scale, UFont* Font);
	void DrawHeart(float X, float Y, float Size);

	// Les différentes parties de l'affichage.
	void DrawBackground(AShooterGameMode* GameMode);
	void DrawStars(AShooterGameMode* GameMode);
	void DrawParticles(AShooterGameMode* GameMode);
	void DrawPopups(AShooterGameMode* GameMode);
	void DrawShip(AShooterGameMode* GameMode);
	void DrawAsteroids(AShooterGameMode* GameMode);
	void DrawProjectiles(AShooterGameMode* GameMode);
	void DrawInterface(AShooterGameMode* GameMode);
	void DrawMenu(AShooterGameMode* GameMode);
	void DrawGameOver(AShooterGameMode* GameMode);

	// Décalage de la secousse d'écran, ajouté aux objets du jeu.
	FVector2D Shake = FVector2D::ZeroVector;
};
