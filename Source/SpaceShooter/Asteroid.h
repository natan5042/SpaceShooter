#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Asteroid.generated.h"

// Astéroïde : il apparaît sur un bord de l'écran et traverse le terrain en tournant.
UCLASS()
class SPACESHOOTER_API AAsteroid : public AActor
{
	GENERATED_BODY()

public:
	AAsteroid();

	virtual void Tick(float DeltaTime) override;

	// True quand l'astéroïde est sorti de l'écran (on peut le supprimer).
	bool IsOutOfScreen(const FVector2D& ScreenSize) const;

	FVector2D Position = FVector2D::ZeroVector;
	FVector2D Velocity = FVector2D::ZeroVector;
	float Radius = 40.0f;

	// Nombre de tirs restants avant la destruction.
	int32 Life = 2;

	// Rotation : Angle tourne à la vitesse Spin, ce qui fait bouger les bosses
	// et les cratères dessinés par le HUD.
	float Angle = 0.0f;
	float Spin = 0.0f;

	// Passe à 1 quand un tir touche, puis redescend : l'astéroïde blanchit un instant.
	float HitFlash = 0.0f;
};
