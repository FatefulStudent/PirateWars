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
	UE_LOG(LogTemp, Warning, TEXT("FIRE1_CHANGED"))
	bFire1 = bPressed;
}


// Sets default values
ABasic_Ship::ABasic_Ship()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ShipBase"));
	}

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ShipDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ShipDirection"));
	ShipDirection->AttachTo(RootComponent);

	ShipSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ShipSprite"));
	ShipSprite->AttachTo(ShipDirection);

	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->CameraLagSpeed = 2.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->AttachTo(RootComponent);
	SpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->AspectRatio = 4.0f / 3.0f;
	CameraComponent->AttachTo(SpringArm, USpringArmComponent::SocketName);
	CameraComponent->SetWorldRotation(FRotator(-90.0f, -90.0f, 0.0f));

	Cannon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Cannon"));
	Cannon->AttachTo(ShipDirection);

	// Default values for speed, acceleration and rotation
	MoveSpeed = 100.0f;
}

// Called when the game starts or when spawned
void ABasic_Ship::BeginPlay()
{
	Super::BeginPlay();	
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
	if (MovementAngle.Yaw != ShipDirection->GetComponentRotation().Yaw)
	{
		// Turn in one frame
		ShipDirection->SetWorldRotation(MovementAngle);
	}
}

// move the ship in the direction it is facing
void ABasic_Ship::MoveTheShip(float DeltaTime)
{
	FVector SpeedVector = ShipDirection->GetForwardVector() * MoveSpeed;
	FVector Pos = GetActorLocation();
	Pos += SpeedVector * DeltaTime;
	SetActorLocation(Pos);
}

// Called to bind functionality to input
void ABasic_Ship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveX", this, &ABasic_Ship::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &ABasic_Ship::MoveY);

	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Pressed, this, &ABasic_Ship::Fire1Pressed);
	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Released, this, &ABasic_Ship::Fire1Released);

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
