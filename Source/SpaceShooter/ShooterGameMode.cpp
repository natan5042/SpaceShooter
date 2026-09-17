#include "ShooterGameMode.h"
#include "ShooterPlayerController.h"
#include "ShooterHUD.h"
#include "ShipPawn.h"
#include "Asteroid.h"
#include "Projectile.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AShooterGameMode::AShooterGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerControllerClass = AShooterPlayerController::StaticClass();
	HUDClass = AShooterHUD::StaticClass();
	DefaultPawnClass = nullptr;
}

void AShooterGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Vide : le vaisseau est créé seulement quand on lance une partie.
}

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Le fond est fait de trois couches d'étoiles : les plus lointaines sont
	// petites, sombres et lentes, ce qui donne un effet de profondeur.
	for (int32 i = 0; i < 150; i++)
	{
		int32 Layer = i % 3;

		FStar Star;
		Star.Position = FVector2D(FMath::FRandRange(0.0f, ScreenSize.X), FMath::FRandRange(0.0f, ScreenSize.Y));
		Star.Speed = 25.0f + Layer * 45.0f;
		Star.Size = 1.0f + Layer * 1.0f;
		Star.Brightness = 0.30f + Layer * 0.25f;
		Stars.Add(Star);
	}
}

void AShooterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// On récupère la taille de la fenêtre pour que le jeu s'adapte.
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D NewSize;
		GEngine->GameViewport->GetViewportSize(NewSize);
		if (NewSize.X > 0.0f && NewSize.Y > 0.0f)
		{
			ScreenSize = NewSize;
		}
	}

	// Les étoiles descendent et reviennent en haut.
	for (int32 i = 0; i < Stars.Num(); i++)
	{
		Stars[i].Position.Y += Stars[i].Speed * DeltaTime;
		if (Stars[i].Position.Y > ScreenSize.Y)
		{
			Stars[i].Position.Y = 0.0f;
			Stars[i].Position.X = FMath::FRandRange(0.0f, ScreenSize.X);
		}
	}

	// Les particules avancent puis disparaissent.
	for (int32 i = Particles.Num() - 1; i >= 0; i--)
	{
		Particles[i].Life -= DeltaTime;
		if (Particles[i].Life <= 0.0f)
		{
			Particles.RemoveAt(i);
		}
		else
		{
			Particles[i].Position += Particles[i].Velocity * DeltaTime;
		}
	}

	// Les points gagnés montent doucement avant de s'effacer.
	for (int32 i = Popups.Num() - 1; i >= 0; i--)
	{
		Popups[i].Life -= DeltaTime;
		if (Popups[i].Life <= 0.0f)
		{
			Popups.RemoveAt(i);
		}
		else
		{
			Popups[i].Position.Y -= 50.0f * DeltaTime;
		}
	}

	// Secousse de l'écran : elle s'atténue toute seule.
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= DeltaTime;
		float Force = ShakeForce * FMath::Max(0.0f, ShakeTime) * 4.0f;
		ShakeOffset = FVector2D(FMath::FRandRange(-Force, Force), FMath::FRandRange(-Force, Force));
	}
	else
	{
		ShakeOffset = FVector2D::ZeroVector;
	}

	if (State != EGameState::Playing)
	{
		return;
	}

	GameTime += DeltaTime;

	// Apparition d'un astéroïde à intervalle aléatoire, de plus en plus vite.
	SpawnTimer -= DeltaTime;
	if (SpawnTimer <= 0.0f)
	{
		SpawnAsteroid();

		float Delay = SpawnDelay - GameTime * 0.006f;
		Delay = FMath::Max(0.4f, Delay);
		SpawnTimer = Delay * FMath::FRandRange(0.7f, 1.3f);
	}

	// On supprime les projectiles sortis de l'écran.
	for (int32 i = Projectiles.Num() - 1; i >= 0; i--)
	{
		if (!IsValid(Projectiles[i]) || Projectiles[i]->Position.Y < -50.0f)
		{
			if (IsValid(Projectiles[i]))
			{
				Projectiles[i]->Destroy();
			}
			Projectiles.RemoveAt(i);
		}
	}

	// Idem pour les astéroïdes.
	for (int32 i = Asteroids.Num() - 1; i >= 0; i--)
	{
		if (!IsValid(Asteroids[i]) || Asteroids[i]->IsOutOfScreen(ScreenSize))
		{
			if (IsValid(Asteroids[i]))
			{
				Asteroids[i]->Destroy();
			}
			Asteroids.RemoveAt(i);
		}
	}

	CheckCollisions();
}

// ---------------------------------------------------------------- Menu

