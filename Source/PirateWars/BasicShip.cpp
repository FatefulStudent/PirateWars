// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BasicShip.h"
#include "StaticFunctions.h"
#include "PaperSpriteComponent.h"

FName ABasicShip::MovementComponentName(TEXT("MovementComponent0"));

void FInputAdapter::Sanitize()
{
	MovementInput = RawMovementInput.ClampAxes(-1.0f, 1.0f);
	MovementInput = MovementInput.GetSafeNormal();
	RawMovementInput.Set(0.0f, 0.0f);
}

void FInputAdapter::MoveX(float AxisValue)
{
	RawMovementInput.X += AxisValue;
}

void FInputAdapter::MoveY(float AxisValue)
{
	RawMovementInput.Y += AxisValue;
}

void FInputAdapter::Fire1(bool bPressed)
{
	bFire1 = bPressed;
}


// Sets default values
ABasicShip::ABasicShip()
{
	bCanBeDamaged = true;
	BaseTurnRate = 45.f;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	BoxShape = FVector(45.0f, 20.0f, 100.0f);

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent >(TEXT("Collision"));
	CollisionComponent->InitCapsuleSize(30.0f, 50.0f);

	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	//CollisionComponent->SetShouldUpdatePhysicsVolume(true); FIXME: weirdly affects movement - getting stuck in islands
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->bDynamicObstacle = false;
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true);

	RootComponent = CollisionComponent;

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UShipMovementComponent>(ABasicShip::MovementComponentName);  //UFloatingPawnMovement UShipMovementComponent
	MovementComponent->UpdatedComponent = CollisionComponent;

	Artillery = CreateDefaultSubobject<UArtilleryComponent>(TEXT("Artillery"));

	ShipSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ShipSprite"));
	ShipSprite->SetupAttachment(RootComponent);
	ShipSprite->SetWorldRotation(FRotator(0.0f, 90.0f, 90.0f));

	// Default values for artillery
	Fire1Cooldown = 1.0f;
	CannonNum = 5;
	RandomStd = 5.0f;
	
	// General
	MovementCollisionProfile = TEXT("BlockAll");
	bIsAlive = 1;
	MaxHealth = 100;
	MoveSpeed = 150.0f;

	CollisionComponent->OnComponentHit.AddDynamic(this, &ABasicShip::OnHit);
}

// Called when the game starts or when spawned
void ABasicShip::BeginPlay()
{
	Super::BeginPlay();	
	CurrentHealth = MaxHealth;

	Artillery->SetCannonNum(CannonNum);
	Artillery->SetRandomStd(RandomStd);
	Artillery->SetCannonNum(Fire1Cooldown);
}

// Called every frame
void ABasicShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InputAdapter.Sanitize();
	MoveAndRotateTheShip(DeltaTime);
	
}

// Checks if the ship needs to be moved and if so rotates and moves it
void ABasicShip::MoveAndRotateTheShip(float DeltaTime)
{
	FVector DesiredMovementDirection = FVector(InputAdapter.MovementInput.X, InputAdapter.MovementInput.Y, 0.0f);
	if (!DesiredMovementDirection.IsNearlyZero())
	{
		RotateTheShip(DesiredMovementDirection.Rotation());
		MoveTheShip(DeltaTime);
	}
}

// Rotate the ships direction, leaving camera untouched
void ABasicShip::RotateTheShip(const FRotator& MovementAngle)
{
	if (MovementAngle.Yaw != RootComponent->GetComponentRotation().Yaw)
	{
		// Turn in one frame
		RootComponent->SetWorldRotation(MovementAngle);
	}
}

// move the ship in the direction it is facing
void ABasicShip::MoveTheShip(float DeltaTime)
{
	FVector SpeedVector = RootComponent->GetForwardVector() * MoveSpeed;
	FVector Pos = GetActorLocation();
	Pos += SpeedVector * DeltaTime;
	SetActorLocation(Pos, true);
}

