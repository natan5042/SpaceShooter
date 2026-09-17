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

	// La secousse ne bouge que les objets du jeu, pas le décor ni l'interface.
	Shake = GameMode->ShakeOffset;

	DrawBackground(GameMode);
	DrawStars(GameMode);

	if (GameMode->State == EGameState::Playing)
	{
		DrawAsteroids(GameMode);
		DrawProjectiles(GameMode);
		DrawShip(GameMode);
		DrawParticles(GameMode);
		DrawPopups(GameMode);
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

	// Deux cercles pour le haut du cœur.
	DrawCircle(X - Size * 0.3f, Y - Size * 0.2f, Size * 0.4f, Red);
	DrawCircle(X + Size * 0.3f, Y - Size * 0.2f, Size * 0.4f, Red);

	// Un triangle pour la pointe du bas.
	for (float Step = 0.0f; Step <= 1.0f; Step += 0.08f)
	{
		float Width = Size * 1.35f * (1.0f - Step);
		DrawRect(Red, X - Width * 0.5f, Y - Size * 0.15f + Step * Size * 0.9f, Width, 3.0f);
	}
}

// ---------------------------------------------------------------- Décor

void AShooterHUD::DrawBackground(AShooterGameMode* GameMode)
{
	float Width = Canvas->SizeX;
	float Height = Canvas->SizeY;

	// Dégradé vertical : violet foncé en haut, bleu nuit en bas.
	FLinearColor Top(0.07f, 0.04f, 0.13f, 1.0f);
	FLinearColor Bottom(0.02f, 0.03f, 0.09f, 1.0f);

	int32 Bands = 24;
	float BandHeight = Height / Bands;
	for (int32 i = 0; i < Bands; i++)
	{
		float Alpha = (float)i / (Bands - 1);
		DrawBox(0.0f, i * BandHeight, Width, BandHeight + 1.0f, FMath::Lerp(Top, Bottom, Alpha));
	}

	// Une planète lointaine, pour que le fond ne soit pas vide.
	float PlanetX = Width * 0.78f;
	float PlanetY = Height * 0.28f;
	float PlanetRadius = Height * 0.15f;

	DrawCircle(PlanetX, PlanetY, PlanetRadius * 1.25f, FLinearColor(0.35f, 0.25f, 0.55f, 0.07f));
	DrawCircle(PlanetX, PlanetY, PlanetRadius, FLinearColor(0.13f, 0.11f, 0.22f, 1.0f));
	DrawCircle(PlanetX - PlanetRadius * 0.22f, PlanetY - PlanetRadius * 0.18f, PlanetRadius * 0.82f,
		FLinearColor(0.19f, 0.16f, 0.31f, 1.0f));
}

void AShooterHUD::DrawStars(AShooterGameMode* GameMode)
{
	for (int32 i = 0; i < GameMode->Stars.Num(); i++)
	{
		const FStar& Star = GameMode->Stars[i];

		// Les étoiles proches sont plus grosses et plus lumineuses.
		FLinearColor Color(Star.Brightness, Star.Brightness, Star.Brightness * 1.1f, 1.0f);
		DrawBox(Star.Position.X, Star.Position.Y, Star.Size, Star.Size, Color);
	}
}

// ---------------------------------------------------------------- Acteurs

