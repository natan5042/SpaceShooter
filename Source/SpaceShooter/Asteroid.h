#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Asteroid.generated.h"

// Asteroide : il apparait sur un bord de l'ecran et traverse le terrain.
UCLASS()
class SPACESHOOTER_API AAsteroid : public AActor
{
	GENERATED_BODY()

public:
	AAsteroid();

	virtual void Tick(float DeltaTime) override;

	// True quand l'asteroide est sorti de l'ecran (on peut le supprimer).
	bool IsOutOfScreen(const FVector2D& ScreenSize) const;

	FVector2D Position = FVector2D::ZeroVector;
	FVector2D Velocity = FVector2D::ZeroVector;
	float Radius = 40.0f;

	// Nombre de tirs restants avant la destruction.
	int32 Life = 2;
};
