// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicShip.h"
#include "EnemyShip.generated.h"

/**
 * 
 */
UCLASS()
class PIRATEWARS_API AEnemyShip : public ABasicShip
{
	GENERATED_BODY()

public:
	AEnemyShip();
	// Whether ship wants to shoot or not
	FORCEINLINE bool Fire1Enabled() const override { return true; }
	
};
