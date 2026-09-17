#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

// Lecture des touches du clavier.
//   Flèches      : déplacer le vaisseau
//   Barre espace : tirer
//   Entrée       : valider dans le menu
//   Échap        : revenir au menu / quitter
UCLASS()
class SPACESHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterPlayerController();

	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
};
