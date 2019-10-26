// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "Components/ArrowComponent.h"
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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Spread when shooting
	float RandomStd;

	FORCEINLINE UArrowComponent* GetCannonDirection() const { return CannonDirection; }
	FORCEINLINE bool ProjectileIsPresent() const { return Projectile; }
	FORCEINLINE void SetProjectile(TSubclassOf<AActor> ProjectileType) { this->Projectile = ProjectileType; }
	FORCEINLINE void SetLeftSide(bool bOnLeftSide) { this->bIsLeftSide = bOnLeftSide; }
	FORCEINLINE bool IsOnLeftSide() { return bIsLeftSide; }
	
	void Fire(UWorld* World);
	
private:
	/** The name of the socket at the muzzle - used for spawning missiles. */
	static const FName MuzzleSocketName;

	// Helpful debug tool - which way is the cannon facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* CannonDirection;

	// Sprite for the cannon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* CannonSprite;

	// Projectile to spawn when firing.
	TSubclassOf<AActor> Projectile;

	// Is this cannon on the left side?
	bool bIsLeftSide;
};