void AShooterGameMode::MoveMenuSelection(int32 Delta)
{
	MenuIndex = MenuIndex + Delta;
	if (MenuIndex < 0)
	{
		MenuIndex = 1;
	}
	if (MenuIndex > 1)
	{
		MenuIndex = 0;
	}
}

void AShooterGameMode::ValidateMenu()
{
	if (State == EGameState::Menu)
	{
		if (MenuIndex == 0)
		{
			StartGame();
		}
		else
		{
			QuitGame();
		}
	}
	else if (State == EGameState::GameOver)
	{
		StartGame();
	}
}

void AShooterGameMode::StartGame()
{
	ClearGame();

	Score = 0;
	Lives = StartingLives;
	GameTime = 0.0f;
	SpawnTimer = 1.0f;
	State = EGameState::Playing;

	// On crée le vaisseau et on le donne au joueur.
	Ship = GetWorld()->SpawnActor<AShipPawn>();
	if (Ship)
	{
		Ship->Position = FVector2D(ScreenSize.X * 0.5f, ScreenSize.Y - 120.0f);
		Ship->Speed = ShipSpeed;
		Ship->FireInterval = FireInterval;
		Ship->InvulnerabilityTime = 1.5f;

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			PlayerController->Possess(Ship);
		}
	}
}

void AShooterGameMode::GoToMenu()
{
	ClearGame();
	State = EGameState::Menu;
	MenuIndex = 0;
}

void AShooterGameMode::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, UGameplayStatics::GetPlayerController(this, 0), EQuitPreference::Quit, false);
}

void AShooterGameMode::ClearGame()
{
	for (int32 i = 0; i < Asteroids.Num(); i++)
	{
		if (IsValid(Asteroids[i]))
		{
			Asteroids[i]->Destroy();
		}
	}
	Asteroids.Empty();

	for (int32 i = 0; i < Projectiles.Num(); i++)
	{
		if (IsValid(Projectiles[i]))
		{
			Projectiles[i]->Destroy();
		}
	}
	Projectiles.Empty();

	if (IsValid(Ship))
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			PlayerController->UnPossess();
		}
		Ship->Destroy();
	}
	Ship = nullptr;

	Particles.Empty();
	Popups.Empty();
}

// ---------------------------------------------------------------- Tir

void AShooterGameMode::Fire()
{
	if (State != EGameState::Playing || !IsValid(Ship))
	{
		return;
	}

	// Le vaisseau refuse de tirer si le délai entre deux tirs n'est pas écoulé.
	if (!Ship->CanFire())
	{
		return;
	}

	FVector2D Start = Ship->Position + FVector2D(0.0f, -30.0f);

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>();
	if (Projectile)
	{
		Projectile->Position = Start;
		Projectile->Speed = ProjectileSpeed;
		Projectiles.Add(Projectile);
	}

	// Effet visuel du tir : une gerbe d'étincelles bleues et un petit recul.
	AddExplosion(Start, FLinearColor(0.4f, 0.9f, 1.0f, 1.0f), 5);
	AddShake(0.6f);
}

// ---------------------------------------------------------------- Astéroïdes

void AShooterGameMode::SpawnAsteroid()
{
	float Radius = FMath::FRandRange(25.0f, 70.0f);
	FVector2D Position;
	int32 Edge = FMath::RandRange(0, 2);

	// Apparition sur un bord de l'écran, à une position aléatoire.
	if (Edge == 1)
	{
		Position = FVector2D(-Radius, FMath::FRandRange(0.0f, ScreenSize.Y * 0.5f));
	}
	else if (Edge == 2)
	{
		Position = FVector2D(ScreenSize.X + Radius, FMath::FRandRange(0.0f, ScreenSize.Y * 0.5f));
	}
	else
	{
		Position = FVector2D(FMath::FRandRange(0.0f, ScreenSize.X), -Radius);
	}

	// Force initiale : soit vers le joueur, soit dans une direction aléatoire.
	FVector2D Direction;
	if (IsValid(Ship) && FMath::FRand() < 0.5f)
	{
		Direction = Ship->Position - Position;
		Direction.Normalize();
	}
	else
	{
		// On vise un point au hasard en bas de l'écran pour que l'astéroïde le traverse.
		FVector2D Target = FVector2D(FMath::FRandRange(0.0f, ScreenSize.X), ScreenSize.Y);
		Direction = Target - Position;
		Direction.Normalize();
	}

	AAsteroid* Asteroid = GetWorld()->SpawnActor<AAsteroid>();
	if (Asteroid)
	{
		Asteroid->Position = Position;
		Asteroid->Velocity = Direction * FMath::FRandRange(AsteroidMinSpeed, AsteroidMaxSpeed);
		Asteroid->Radius = Radius;
		// Nombre de tirs aléatoire pour détruire l'astéroïde.
		Asteroid->Life = FMath::RandRange(1, 3);
		// Chaque astéroïde tourne à sa propre vitesse, dans un sens ou dans l'autre.
		Asteroid->Angle = FMath::FRandRange(0.0f, 6.28f);
		Asteroid->Spin = FMath::FRandRange(-1.4f, 1.4f);
		Asteroids.Add(Asteroid);
	}
}

