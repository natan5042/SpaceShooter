#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameMode.generated.h"

class AShipPawn;
class AAsteroid;
class AProjectile;

// Les trois écrans du jeu.
UENUM(BlueprintType)
enum class EGameState : uint8
{
	Menu,
	Playing,
	GameOver
};

// Petit carré coloré utilisé pour les explosions et les tirs.
struct FParticle
{
	FVector2D Position = FVector2D::ZeroVector;
	FVector2D Velocity = FVector2D::ZeroVector;
	float Life = 0.0f;
	float Size = 4.0f;
	FLinearColor Color = FLinearColor::White;
};

// Étoile du fond. Les étoiles des couches lointaines sont plus petites et plus lentes.
struct FStar
{
	FVector2D Position = FVector2D::ZeroVector;
	float Speed = 40.0f;
	float Size = 2.0f;
	float Brightness = 0.6f;
};

// Texte qui monte à l'écran quand on marque des points.
struct FPopup
{
	FVector2D Position = FVector2D::ZeroVector;
	FString Text;
	float Life = 0.0f;
};

// GameMode : il gère le menu, la partie, les astéroïdes, les collisions et le score.
UCLASS()
class SPACESHOOTER_API AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShooterGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// On ne crée pas de vaisseau tant que le joueur est dans le menu.
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// Réglages : modifiables dans un Blueprint dérivé de ce GameMode.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Réglages")
	float ShipSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Réglages")
	float FireInterval = 0.16f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Réglages")
	float ProjectileSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Réglages")
	int32 StartingLives = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Réglages")
	float AsteroidMinSpeed = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Réglages")
	float AsteroidMaxSpeed = 320.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Réglages")
	float SpawnDelay = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Réglages")
	FString TeamMembers = TEXT("Réalisé par Natan Arnaud");

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

	// État de la partie, lu par le HUD pour l'affichage.
	EGameState State = EGameState::Menu;
	int32 MenuIndex = 0;
	int32 Score = 0;
	int32 BestScore = 0;
	int32 Lives = 3;
	float GameTime = 0.0f;
	FVector2D ScreenSize = FVector2D(1920.0f, 1080.0f);

	// Secousse de l'écran, appliquée par le HUD au moment de dessiner.
	FVector2D ShakeOffset = FVector2D::ZeroVector;

	UPROPERTY()
	AShipPawn* Ship = nullptr;

	UPROPERTY()
	TArray<AAsteroid*> Asteroids;

	UPROPERTY()
	TArray<AProjectile*> Projectiles;

	TArray<FParticle> Particles;
	TArray<FStar> Stars;
	TArray<FPopup> Popups;

private:
	void SpawnAsteroid();
	void CheckCollisions();
	void AddExplosion(const FVector2D& Position, const FLinearColor& Color, int32 Count);
	void AddPopup(const FVector2D& Position, const FString& Text);
	void AddShake(float Force);
	void ClearGame();
	void LoseLife();

	float SpawnTimer = 0.0f;
	float ShakeTime = 0.0f;
	float ShakeForce = 0.0f;
};
