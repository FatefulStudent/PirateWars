// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"
#include "StaticFunctions.h"

const FName ACannon::MuzzleSocketName(TEXT("Muzzle"));

// Sets default values
ACannon::ACannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CannonDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("CannonDirection"));

	CannonSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CannonSprite"));
	CannonSprite->SetupAttachment(CannonDirection);
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

	float RandomYawAddition = UStaticFunctions::GetRandomNormal(0.0f, this->RandomStd);
	UE_LOG(LogTemp, Warning, TEXT("ADDING %f to YAW"), RandomYawAddition)
	if (AActor* NewProjectile = World->SpawnActor(Projectile))
	{
		NewProjectile->SetActorLocation(Loc);
		NewProjectile->SetActorRotation(Rot.Add(0.0f, RandomYawAddition, 0.0f));
	}
}

