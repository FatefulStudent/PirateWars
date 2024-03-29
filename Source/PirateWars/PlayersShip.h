// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicShip.h"
#include "Camera/CameraComponent.h"
#include "PlayersShip.generated.h"

/**
 * 
 */
UCLASS()
class PIRATEWARS_API APlayersShip : public ABasicShip
{
	GENERATED_BODY()
	
public:
	// Sets default values for this pawn's properties
	APlayersShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Gets Cooldown in percents (for the HUD/UI)
	UFUNCTION(BlueprintCallable, Category = "Ship")
	FORCEINLINE float GetFire1CooldownPct() const { return (bIsAlive ? Artillery->GetFire1CooldownPct() : 0.0f); }

	bool IsLeftSideFiring() const override;
};
