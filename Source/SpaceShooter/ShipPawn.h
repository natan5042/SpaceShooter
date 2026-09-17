#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShipPawn.generated.h"

// Vaisseau du joueur.
UCLASS()
class SPACESHOOTER_API AShipPawn : public APawn
{
	GENERATED_BODY()

public:
	AShipPawn();

	virtual void Tick(float DeltaTime) override;

	// Déplace le vaisseau et le garde dans l'écran.
	void Move(const FVector2D& Direction, const FVector2D& ScreenSize, float DeltaTime);

	// Renvoie true si le vaisseau peut tirer, et relance le délai.
	bool CanFire();

	FVector2D Position = FVector2D::ZeroVector;
	float Speed = 600.0f;
	float FireInterval = 0.16f;
	float FireTimer = 0.0f;
	float InvulnerabilityTime = 0.0f;

	// Inclinaison du vaisseau, entre -1 (à gauche) et 1 (à droite).
	// Elle suit l'entrée du joueur avec un peu de retard, ce qui donne
	// l'impression que le vaisseau penche dans ses virages.
	float Lean = 0.0f;
};
