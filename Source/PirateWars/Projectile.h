// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class PIRATEWARS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Speed;

private:
	// Sprite for the Cannon Ball
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* ProjectileSprite;

	//// Projectile movement component.
	//UPROPERTY(VisibleAnywhere, Category = "Projectile")
	//class UProjectileMovementComponent* ProjectileMovementComponent;

	//// Function that initializes the projectile's velocity in the shoot direction.
	//void FireInDirection(const FVector& ShootDirection);

protected:
	void Explode();
	virtual void Explode_Implementation();
};
