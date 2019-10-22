// Fill out your copyright notice in the Description page of Project Settings.


#include "Artillery.h"
#include "Engine/EngineTypes.h"

// Sets default values for this component's properties
UArtillery::UArtillery()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Default values for number of cannons and cooldown of guns
	CannonNum = 5;
	Fire1Cooldown = 1.0f;
	RandomStd = 5.0f;
}


// Called when the game starts
void UArtillery::BeginPlay()
{
	Super::BeginPlay();

	// TODO: if it is not a ship
	ShipOwner = Cast<AShipInterface>(this->GetAttachmentRootActor());
	CreateArtillery();
}

// Creating Artillery on left and right side of the ship
void UArtillery::CreateArtillery()
{
	UE_LOG(LogTemp, Warning, TEXT("CREATING ARTILLERY"))
	// TODO: transfer to properties
	float maxHeight = 30.0f;
	float minHeight = -40.0f;
	float fixedWidth = 15.0f;
	if (UWorld* World = GetWorld())
	{
		FVector Loc = ShipOwner->GetActorLocation();
		FRotator Rot = ShipOwner->GetActorRotation();

		float DeltaBetweenCannons = 0.0f;
		if (CannonNum > 1)
			DeltaBetweenCannons = (maxHeight - minHeight) / (CannonNum - 1);
			
		// Iterating from left to right side
		for (int bLeftSide = 0; bLeftSide < 2; bLeftSide++)
		{
			// Creating CannonNum cannons on each side
			for (int i = 0; i < CannonNum; i++)
			{
				// Calculating appropriate location
				float XDelta = (bLeftSide ? -1.0f * fixedWidth : fixedWidth);
				float YDelta = minHeight + i * DeltaBetweenCannons;
				FVector NewLoc = Loc + FVector(YDelta, XDelta, -2.0f);

				FRotator NewRot = Rot;
				if (bLeftSide == 0)
					NewRot = NewRot.Add(0.0f, 180.0f, 0.0f);
				
				FActorSpawnParameters SpawnParameters;
				if (ACannon* NewCannon = World->SpawnActor<ACannon>(CannonType, NewLoc, NewRot, SpawnParameters))
				{
					NewCannon->bIsLeftSide = bLeftSide;
					NewCannon->RandomStd = RandomStd;
					NewCannon->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
					CannonArr.Add(NewCannon);
				}
			}

		}
		

	}

}

// Called every frame
void UArtillery::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShipOwner != nullptr && ShipOwner->GetDeathStatus() == ALIVE && CannonType)
	{
		// Handle input.
		bool bFire1 = ShipOwner->Fire1Enabled();
		
		if (bFire1 && CannonArr.Num() != 0 && CannonArr[0]->ProjectileIsPresent())
		{
			if (UWorld* World = GetWorld())
			{
				float CurrentTime = World->GetTimeSeconds();
				if (Fire1ReadyTime <= CurrentTime)
				{
					
					bool bFireLeftSide = ShipOwner->IsLeftSideFiring();
					// Firing all cannons on the appropriate side
					for (int i = 0; i < CannonArr.Num(); i++)
					{
						if (CannonArr[i]->bIsLeftSide == bFireLeftSide)
							CannonArr[i]->Fire(World);
					}
					// Set the cooldown timer.
					Fire1ReadyTime = CurrentTime + Fire1Cooldown;
					
				}
			}
		}
	}
}

// Clears up after deletion
void UArtillery::OnUnregister()
{
	Super::OnUnregister();
	
	for (int i = 0; i < CannonArr.Num(); i++)
	{
		CannonArr[i]->Destroy();
	}
}