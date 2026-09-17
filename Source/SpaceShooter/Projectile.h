#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

// Tir du vaisseau : il monte tout droit vers le haut de l'ecran.
UCLASS()
class SPACESHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

	virtual void Tick(float DeltaTime) override;

	FVector2D Position = FVector2D::ZeroVector;
	float Speed = 1200.0f;
};