void AShooterHUD::DrawShip(AShooterGameMode* GameMode)
{
	if (!IsValid(GameMode->Ship))
	{
		return;
	}

	float X = GameMode->Ship->Position.X + Shake.X;
	float Y = GameMode->Ship->Position.Y + Shake.Y;
	float Lean = GameMode->Ship->Lean;

	// Le vaisseau clignote quand il est invulnérable.
	float Alpha = 1.0f;
	if (GameMode->Ship->InvulnerabilityTime > 0.0f)
	{
		Alpha = 0.3f + 0.7f * FMath::Abs(FMath::Sin(GameMode->GameTime * 15.0f));
	}

	FLinearColor Body(0.85f, 0.88f, 0.95f, Alpha);
	FLinearColor Wing(0.30f, 0.45f, 0.75f, Alpha);
	FLinearColor Cockpit(0.35f, 0.9f, 1.0f, Alpha);

	// Flamme du réacteur : elle vacille et part à l'opposé du virage.
	float Flame = 14.0f + FMath::FRandRange(0.0f, 10.0f);
	DrawBox(X - 5.0f - Lean * 4.0f, Y + 18.0f, 10.0f, Flame, FLinearColor(1.0f, 0.45f, 0.1f, Alpha));
	DrawBox(X - 2.0f - Lean * 4.0f, Y + 18.0f, 4.0f, Flame * 0.6f, FLinearColor(1.0f, 0.9f, 0.4f, Alpha));

	// Ailes : elles montent d'un côté et descendent de l'autre quand le vaisseau penche.
	DrawBox(X - 26.0f, Y + 2.0f - Lean * 5.0f, 20.0f, 10.0f, Wing);
	DrawBox(X + 6.0f, Y + 2.0f + Lean * 5.0f, 20.0f, 10.0f, Wing);

	// Corps et cockpit.
	DrawBox(X - 7.0f, Y - 20.0f, 14.0f, 38.0f, Body);
	DrawBox(X - 3.0f + Lean * 2.0f, Y - 30.0f, 6.0f, 12.0f, Body);
	DrawBox(X - 4.0f + Lean * 2.0f, Y - 14.0f, 8.0f, 12.0f, Cockpit);
}

void AShooterHUD::DrawAsteroids(AShooterGameMode* GameMode)
{
	for (int32 i = 0; i < GameMode->Asteroids.Num(); i++)
	{
		AAsteroid* Asteroid = GameMode->Asteroids[i];

		float X = Asteroid->Position.X + Shake.X;
		float Y = Asteroid->Position.Y + Shake.Y;
		float Radius = Asteroid->Radius;
		float Angle = Asteroid->Angle;

		FLinearColor Rock(0.60f, 0.42f, 0.30f, 1.0f);
		FLinearColor Dark(0.42f, 0.29f, 0.20f, 1.0f);

		// L'astéroïde blanchit une fraction de seconde quand il est touché.
		if (Asteroid->HitFlash > 0.0f)
		{
			float Flash = FMath::Clamp(Asteroid->HitFlash, 0.0f, 1.0f);
			Rock = FMath::Lerp(Rock, FLinearColor::White, Flash * 0.8f);
			Dark = FMath::Lerp(Dark, FLinearColor::White, Flash * 0.5f);
		}

		// Trois bosses sur le bord cassent la silhouette et tournent avec le rocher.
		for (int32 b = 0; b < 3; b++)
		{
			float BumpAngle = Angle + b * 2.09f;
			DrawCircle(X + FMath::Cos(BumpAngle) * Radius * 0.75f,
				Y + FMath::Sin(BumpAngle) * Radius * 0.75f,
				Radius * 0.38f, Rock);
		}

		DrawCircle(X, Y, Radius, Rock);

		// Deux cratères plus sombres, qui tournent eux aussi.
		DrawCircle(X + FMath::Cos(Angle + 1.0f) * Radius * 0.35f,
			Y + FMath::Sin(Angle + 1.0f) * Radius * 0.35f,
			Radius * 0.24f, Dark);
		DrawCircle(X + FMath::Cos(Angle + 3.6f) * Radius * 0.40f,
			Y + FMath::Sin(Angle + 3.6f) * Radius * 0.40f,
			Radius * 0.16f, Dark);
	}
}

void AShooterHUD::DrawProjectiles(AShooterGameMode* GameMode)
{
	for (int32 i = 0; i < GameMode->Projectiles.Num(); i++)
	{
		float X = GameMode->Projectiles[i]->Position.X + Shake.X;
		float Y = GameMode->Projectiles[i]->Position.Y + Shake.Y;

		// Traînée : trois rectangles de plus en plus transparents derrière le tir.
		for (int32 t = 1; t <= 3; t++)
		{
			DrawBox(X - 2.0f, Y + t * 14.0f, 4.0f, 14.0f,
				FLinearColor(0.3f, 0.8f, 1.0f, 0.30f - t * 0.08f));
		}

		DrawBox(X - 4.0f, Y - 14.0f, 8.0f, 24.0f, FLinearColor(0.3f, 0.8f, 1.0f, 0.6f));
		DrawBox(X - 2.0f, Y - 12.0f, 4.0f, 20.0f, FLinearColor(0.9f, 1.0f, 1.0f, 1.0f));
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

		DrawRect(Color, Particle.Position.X + Shake.X, Particle.Position.Y + Shake.Y, Particle.Size, Particle.Size);
	}
}

