#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameMode.generated.h"

class AShipPawn;
class AAsteroid;
class AProjectile;

// Les trois ecrans du jeu.
UENUM(BlueprintType)
enum class EGameState : uint8
{
	Menu,
	Playing,
	GameOver
};

// Petit carre colore utilise pour les explosions et les tirs.
struct FParticle
{
	FVector2D Position = FVector2D::ZeroVector;
	FVector2D Velocity = FVector2D::ZeroVector;
	float Life = 0.0f;
	float Size = 4.0f;
	FLinearColor Color = FLinearColor::White;
};

// Etoile du fond qui descend a l'ecran.
struct FStar
{
	FVector2D Position = FVector2D::ZeroVector;
	float Speed = 40.0f;
};

// GameMode : il gere le menu, la partie, les asteroides, les collisions et le score.
UCLASS()
class SPACESHOOTER_API AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShooterGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// On ne cree pas de vaisseau tant que le joueur est dans le menu.
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// Reglages : modifiables dans un Blueprint derive de ce GameMode.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reglages")
	float ShipSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reglages")
	float FireInterval = 0.16f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reglages")
	float ProjectileSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reglages")
	int32 StartingLives = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reglages")
	float AsteroidMinSpeed = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reglages")
	float AsteroidMaxSpeed = 320.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reglages")
	float SpawnDelay = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reglages")
	FString TeamMembers = TEXT("Natan - Arnaud");

	// Actions du menu.
	UFUNCTION(BlueprintCallable, Category = "Jeu")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Jeu")
	void GoToMenu();

	UFUNCTION(BlueprintCallable, Category = "Jeu")
	void QuitGame();

	void MoveMenuSelection(int32 Delta);
	void ValidateMenu();
	void Fire();

	// Etat de la partie, lu par le HUD pour l'affichage.
	EGameState State = EGameState::Menu;
	int32 MenuIndex = 0;
	int32 Score = 0;
	int32 Lives = 3;
	float GameTime = 0.0f;
	FVector2D ScreenSize = FVector2D(1920.0f, 1080.0f);

	UPROPERTY()
	AShipPawn* Ship = nullptr;

	UPROPERTY()
	TArray<AAsteroid*> Asteroids;

	UPROPERTY()
	TArray<AProjectile*> Projectiles;

	TArray<FParticle> Particles;
	TArray<FStar> Stars;

private:
	void SpawnAsteroid();
	void CheckCollisions();
	void AddExplosion(const FVector2D& Position, const FLinearColor& Color, int32 Count);
	void ClearGame();
	void LoseLife();

	float SpawnTimer = 0.0f;
};
