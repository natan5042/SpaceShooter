#include "ShooterHUD.h"
#include "ShooterGameMode.h"
#include "ShipPawn.h"
#include "Asteroid.h"
#include "Projectile.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	AShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	if (!GameMode)
	{
		return;
	}

	// Fond noir bleute.
	DrawBox(0.0f, 0.0f, Canvas->SizeX, Canvas->SizeY, FLinearColor(0.04f, 0.03f, 0.09f, 1.0f));
	DrawStars(GameMode);

	if (GameMode->State == EGameState::Playing)
	{
		DrawAsteroids(GameMode);
		DrawProjectiles(GameMode);
		DrawShip(GameMode);
		DrawParticles(GameMode);
		DrawInterface(GameMode);
	}
	else if (GameMode->State == EGameState::GameOver)
	{
		DrawParticles(GameMode);
		DrawGameOver(GameMode);
	}
	else
	{
		DrawMenu(GameMode);
	}
}

// ---------------------------------------------------------------- Outils de dessin

void AShooterHUD::DrawBox(float X, float Y, float Width, float Height, const FLinearColor& Color)
{
	DrawRect(Color, X, Y, Width, Height);
}

void AShooterHUD::DrawCircle(float X, float Y, float Radius, const FLinearColor& Color)
{
	// On dessine le cercle ligne par ligne avec des rectangles.
	for (float Offset = -Radius; Offset <= Radius; Offset += 3.0f)
	{
		float HalfWidth = FMath::Sqrt(FMath::Max(0.0f, Radius * Radius - Offset * Offset));
		DrawRect(Color, X - HalfWidth, Y + Offset, HalfWidth * 2.0f, 3.0f);
	}
}

void AShooterHUD::DrawTextCentered(const FString& Text, const FLinearColor& Color, float X, float Y, float Scale, UFont* Font)
{
	float Width = 0.0f;
	float Height = 0.0f;
	GetTextSize(Text, Width, Height, Font, Scale);

	DrawText(Text, Color, X - Width * 0.5f, Y - Height * 0.5f, Font, Scale);
}

void AShooterHUD::DrawHeart(float X, float Y, float Size)
{
	FLinearColor Red(1.0f, 0.25f, 0.35f, 1.0f);

	// Deux cercles pour le haut du coeur.
	DrawCircle(X - Size * 0.3f, Y - Size * 0.2f, Size * 0.4f, Red);
	DrawCircle(X + Size * 0.3f, Y - Size * 0.2f, Size * 0.4f, Red);

	// Un triangle pour la pointe du bas.
	for (float Step = 0.0f; Step <= 1.0f; Step += 0.08f)
	{
		float Width = Size * 1.35f * (1.0f - Step);
		DrawRect(Red, X - Width * 0.5f, Y - Size * 0.15f + Step * Size * 0.9f, Width, 3.0f);
	}
}

// ---------------------------------------------------------------- Decor

void AShooterHUD::DrawStars(AShooterGameMode* GameMode)
{
	FLinearColor White(0.8f, 0.8f, 0.9f, 1.0f);

	for (int32 i = 0; i < GameMode->Stars.Num(); i++)
	{
		FVector2D Position = GameMode->Stars[i].Position;
		DrawRect(White, Position.X, Position.Y, 2.0f, 2.0f);
	}
}

void AShooterHUD::DrawParticles(AShooterGameMode* GameMode)
{
	for (int32 i = 0; i < GameMode->Particles.Num(); i++)
	{
		const FParticle& Particle = GameMode->Particles[i];

		// La particule devient transparente en disparaissant.
		FLinearColor Color = Particle.Color;
		Color.A = FMath::Clamp(Particle.Life * 2.0f, 0.0f, 1.0f);

		DrawRect(Color, Particle.Position.X, Particle.Position.Y, Particle.Size, Particle.Size);
	}
}

// ---------------------------------------------------------------- Acteurs

void AShooterHUD::DrawShip(AShooterGameMode* GameMode)
{
	if (!IsValid(GameMode->Ship))
	{
		return;
	}

	float X = GameMode->Ship->Position.X;
	float Y = GameMode->Ship->Position.Y;

	// Le vaisseau clignote quand il est invulnerable.
	float Alpha = 1.0f;
	if (GameMode->Ship->InvulnerabilityTime > 0.0f)
	{
		Alpha = 0.3f + 0.7f * FMath::Abs(FMath::Sin(GameMode->GameTime * 15.0f));
	}

	FLinearColor Body(0.85f, 0.88f, 0.95f, Alpha);
	FLinearColor Wing(0.30f, 0.45f, 0.75f, Alpha);
	FLinearColor Cockpit(0.35f, 0.9f, 1.0f, Alpha);

	// Flamme du reacteur.
	float Flame = 14.0f + FMath::FRandRange(0.0f, 10.0f);
	DrawBox(X - 5.0f, Y + 18.0f, 10.0f, Flame, FLinearColor(1.0f, 0.45f, 0.1f, Alpha));
	DrawBox(X - 2.0f, Y + 18.0f, 4.0f, Flame * 0.6f, FLinearColor(1.0f, 0.9f, 0.4f, Alpha));

	// Ailes, corps et cockpit.
	DrawBox(X - 26.0f, Y + 2.0f, 20.0f, 10.0f, Wing);
	DrawBox(X + 6.0f, Y + 2.0f, 20.0f, 10.0f, Wing);
	DrawBox(X - 7.0f, Y - 20.0f, 14.0f, 38.0f, Body);
	DrawBox(X - 3.0f, Y - 30.0f, 6.0f, 12.0f, Body);
	DrawBox(X - 4.0f, Y - 14.0f, 8.0f, 12.0f, Cockpit);
}

