// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "PaperSpriteComponent.h"
//#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ShipBase"));
	}

	ProjectileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CannonSprite"));
	ProjectileSprite->AttachTo(RootComponent);

	// Use this component to drive this projectile's movement.
	//ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	//// ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	//ProjectileMovementComponent->InitialSpeed = 3000.0f;
	//ProjectileMovementComponent->MaxSpeed = 3000.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle DummyTimerHandle;
	GetWorldTimerManager().SetTimer(DummyTimerHandle, this, &AProjectile::Explode, 1.0f);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Loc = GetActorLocation();
	Loc += (DeltaTime * Speed) * GetTransform().GetUnitAxis(EAxis::X);
	SetActorLocation(Loc);
}

void AProjectile::Explode()
{
	Explode_Implementation();
}

void AProjectile::Explode_Implementation()
{
	Destroy();
}