// Fill out your copyright notice in the Description page of Project Settings.


#include "Artillery.h"
#include "Basic_Ship.h"
#include "Kismet/GameplayStatics.h"
#include "StaticFunctions.h"
#include "Engine/EngineTypes.h"

// Sets default values for this component's properties
UArtillery::UArtillery()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Fire1Cooldown = 1.0f;
}


// Called when the game starts
void UArtillery::BeginPlay()
{
	Super::BeginPlay();

	ShipOwner = Cast<ABasic_Ship>(this->GetAttachmentRootActor());
	CreateArtillery();
}

// Creating Artillery on left and right side of the ship
void UArtillery::CreateArtillery()
{
	UE_LOG(LogTemp, Warning, TEXT("CREATING ARTILLERY"))
	float maxHeight = 30.0f;
	float minHeight = -40.0f;
	float fixedWidth = 15.0f;
	if (UWorld* World = GetWorld())
	{
		FVector Loc = ShipOwner->GetRootComponentLocation();
		FRotator Rot = ShipOwner->GetRootComponentRotation();

		float DeltaBetweenCannons = 0.0f;
		if (ShipOwner->GetCannonNum() > 1)
			DeltaBetweenCannons = (maxHeight - minHeight) / (ShipOwner->GetCannonNum() - 1);
			

		for (int sides = 0; sides < 2; sides++)
		{
			// Artillery
			for (int i = 0; i < ShipOwner->GetCannonNum(); i++)
			{
				float XDelta = (sides ? -1.0f * fixedWidth : fixedWidth);
				float YDelta = minHeight + i * DeltaBetweenCannons;
				FVector NewLoc = Loc + FVector(YDelta, XDelta, -2.0f);

				FRotator NewRot = Rot;
				if (sides == 0)
					NewRot = NewRot.Add(0.0f, 90.0f, 0.0f);
				else
					NewRot = NewRot.Add(0.0f, -90.0f, 0.0f);

				if (ACannon* NewCannon = World->SpawnActor<ACannon>(CannonType, NewLoc, NewRot))
				{
					NewCannon->bIsLeftSide = sides;
					NewCannon->AttachToComponent(ShipOwner->GetShipDirectionArrow(), FAttachmentTransformRules::KeepRelativeTransform);
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

	if (ShipOwner != nullptr && CannonType)
	{
		// Handle input.
		const FInputAdapter& CurrentInput = ShipOwner->GetCurrentInput();
		
		if (CurrentInput.bFire1 && CannonArr.Num() != 0 && CannonArr[0]->ProjectileIsPresent())
		{
			if (UWorld* World = GetWorld())
			{
				float CurrentTime = World->GetTimeSeconds();
				if (Fire1ReadyTime <= CurrentTime)
				{
					if (APlayerController* PC = Cast<APlayerController>(ShipOwner->GetController()))
					{
						FVector2D AimLocation;
						if (PC->GetMousePosition(AimLocation.X, AimLocation.Y))
						{
							FVector2D ArtilleryLocation = FVector2D::ZeroVector;
							UGameplayStatics::ProjectWorldToScreen(PC, GetComponentLocation(), ArtilleryLocation);
							UE_LOG(LogTemp, Warning, TEXT("YAW:%f, ArtilleryLoc: (%f, %f), AimLoc (%f, %f)"), ShipOwner->GetShipYaw(), ArtilleryLocation.X, ArtilleryLocation.Y, AimLocation.X, AimLocation.Y)
							bool bFireLeftSide = UStaticFunctions::TargetedPointIsOnLeftSideOfTheLine(ShipOwner->GetShipYaw(), ArtilleryLocation, AimLocation);
							if (bFireLeftSide)
								UE_LOG(LogTemp, Warning, TEXT("FIRING LEFT!"))
							else
								UE_LOG(LogTemp, Warning, TEXT("FIRING RIGHT!"))
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
	}
}

