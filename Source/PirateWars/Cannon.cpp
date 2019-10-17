// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"
#include "Basic_Ship.h"

const FName ACannon::MuzzleSocketName(TEXT("Muzzle"));

// Sets default values
ACannon::ACannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CannonDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("CannonDirection"));

	CannonSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CannonSprite"));
	CannonSprite->AttachTo(CannonDirection);
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACannon::Fire(UWorld* World)
{
	check(CannonDirection);
	FVector Loc = CannonSprite->GetSocketLocation(MuzzleSocketName);
	FRotator Rot = CannonDirection->GetComponentRotation();

	if (AActor* NewProjectile = World->SpawnActor(Projectile))
	{
		NewProjectile->SetActorLocation(Loc);
		NewProjectile->SetActorRotation(Rot);
	}
}