// Decrease health by damaging the ship (can be overriden)
void ABasicShip::ReceiveDamage(int DamageValue)
{
	if (bIsAlive == false)
		return;

	if (DamageValue >= 0)
		CurrentHealth -= DamageValue;

	UE_LOG(LogTemp, Warning, TEXT("%s: I WAS HIT FOR %d. Remaining Health: %d"), *(GetName()), DamageValue, CurrentHealth)

	// Dynamically swap sprites based on hp left
	if (SpritesForTheShip.Num() > 0)
	{
		int IntervalsNum = SpritesForTheShip.Num() - 1;
		float IntervalLength = float(MaxHealth) / float(IntervalsNum);
		
		for (int IntervalIndex = IntervalsNum; IntervalIndex > 0; IntervalIndex--)
		{
			float ThresholdValue = IntervalLength * (IntervalIndex - 1);
			if (CurrentHealth <= ThresholdValue && CurrentHealth + DamageValue > ThresholdValue)
			{
				ShipSprite->SetSprite(SpritesForTheShip[IntervalsNum - IntervalIndex + 1]);
			}
				
		}
	}

	if (CurrentHealth <= 0)
	{
		Die();
	}
}

// When the health is 0 / below zero stop ticking of the actor
// Disable collisions and disappear after some time
void ABasicShip::Die_Implementation()
{
	bIsAlive = false;
	UE_LOG(LogTemp, Warning, TEXT("%s: I DIEDED"), *(GetName()))
	SetActorTickEnabled(false);
	MovementComponent->Deactivate();
	// CollisionComponent->SetCollisionProfileName("BlockAll");
	
	// Get Out of the way
	FVector Pos = GetActorLocation();
	Pos += FVector(0.0f, 0.0f, -2.0f);
	SetActorLocation(Pos);

	FTimerHandle DummyTimerHandle;
	GetWorldTimerManager().SetTimer(DummyTimerHandle, this, &ABasicShip::Drown, 10.0f);
}

void ABasicShip::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: we were hit by %s!!"), *(GetName()), *(OtherActor->GetName()))
		
		// if we collided with a ship we die
		if (AAbstractShip* OtherShip = Cast<AAbstractShip>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: it was a ship"), *(GetName()))
			if (OtherShip->IsAlive() && this->bIsAlive)
			{
				ReceiveDamage(99999);
				OtherShip->ReceiveDamage(99999);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("gotta be wind"))
	}
}

void ABasicShip::Drown()
{
	Destroy();
}

// Called to bind functionality to input
void ABasicShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABasicShip::MoveX(float AxisValue)
{
	InputAdapter.MoveX(AxisValue);
}

void ABasicShip::MoveY(float AxisValue)
{
	InputAdapter.MoveY(AxisValue);
}

void ABasicShip::Fire1Pressed()
{
	InputAdapter.Fire1(true);
}

void ABasicShip::Fire1Released()
{
	InputAdapter.Fire1(false);
}

void ABasicShip::UpdateNavigationRelevance()
{
	if (CollisionComponent)
	{
		CollisionComponent->SetCanEverAffectNavigation(bCanAffectNavigationGeneration);
	}
}

void ABasicShip::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		if (Controller)
		{
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// transform to world space and add it
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), Val);
		}
	}
}

void ABasicShip::MoveForward(float Val)
{
	if (Val != 0.f)
	{
		if (Controller)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Tring go frwrd"))
			Val *= -1;
			FRotator const ControlSpaceRot = Controller->GetControlRotation();
			/*auto DesiredRotation = Controller->GetDesiredRotation();
			SetActorRotation(DesiredRotation);*/
			// transform to world space and add it
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), Val);
		}
	}
}

void ABasicShip::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

UPawnMovementComponent* ABasicShip::GetMovementComponent() const
{
	return MovementComponent;
}
