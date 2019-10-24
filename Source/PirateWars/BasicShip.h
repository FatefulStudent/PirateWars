// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "ArtilleryComponent.h"
#include "PaperSprite.h"
#include "AbstractShip.h"
#include "BasicShip.generated.h"

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
class PIRATEWARS_API ABasicShip : public AAbstractShip
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasicShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	// Whether ship wants to shoot or not
	FORCEINLINE bool Fire1Enabled() const override { return InputAdapter.bFire1; }
	
	// Whether ship is alive or pending death
	UFUNCTION(BlueprintCallable, Category = "Ship")
	FORCEINLINE DEATH_STATUS GetDeathStatus() const override { return DeathStatus; }

	// Gets health in percents (for the HUD/UI)
	UFUNCTION(BlueprintCallable, Category = "Ship")
	FORCEINLINE float GetHealthPct() const override { return (CurrentHealth > 0 ? float(CurrentHealth)/float(MaxHealth) : 0.0f); }

	// Public method to damage the ship
	UFUNCTION(BlueprintCallable, Category = "Ship")
	void ReceiveDamage(int DamageValue) override;

	FORCEINLINE bool IsLeftSideFiring() const override { return true; }

protected:
	/*
	 * FUNCTIONS
	 */
	virtual void MoveX(float AxisValue);
	virtual void MoveY(float AxisValue);
	void MoveAndRotateTheShip(float DeltaTime);
	void RotateTheShip(const FRotator& MovementAngle);
	void MoveTheShip(float DeltaTime);
	void Fire1Pressed();
	void Fire1Released();

	virtual void Die();
	void Drown();

	UFUNCTION(BlueprintCallable, Category = "Ship")
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	/*
	 * PROPERTIES 
	 */
	DEATH_STATUS DeathStatus;

	// Artillery to shoot with cannons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship", meta = (AllowPrivateAccess = "true"))
	UArtilleryComponent* Artillery;

	// Time to delay between Fire1 commands.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	float Fire1Cooldown;

	// Number of cannons for each side
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	int CannonNum;

	// Randomness for cannons
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	float RandomStd;

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
