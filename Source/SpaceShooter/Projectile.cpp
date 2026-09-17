#include "Projectile.h"
#include "Components/SceneComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	SetActorEnableCollision(false);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Position.Y -= Speed * DeltaTime;
}
