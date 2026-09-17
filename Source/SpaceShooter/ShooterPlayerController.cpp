#include "ShooterPlayerController.h"
#include "ShooterGameMode.h"
#include "ShipPawn.h"
#include "Engine/World.h"

AShooterPlayerController::AShooterPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = false;
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Toutes les touches vont au jeu.
	SetInputMode(FInputModeGameOnly());
}

void AShooterPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	AShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	if (!GameMode)
	{
		return;
	}

	// Échap : retour au menu, ou quitter si on y est déjà.
	if (WasInputKeyJustPressed(EKeys::Escape))
	{
		if (GameMode->State == EGameState::Menu)
		{
			GameMode->QuitGame();
		}
		else
		{
			GameMode->GoToMenu();
		}
		return;
	}

	if (GameMode->State == EGameState::Playing)
	{
		// Déplacement du vaisseau avec les flèches.
		FVector2D Direction = FVector2D::ZeroVector;

		if (IsInputKeyDown(EKeys::Left))
		{
			Direction.X = Direction.X - 1.0f;
		}
		if (IsInputKeyDown(EKeys::Right))
		{
			Direction.X = Direction.X + 1.0f;
		}
		if (IsInputKeyDown(EKeys::Up))
		{
			Direction.Y = Direction.Y - 1.0f;
		}
		if (IsInputKeyDown(EKeys::Down))
		{
			Direction.Y = Direction.Y + 1.0f;
		}

		if (IsValid(GameMode->Ship))
		{
			GameMode->Ship->Move(Direction, GameMode->ScreenSize, DeltaTime);
		}

		// Tir avec la barre espace.
		if (IsInputKeyDown(EKeys::SpaceBar))
		{
			GameMode->Fire();
		}
	}
	else
	{
		// Navigation dans le menu.
		if (WasInputKeyJustPressed(EKeys::Up))
		{
			GameMode->MoveMenuSelection(-1);
		}
		if (WasInputKeyJustPressed(EKeys::Down))
		{
			GameMode->MoveMenuSelection(1);
		}
		if (WasInputKeyJustPressed(EKeys::Enter) || WasInputKeyJustPressed(EKeys::SpaceBar))
		{
			GameMode->ValidateMenu();
		}
	}
}
