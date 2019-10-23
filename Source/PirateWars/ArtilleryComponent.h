// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AbstractShip.h"
#include "Cannon.h"
#include "ArtilleryComponent.generated.h"


UCLASS( ClassGroup="ArtilleryComponent", meta=(BlueprintSpawnableComponent) )
class PIRATEWARS_API UArtilleryComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArtilleryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE void SetCannonNum(int num) { CannonNum = num; }
	FORCEINLINE void SetRandomStd(float value) { RandomStd = value; }
	FORCEINLINE void SetFire1Cooldown(float value) { Fire1Cooldown = value; }

	float GetFire1CooldownPct() const;

private:
	// Ship Owner of the Artillery
	class AAbstractShip* ShipOwner;

	// Projectile to spawn when firing.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> CannonType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	TArray<ACannon*> CannonArr;

	// Time to delay between Fire1 commands.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	float Fire1Cooldown;

	// Number of cannons for each side
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	int CannonNum;

	// Randomness for cannons
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	float RandomStd;

	// If this value is greater than the current game time, Fire1 is ignored because it has been fired too recently.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	float Fire1ReadyTime;
	
	UFUNCTION()
	void CreateArtillery();
	
	UFUNCTION()
	void OnUnregister() override;

	float GetCurrentTime() const;
	void FireAllCannons(UWorld* World, float CurrentTime);
};
