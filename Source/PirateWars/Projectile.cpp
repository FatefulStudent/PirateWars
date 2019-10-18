// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "PaperSpriteComponent.h"


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
	ProjectileSprite->SetupAttachment(RootComponent);

	Radius = 6.0f;
	Speed = 300.0f;
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
	FVector DesiredLoc = Loc + (DeltaTime * Speed) * GetTransform().GetUnitAxis(EAxis::X);
	
	if (UWorld* World = GetWorld())
	{
		FHitResult OutHit;
		FCollisionShape CollisionShape;
		CollisionShape.SetCapsule(Radius, 200.0f);
		if (World->SweepSingleByProfile(OutHit, Loc, DesiredLoc, FQuat::Identity, MovementCollisionProfile, CollisionShape))
		{
			UE_LOG(LogTemp, Warning, TEXT("PROJECTILE HIT %s"), *(OutHit.Actor.Get()->GetName()))
			SetActorLocation(OutHit.Location);
			Explode();
		}
		else
		{
			SetActorLocation(DesiredLoc);
		}
	}
	
}

void AProjectile::Explode()
{
	Explode_Implementation();
}

void AProjectile::Explode_Implementation()
{
	Destroy();
}