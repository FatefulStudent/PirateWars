// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractShip.generated.h"

UENUM()
enum DEATH_STATUS { ALIVE, PENDING_KILL, DEAD };

UCLASS(abstract)
class PIRATEWARS_API AAbstractShip : public APawn
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Whether ship wants to shoot or not
	FORCEINLINE virtual bool Fire1Enabled() const { return true; }

	// Whether ship is alive or pending death
	FORCEINLINE virtual DEATH_STATUS GetDeathStatus() const { return DEAD; }

	// Gets health in procents (for the HUD/UI)
	FORCEINLINE virtual float GetHealthPct() const { return 0.0f; }

	// Public method to damage the ship
	FORCEINLINE virtual void ReceiveDamage(int DamageValue) {}

	// Get Aim Location
	FORCEINLINE virtual bool IsLeftSideFiring() const { return true; }
};
