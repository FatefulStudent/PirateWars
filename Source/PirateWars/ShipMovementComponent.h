// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

/**
* Movement component updates position of associated PrimitiveComponent during its tick
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "ShipMovementComponent.generated.h"

/**
* FloatingPawnMovement is a movement component that provides simple movement for any Pawn class.
* Limits on speed and acceleration are provided, while gravity is not implemented.
*
* Normally the root component of the owning actor is moved, however another component may be selected (see SetUpdatedComponent()).
* During swept (non-teleporting) movement only collision of UpdatedComponent is considered, attached components will teleport to the end location ignoring collision.
*/
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class PIRATEWARS_API UShipMovementComponent : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:
	UShipMovementComponent();

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface

	/** Update Velocity based on input. Also applies gravity. */
	virtual void ApplyControlInputToVelocity(float DeltaTime) override;


};

