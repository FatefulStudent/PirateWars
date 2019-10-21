// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayersShip.h"

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
	PlayerInputComponent->BindAxis("MoveX", this, &APlayersShip::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &APlayersShip::MoveY);

	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Pressed, this, &APlayersShip::Fire1Pressed);
	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Released, this, &APlayersShip::Fire1Released);
}