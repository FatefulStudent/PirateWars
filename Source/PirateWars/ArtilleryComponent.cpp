// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtilleryComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/EngineTypes.h"

// Sets default values for this component's properties
UArtilleryComponent::UArtilleryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CannonNum = 1;
	Fire1Cooldown = 1.0f;
	RandomStd = 1.0f;
}

// Called when the game starts
void UArtilleryComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO: if it is not a ship
	ShipOwner = Cast<AAbstractShip>(this->GetAttachmentRootActor());
	CreateArtillery();
}

// Creating Artillery on left and right side of the ship
void UArtilleryComponent::CreateArtillery()
{
	UE_LOG(LogTemp, Warning, TEXT("CREATING ARTILLERY"))
	// TODO: transfer to properties
	float maxHeight = 30.0f;
	float minHeight = -40.0f;
	float fixedWidth = 15.0f;
	if (UWorld* World = GetWorld())
	{
		FVector Loc = FVector(0, 0, 0);
		FRotator Rot = FRotator(0, 0, 0);

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
				float XDelta = (bLeftSide ? -1.0f * fixedWidth: fixedWidth);
				float YDelta = minHeight + i * DeltaBetweenCannons;
				float ZDelta = -2.0f;

				FVector NewLoc = FVector(YDelta, XDelta, ZDelta);

				FRotator NewRot;
				if (bLeftSide == 0)
					NewRot = FRotator(0.0f, 90.0f, 0.0f);
				else
					NewRot = FRotator(0.0f, -90.0f, 0.0f);

				FActorSpawnParameters SpawnParameters;
				if (ACannon* NewCannon = World->SpawnActor<ACannon>(CannonType, NewLoc, NewRot, SpawnParameters))
				{
					NewCannon->bIsLeftSide = bLeftSide;
					NewCannon->RandomStd = RandomStd;
					NewCannon->AttachToActor(Cast<AActor>(ShipOwner), FAttachmentTransformRules::KeepRelativeTransform);
					CannonArr.Add(NewCannon); 
				}
			}

		}
		

	}

}

// Called every frame
void UArtilleryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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
				float CurrentTime = GetCurrentTime();
				if (Fire1ReadyTime <= CurrentTime)
				{
					FireAllCannons(World, CurrentTime);
				}
			}
			
		}
	}
}

void UArtilleryComponent::FireAllCannons(UWorld* World, float CurrentTime)
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

float UArtilleryComponent::GetFire1CooldownPct() const
{
	float CurrentTime = GetCurrentTime();

	if (CurrentTime < Fire1ReadyTime)
		return 1.0f - (Fire1ReadyTime - CurrentTime) / Fire1Cooldown;
	else
		return 1.0f; 
}

float UArtilleryComponent::GetCurrentTime() const
{
	float CurrentTime = 0.0f;
	if (UWorld* World = GetWorld())
	{
		CurrentTime = World->GetTimeSeconds();
	}
	return CurrentTime;
}

// Clears up after deletion
void UArtilleryComponent::OnUnregister()
{
	Super::OnUnregister();
	
	for (int i = 0; i < CannonArr.Num(); i++)
	{
		CannonArr[i]->Destroy();
	}
}