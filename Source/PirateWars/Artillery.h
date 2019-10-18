// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Cannon.h"
#include "Artillery.generated.h"


UCLASS( ClassGroup="Artillery", meta=(BlueprintSpawnableComponent) )
class PIRATEWARS_API UArtillery : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArtillery();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//FORCEINLINE ABasic_Ship* GetShipOwner() { return ShipOwner; }

private:
	// Ship Owner of the Artillery
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	class ABasic_Ship* ShipOwner;

	// Projectile to spawn when firing.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> CannonType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	TArray<ACannon*> CannonArr;

	// Time to delay between Fire1 commands.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	float Fire1Cooldown;

	// If this value is greater than the current game time, Fire1 is ignored because it has been fired too recently.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	float Fire1ReadyTime;

	// Number of cannons for each side
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	int CannonNum;

	// Randomness for cannons
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Artillery", meta = (AllowPrivateAccess = "true"))
	float RandomStd;
	
	void CreateArtillery();
};