// ---------------------------------------------------------------- Collisions

void AShooterGameMode::CheckCollisions()
{
	// Projectiles contre astéroïdes.
	for (int32 i = Projectiles.Num() - 1; i >= 0; i--)
	{
		for (int32 j = Asteroids.Num() - 1; j >= 0; j--)
		{
			float Distance = FVector2D::Distance(Projectiles[i]->Position, Asteroids[j]->Position);
			if (Distance > Asteroids[j]->Radius)
			{
				continue;
			}

			Asteroids[j]->Life--;
			Asteroids[j]->HitFlash = 1.0f;

			if (Asteroids[j]->Life <= 0)
			{
				// Explosion, points et secousse quand l'astéroïde est détruit.
				// Les gros astéroïdes rapportent davantage.
				int32 Points = 50 + FMath::RoundToInt(Asteroids[j]->Radius);
				Score += Points;

				AddExplosion(Asteroids[j]->Position, FLinearColor(1.0f, 0.6f, 0.2f, 1.0f), 20);
				AddPopup(Asteroids[j]->Position, FString::Printf(TEXT("+%d"), Points));
				AddShake(2.5f);

				Asteroids[j]->Destroy();
				Asteroids.RemoveAt(j);
			}
			else
			{
				AddExplosion(Projectiles[i]->Position, FLinearColor(1.0f, 0.8f, 0.4f, 1.0f), 6);
			}

			Projectiles[i]->Destroy();
			Projectiles.RemoveAt(i);
			break;
		}
	}

	// Astéroïdes contre vaisseau.
	if (!IsValid(Ship) || Ship->InvulnerabilityTime > 0.0f)
	{
		return;
	}

	for (int32 i = Asteroids.Num() - 1; i >= 0; i--)
	{
		float Distance = FVector2D::Distance(Asteroids[i]->Position, Ship->Position);
		if (Distance < Asteroids[i]->Radius + 18.0f)
		{
			AddExplosion(Asteroids[i]->Position, FLinearColor(1.0f, 0.3f, 0.2f, 1.0f), 25);
			Asteroids[i]->Destroy();
			Asteroids.RemoveAt(i);
			LoseLife();
			break;
		}
	}
}

void AShooterGameMode::LoseLife()
{
	Lives--;
	AddShake(6.0f);

	if (Lives <= 0)
	{
		AddExplosion(Ship->Position, FLinearColor(1.0f, 0.5f, 0.2f, 1.0f), 40);
		State = EGameState::GameOver;
		BestScore = FMath::Max(BestScore, Score);

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			PlayerController->UnPossess();
		}

		Ship->Destroy();
		Ship = nullptr;
	}
	else
	{
		// On replace le vaisseau au centre et on le rend invulnérable un moment.
		Ship->Position = FVector2D(ScreenSize.X * 0.5f, ScreenSize.Y - 120.0f);
		Ship->InvulnerabilityTime = 2.0f;
	}
}

// ---------------------------------------------------------------- Effets

void AShooterGameMode::AddExplosion(const FVector2D& Position, const FLinearColor& Color, int32 Count)
{
	for (int32 i = 0; i < Count; i++)
	{
		float Angle = FMath::FRandRange(0.0f, 6.28f);
		float Speed = FMath::FRandRange(80.0f, 350.0f);

		FParticle Particle;
		Particle.Position = Position;
		Particle.Velocity = FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Speed;
		Particle.Life = FMath::FRandRange(0.2f, 0.6f);
		Particle.Size = FMath::FRandRange(3.0f, 8.0f);
		Particle.Color = Color;

		Particles.Add(Particle);
	}
}

void AShooterGameMode::AddPopup(const FVector2D& Position, const FString& Text)
{
	FPopup Popup;
	Popup.Position = Position;
	Popup.Text = Text;
	Popup.Life = 0.9f;

	Popups.Add(Popup);
}

void AShooterGameMode::AddShake(float Force)
{
	// Une nouvelle secousse ne doit pas affaiblir celle qui est en cours.
	if (Force > ShakeForce || ShakeTime <= 0.0f)
	{
		ShakeForce = Force;
	}
	ShakeTime = 0.25f;
}
