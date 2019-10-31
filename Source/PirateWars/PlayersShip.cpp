// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayersShip.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "StaticFunctions.h"

// Sets default values
APlayersShip::APlayersShip()
{
}

// Called when the game starts or when spawned
void APlayersShip::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayersShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayersShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveX", this, &APlayersShip::MoveRight);
	PlayerInputComponent->BindAxis("MoveY", this, &APlayersShip::MoveForward);

	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Pressed, this, &APlayersShip::Fire1Pressed);
	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Released, this, &APlayersShip::Fire1Released);
}

bool APlayersShip::IsLeftSideFiring() const
{
	bool bFireLeftSide = false;
	FVector2D AimLocation(0.0f, 0.0f);
	if (APlayerController* PC = Cast<APlayerController>(this->GetController()))
	{
		if (PC->GetMousePosition(AimLocation.X, AimLocation.Y))
		{
			FVector2D ArtilleryLocation = FVector2D::ZeroVector;
			UGameplayStatics::ProjectWorldToScreen(PC, GetActorLocation(), ArtilleryLocation);
			FRotator ActorRotation = GetActorRotation();
			bFireLeftSide = UStaticFunctions::TargetedPointIsOnLeftSideOfTheLine(
				ActorRotation.Yaw, ArtilleryLocation, AimLocation
			);
		}
	}
	

	return bFireLeftSide;
}