void AShooterHUD::DrawAsteroids(AShooterGameMode* GameMode)
{
	for (int32 i = 0; i < GameMode->Asteroids.Num(); i++)
	{
		AAsteroid* Asteroid = GameMode->Asteroids[i];

		float X = Asteroid->Position.X;
		float Y = Asteroid->Position.Y;
		float Radius = Asteroid->Radius;

		DrawCircle(X, Y, Radius, FLinearColor(0.60f, 0.42f, 0.30f, 1.0f));

		// Deux crateres plus sombres pour faire un peu de relief.
		DrawCircle(X - Radius * 0.3f, Y - Radius * 0.2f, Radius * 0.22f, FLinearColor(0.42f, 0.29f, 0.20f, 1.0f));
		DrawCircle(X + Radius * 0.25f, Y + Radius * 0.3f, Radius * 0.16f, FLinearColor(0.42f, 0.29f, 0.20f, 1.0f));
	}
}

void AShooterHUD::DrawProjectiles(AShooterGameMode* GameMode)
{
	for (int32 i = 0; i < GameMode->Projectiles.Num(); i++)
	{
		FVector2D Position = GameMode->Projectiles[i]->Position;

		DrawBox(Position.X - 4.0f, Position.Y - 14.0f, 8.0f, 24.0f, FLinearColor(0.3f, 0.8f, 1.0f, 0.6f));
		DrawBox(Position.X - 2.0f, Position.Y - 12.0f, 4.0f, 20.0f, FLinearColor(0.9f, 1.0f, 1.0f, 1.0f));
	}
}

// ---------------------------------------------------------------- Interface

void AShooterHUD::DrawInterface(AShooterGameMode* GameMode)
{
	UFont* BigFont = GEngine->GetLargeFont();
	FLinearColor White(1.0f, 1.0f, 1.0f, 1.0f);

	// Score en haut a gauche.
	DrawText(FString::Printf(TEXT("Score: %d"), GameMode->Score), White, 30.0f, 25.0f, BigFont, 1.6f);

	// Vies en haut a droite.
	for (int32 i = 0; i < GameMode->Lives; i++)
	{
		DrawHeart(Canvas->SizeX - 45.0f - i * 55.0f, 50.0f, 28.0f);
	}
}

void AShooterHUD::DrawMenu(AShooterGameMode* GameMode)
{
	UFont* BigFont = GEngine->GetLargeFont();
	UFont* SmallFont = GEngine->GetMediumFont();

	float CenterX = Canvas->SizeX * 0.5f;
	float CenterY = Canvas->SizeY * 0.5f;

	FLinearColor White(1.0f, 1.0f, 1.0f, 1.0f);
	FLinearColor Grey(0.6f, 0.6f, 0.7f, 1.0f);
	FLinearColor Blue(0.4f, 0.85f, 1.0f, 1.0f);

	DrawTextCentered(TEXT("SPACE SHOOTER"), White, CenterX, CenterY - 220.0f, 3.5f, BigFont);
	DrawTextCentered(GameMode->TeamMembers, Blue, CenterX, CenterY - 140.0f, 1.4f, SmallFont);

	// Les deux choix du menu : le choix selectionne est en blanc avec des fleches.
	FString Choices[2] = { TEXT("JOUER"), TEXT("QUITTER") };

	for (int32 i = 0; i < 2; i++)
	{
		float Y = CenterY + i * 80.0f;
		bool bSelected = (GameMode->MenuIndex == i);

		DrawTextCentered(Choices[i], bSelected ? White : Grey, CenterX, Y, 2.2f, BigFont);

		if (bSelected)
		{
			DrawTextCentered(TEXT(">"), Blue, CenterX - 150.0f, Y, 2.2f, BigFont);
			DrawTextCentered(TEXT("<"), Blue, CenterX + 150.0f, Y, 2.2f, BigFont);
		}
	}

	DrawTextCentered(TEXT("Fleches pour se deplacer - Espace pour tirer"), Grey, CenterX, CenterY + 240.0f, 1.2f, SmallFont);
}

void AShooterHUD::DrawGameOver(AShooterGameMode* GameMode)
{
	UFont* BigFont = GEngine->GetLargeFont();
	UFont* SmallFont = GEngine->GetMediumFont();

	float CenterX = Canvas->SizeX * 0.5f;
	float CenterY = Canvas->SizeY * 0.5f;

	DrawTextCentered(TEXT("PARTIE TERMINEE"), FLinearColor(1.0f, 0.35f, 0.35f, 1.0f), CenterX, CenterY - 120.0f, 3.0f, BigFont);
	DrawTextCentered(FString::Printf(TEXT("Score: %d"), GameMode->Score), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), CenterX, CenterY, 2.0f, BigFont);
	DrawTextCentered(TEXT("Entree pour rejouer - Echap pour le menu"), FLinearColor(0.7f, 0.7f, 0.8f, 1.0f), CenterX, CenterY + 120.0f, 1.3f, SmallFont);
}
