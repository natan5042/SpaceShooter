#include "ShipPawn.h"
#include "Components/SceneComponent.h"

AShipPawn::AShipPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	SetActorEnableCollision(false);
}

void AShipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FireTimer > 0.0f)
	{
		FireTimer -= DeltaTime;
	}

	if (InvulnerabilityTime > 0.0f)
	{
		InvulnerabilityTime -= DeltaTime;
	}
}

void AShipPawn::Move(const FVector2D& Direction, const FVector2D& ScreenSize, float DeltaTime)
{
	Position += Direction * Speed * DeltaTime;

	// Le vaisseau ne doit pas sortir de l'écran.
	Position.X = FMath::Clamp(Position.X, 30.0f, ScreenSize.X - 30.0f);
	Position.Y = FMath::Clamp(Position.Y, 40.0f, ScreenSize.Y - 40.0f);

	// Le vaisseau penche du côté où il se déplace.
	Lean = FMath::FInterpTo(Lean, Direction.X, DeltaTime, 9.0f);
}

bool AShipPawn::CanFire()
{
	if (FireTimer > 0.0f)
	{
		return false;
	}

	FireTimer = FireInterval;
	return true;
}
