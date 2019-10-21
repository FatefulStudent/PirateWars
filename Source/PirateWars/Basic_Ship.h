// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PaperSprite.h"
#include "Components/BoxComponent.h"
#include "Basic_Ship.generated.h"

USTRUCT(BlueprintType)
struct FInputAdapter
{
	GENERATED_BODY();

public:
	// Sanitized movement input
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Adapter")
	FVector2D MovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	uint32 bFire1 : 1;

	void Sanitize();
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void Fire1(bool bPressed);

private:
	// Raw data. Game code should never see it
	FVector2D RawMovementInput;
};

UCLASS(abstract)
class PIRATEWARS_API ABasic_Ship : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasic_Ship();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	virtual void MoveX(float AxisValue);
	virtual void MoveY(float AxisValue);
	void MoveAndRotateTheShip(float DeltaTime);
	void RotateTheShip(const FRotator& MovementAngle);
	void MoveTheShip(float DeltaTime);
	void Fire1Pressed();
	void Fire1Released();

	bool bShipIsDead;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Helpful debug tool - which way is the ship facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ShipDirection;

	FORCEINLINE UArrowComponent* GetShipDirectionArrow() const { return ShipDirection; }
	FORCEINLINE float GetShipYaw() const { return ShipDirection->GetComponentRotation().Yaw; }
	FORCEINLINE const FInputAdapter& GetCurrentInput() const { return InputAdapter; }
	FORCEINLINE FVector GetRootComponentLocation() const { return RootComponent->GetComponentLocation(); }
	FORCEINLINE FRotator GetRootComponentRotation() const { return RootComponent->GetComponentRotation(); }
	FORCEINLINE int GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Ship")
	virtual void RecieveDamage(int DamageValue);

	UFUNCTION(BlueprintCallable, Category = "Ship")
	virtual void Die();

	void Drown();
	virtual void DrownImplementation();

protected:
	// Sprite for the ship body.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* ShipSprite;

	// Array of availible sprites 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
	TArray<UPaperSprite*> SpritesForTheShip;

	// Box for collision calculations
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollider;

	// Box collider shape
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
	FVector BoxShape;

	// Describes collision behaviour
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
	FName MovementCollisionProfile;

	// Current input for our ship. Sanitized in Tick().
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
	FInputAdapter InputAdapter;

	// Top speed for our ship going forward. Ship's velocity will be clamped to this magnitude.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship", meta = (ClampMin = "0.0"))
	float MoveSpeed;

	// Max health of the ship
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship", meta = (ClampMin = "0.0"))
	int MaxHealth;

	// Current health of the ship
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship", meta = (ClampMin = "0.0"))
	int CurrentHealth;

	
};
