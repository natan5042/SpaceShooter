#include "Asteroid.h"
#include "Components/SceneComponent.h"

AAsteroid::AAsteroid()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	SetActorEnableCollision(false);
}

void AAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Position += Velocity * DeltaTime;
	Angle += Spin * DeltaTime;

	if (HitFlash > 0.0f)
	{
		HitFlash -= DeltaTime * 4.0f;
	}
}

bool AAsteroid::IsOutOfScreen(const FVector2D& ScreenSize) const
{
	float Margin = Radius + 150.0f;

	return Position.X < -Margin
		|| Position.X > ScreenSize.X + Margin
		|| Position.Y < -Margin
		|| Position.Y > ScreenSize.Y + Margin;
}