void AShooterHUD::DrawPopups(AShooterGameMode* GameMode)
{
	UFont* Font = GEngine->GetMediumFont();

	for (int32 i = 0; i < GameMode->Popups.Num(); i++)
	{
		const FPopup& Popup = GameMode->Popups[i];

		FLinearColor Color(1.0f, 0.85f, 0.4f, FMath::Clamp(Popup.Life * 1.5f, 0.0f, 1.0f));
		DrawTextCentered(Popup.Text, Color, Popup.Position.X, Popup.Position.Y, 1.3f, Font);
	}
}

// ---------------------------------------------------------------- Interface

void AShooterHUD::DrawInterface(AShooterGameMode* GameMode)
{
	UFont* BigFont = GEngine->GetLargeFont();
	UFont* SmallFont = GEngine->GetMediumFont();
	FLinearColor White(1.0f, 1.0f, 1.0f, 1.0f);
	FLinearColor Grey(0.65f, 0.65f, 0.75f, 1.0f);

	// Score et temps de survie en haut à gauche.
	DrawText(FString::Printf(TEXT("Score : %d"), GameMode->Score), White, 30.0f, 25.0f, BigFont, 1.6f);

	int32 Seconds = FMath::FloorToInt(GameMode->GameTime);
	DrawText(FString::Printf(TEXT("Temps : %02d:%02d"), Seconds / 60, Seconds % 60), Grey, 32.0f, 70.0f, SmallFont, 1.1f);

	// Vies en haut à droite.
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

	// Le titre respire doucement, avec une ombre bleue décalée.
	float Pulse = 3.4f + 0.06f * FMath::Sin(GetWorld()->GetTimeSeconds() * 1.6f);
	DrawTextCentered(TEXT("SPACE SHOOTER"), FLinearColor(0.2f, 0.5f, 0.9f, 0.7f), CenterX + 4.0f, CenterY - 216.0f, Pulse, BigFont);
	DrawTextCentered(TEXT("SPACE SHOOTER"), White, CenterX, CenterY - 220.0f, Pulse, BigFont);

	DrawTextCentered(GameMode->TeamMembers, Blue, CenterX, CenterY - 140.0f, 1.4f, SmallFont);

	// Les deux choix du menu : le choix sélectionné est en blanc avec des flèches.
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

	if (GameMode->BestScore > 0)
	{
		DrawTextCentered(FString::Printf(TEXT("Meilleur score : %d"), GameMode->BestScore),
			FLinearColor(1.0f, 0.85f, 0.4f, 1.0f), CenterX, CenterY + 190.0f, 1.2f, SmallFont);
	}

	DrawTextCentered(TEXT("Flèches pour se déplacer — Espace pour tirer"), Grey, CenterX, CenterY + 240.0f, 1.2f, SmallFont);
}

void AShooterHUD::DrawGameOver(AShooterGameMode* GameMode)
{
	UFont* BigFont = GEngine->GetLargeFont();
	UFont* SmallFont = GEngine->GetMediumFont();

	float CenterX = Canvas->SizeX * 0.5f;
	float CenterY = Canvas->SizeY * 0.5f;

	DrawTextCentered(TEXT("PARTIE TERMINÉE"), FLinearColor(1.0f, 0.35f, 0.35f, 1.0f),
		CenterX, CenterY - 120.0f, 3.0f, BigFont);

	DrawTextCentered(FString::Printf(TEXT("Score : %d"), GameMode->Score), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f),
		CenterX, CenterY, 2.0f, BigFont);

	DrawTextCentered(FString::Printf(TEXT("Meilleur score : %d"), GameMode->BestScore),
		FLinearColor(1.0f, 0.85f, 0.4f, 1.0f), CenterX, CenterY + 70.0f, 1.3f, SmallFont);

	DrawTextCentered(TEXT("Entrée pour rejouer — Échap pour le menu"), FLinearColor(0.7f, 0.7f, 0.8f, 1.0f),
		CenterX, CenterY + 150.0f, 1.3f, SmallFont);
}
