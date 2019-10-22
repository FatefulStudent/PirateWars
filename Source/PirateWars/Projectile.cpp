// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "PaperSpriteComponent.h"
#include "ShipInterface.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	RootComponent = CapsuleCollider;
	CapsuleCollider->SetGenerateOverlapEvents(true);
	CapsuleCollider->SetNotifyRigidBodyCollision(true);
	
	ProjectileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CannonSprite"));
	ProjectileSprite->SetupAttachment(RootComponent);

	DamageValue = 10;
	Radius = 6.0f;
	Speed = 300.0f;
	FlightTime = 1.0f;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		AShipInterface* HitShip = Cast<AShipInterface>(OtherActor);
		if (HitShip != nullptr)
		{
			HitShip->ReceiveDamage(DamageValue);
			Explode();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PROJECTILE HIT %s"), *(OtherActor->GetName()))
			Explode();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("gotta be wind"))
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	CapsuleCollider->SetCapsuleSize(Radius, 200.0f);
	CapsuleCollider->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	
	FTimerHandle DummyTimerHandle;
	GetWorldTimerManager().SetTimer(DummyTimerHandle, this, &AProjectile::Explode, FlightTime);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Loc = GetActorLocation();
	FVector DesiredLoc = Loc + (DeltaTime * Speed) * GetTransform().GetUnitAxis(EAxis::X);
	SetActorLocation(DesiredLoc, true);
}

void AProjectile::Explode()
{
	Destroy();
}