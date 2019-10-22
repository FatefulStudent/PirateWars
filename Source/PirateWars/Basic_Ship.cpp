// Fill out your copyright notice in the Description page of Project Settings.

#include "Basic_Ship.h"
#include "StaticFunctions.h"
#include "PaperSpriteComponent.h"

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
ABasic_Ship::ABasic_Ship()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollider;
	BoxCollider->SetGenerateOverlapEvents(true);
	BoxCollider->SetNotifyRigidBodyCollision(true);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	MovementCollisionProfile = TEXT("BlockAll");

	ShipSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ShipSprite"));
	ShipSprite->SetupAttachment(RootComponent);
	ShipSprite->SetWorldRotation(FRotator(0.0f, 90.0f, 90.0f));

	// Default values for speed
	DeathStatus = ALIVE;
	MaxHealth = 100;
	MoveSpeed = 100.0f;
	BoxShape = FVector(45.0f, 20.0f, 100.0f);
}

// Called when the game starts or when spawned
void ABasic_Ship::BeginPlay()
{
	Super::BeginPlay();	
	CurrentHealth = MaxHealth;

	BoxCollider->SetBoxExtent(BoxShape, true);
	BoxCollider->SetCollisionProfileName(MovementCollisionProfile);

	BoxCollider->OnComponentHit.AddDynamic(this, &ABasic_Ship::OnHit);
}

// Called every frame
void ABasic_Ship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InputAdapter.Sanitize();
	MoveAndRotateTheShip(DeltaTime);
	
}

// Checks if the ship needs to be moved and if so rotates and moves it
void ABasic_Ship::MoveAndRotateTheShip(float DeltaTime)
{
	FVector DesiredMovementDirection = FVector(InputAdapter.MovementInput.X, InputAdapter.MovementInput.Y, 0.0f);
	if (!DesiredMovementDirection.IsNearlyZero())
	{
		RotateTheShip(DesiredMovementDirection.Rotation());
		MoveTheShip(DeltaTime);
	}
}

// Rotate the ships direction, leaving camera untouched
void ABasic_Ship::RotateTheShip(const FRotator& MovementAngle)
{
	if (MovementAngle.Yaw != RootComponent->GetComponentRotation().Yaw)
	{
		// Turn in one frame
		RootComponent->SetWorldRotation(MovementAngle);
	}
}

// move the ship in the direction it is facing
void ABasic_Ship::MoveTheShip(float DeltaTime)
{
	FVector SpeedVector = RootComponent->GetForwardVector() * MoveSpeed;
	FVector Pos = GetActorLocation();
	Pos += SpeedVector * DeltaTime;
	SetActorLocation(Pos, true);
}

// Decrease health by damaging the ship (can be overriden)
void ABasic_Ship::ReceiveDamage(int DamageValue)
{
	if (DeathStatus == DEAD)
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

	if (CurrentHealth <= 0 && CurrentHealth + DamageValue > 0)
	{
		Die();
	}
}

// When the health is 0 / below zero stop ticking of the actor
// Disable collisions and disappear after some time
void ABasic_Ship::Die()
{
	DeathStatus = DEAD;
	UE_LOG(LogTemp, Warning, TEXT("%s: I DIEDED"), *(GetName()))
	SetActorTickEnabled(false);
	BoxCollider->SetCollisionProfileName("BlockAll");
	
	// Get Out of the way
	FVector Pos = GetActorLocation();
	Pos += FVector(0.0f, 0.0f, -2.0f);
	SetActorLocation(Pos);

	FTimerHandle DummyTimerHandle;
	GetWorldTimerManager().SetTimer(DummyTimerHandle, this, &ABasic_Ship::Drown, 10.0f);
}

void ABasic_Ship::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: we were hit by %s!!"), *(GetName()), *(OtherActor->GetName()))
		
		// if we collided with a ship we die
		if (AShipInterface* OtherShip = Cast<AShipInterface>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: it was a ship"), *(GetName()))
			DeathStatus = PENDING_KILL;

			ReceiveDamage(99999);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("gotta be wind"))
	}
}

void ABasic_Ship::Drown()
{
	Destroy();
}

// Called to bind functionality to input
void ABasic_Ship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABasic_Ship::MoveX(float AxisValue)
{
	InputAdapter.MoveX(AxisValue);
}

void ABasic_Ship::MoveY(float AxisValue)
{
	InputAdapter.MoveY(AxisValue);
}

void ABasic_Ship::Fire1Pressed()
{
	InputAdapter.Fire1(true);
}

void ABasic_Ship::Fire1Released()
{
	InputAdapter.Fire1(false);
}
