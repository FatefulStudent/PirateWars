// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractShip.generated.h"


UCLASS(abstract)
class PIRATEWARS_API AAbstractShip : public APawn
{
	GENERATED_BODY()

public:
	// Whether ship wants to shoot or not
	FORCEINLINE virtual bool Fire1Enabled() const PURE_VIRTUAL(AAbstractShip::Fire1Enabled, return false;);

	// Whether ship is alive or pending death
	FORCEINLINE virtual bool IsAlive() const PURE_VIRTUAL(AAbstractShip::IsAlive, return false;);

	// Gets health in procents (for the HUD/UI)
	FORCEINLINE virtual float GetHealthPct() const PURE_VIRTUAL(AAbstractShip::GetHealthPct, return 0.0f;);

	// Public method to damage the ship
	FORCEINLINE virtual void ReceiveDamage(int DamageValue) PURE_VIRTUAL(AAbstractShip::ReceiveDamage, ;);

	// Get Aim Location
	FORCEINLINE virtual bool IsLeftSideFiring() const PURE_VIRTUAL(AAbstractShip::IsLeftSideFiring, return false;);
};
