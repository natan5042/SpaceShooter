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

	// Deplace le vaisseau et le garde dans l'ecran.
	void Move(const FVector2D& Direction, const FVector2D& ScreenSize, float DeltaTime);

	// Renvoie true si le vaisseau peut tirer, et relance le delai.
	bool CanFire();

	FVector2D Position = FVector2D::ZeroVector;
	float Speed = 600.0f;
	float FireInterval = 0.16f;
	float FireTimer = 0.0f;
	float InvulnerabilityTime = 0.0f;
};
