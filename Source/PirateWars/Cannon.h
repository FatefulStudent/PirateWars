// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "Basic_Ship.h"
#include "Cannon.generated.h"

UCLASS()
class PIRATEWARS_API ACannon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACannon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Projectile to spawn when firing.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret")
	TSubclassOf<AActor> Projectile;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UArrowComponent* GetCannonDirection() { return CannonDirection; }

	/** The name of the socket at the muzzle - used for spawning missiles. */
	static const FName MuzzleSocketName;

private:
	// Helpful debug tool - which way is the cannon facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* CannonDirection;

	// Sprite for the cannon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* CannonSprite;

	// Tank that owns this turret.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (AllowPrivateAccess = "true"))
	ABasic_Ship* Ship;

	// Maximum rate at which the turret can turn to aim.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (AllowPrivateAccess = "true"))
	float YawSpeed;

	// Time to delay between Fire1 commands.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (AllowPrivateAccess = "true"))
	float Fire1Cooldown;

	// If this value is greater than the current game time, Fire1 is ignored because it has been fired too recently.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (AllowPrivateAccess = "true"))
	float Fire1ReadyTime;
};
