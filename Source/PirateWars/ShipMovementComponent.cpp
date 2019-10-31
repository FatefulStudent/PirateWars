// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShipMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/WorldSettings.h"

UShipMovementComponent::UShipMovementComponent()
{
}

void UShipMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}

	const AController* Controller = PawnOwner->GetController();
	if (Controller && Controller->IsLocalController())
	{
		// apply input for local players but also for AI that's not following a navigation path at the moment
		if (Controller->IsLocalPlayerController() == true || Controller->IsFollowingAPath() == false || bUseAccelerationForPaths)
		{
			ApplyControlInputToVelocity(DeltaTime);
		}
		// if it's not player controller, but we do have a controller, then it's AI
		// (that's not following a path) and we need to limit the speed
		else if (IsExceedingMaxSpeed(MaxSpeed) == true)
		{
			Velocity = Velocity.GetUnsafeNormal() * MaxSpeed;
		}

		bPositionCorrected = false;
		
		// Finalize
		UpdateComponentVelocity();
	}
};

void UShipMovementComponent::ApplyControlInputToVelocity(float DeltaTime)
{
	const FVector ControlAcceleration = GetPendingInputVector().GetSafeNormal();

	// Update Velocity
	Velocity = ControlAcceleration * GetMaxSpeed();
	ConsumeInputVector();

	// Rotate pawn
	if (!ControlAcceleration.IsNearlyZero())
	{
		const FRotator& MovementAngle = ControlAcceleration.Rotation();
		if (MovementAngle.Yaw != PawnOwner->GetActorRotation().Yaw)
		{
			// Turn in one frame
			PawnOwner->SetActorRotation(MovementAngle);
		}
	}
}