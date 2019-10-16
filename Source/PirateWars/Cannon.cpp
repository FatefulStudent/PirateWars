// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"
#include "Components/ArrowComponent.h"

const FName ACannon::MuzzleSocketName(TEXT("Muzzle"));

// Sets default values
ACannon::ACannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CannonDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("CannonDirection"));

	CannonSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CannonSprite"));
	CannonSprite->AttachTo(CannonDirection);

	Fire1Cooldown = 1.0f;
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	Ship = Cast<ABasic_Ship>(GetParentComponent()->GetOwner());
	check(Ship);
}

// Called every frame
void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	check(CannonDirection);
	if (Ship != nullptr)
	{
		// Handle input.
		const FInputAdapter& CurrentInput = Ship->GetCurrentInput();
		if (CurrentInput.bFire1 && Projectile)
		{
			if (UWorld* World = GetWorld())
			{
				float CurrentTime = World->GetTimeSeconds();
				if (Fire1ReadyTime <= CurrentTime)
				{
					FVector Loc = CannonSprite->GetSocketLocation(MuzzleSocketName);
					FRotator Rot = CannonDirection->GetComponentRotation();

					if (AActor* NewProjectile = World->SpawnActor(Projectile))
					{
						NewProjectile->SetActorLocation(Loc);
						NewProjectile->SetActorRotation(Rot);
					}

					// Set the cooldown timer.
					Fire1ReadyTime = CurrentTime + Fire1Cooldown;
				}
			}
		}
	}

}

