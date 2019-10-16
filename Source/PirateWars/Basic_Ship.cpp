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

	// Default values for speed, acceleration and rotation
	MoveSpeedForward = 100.0f;
	MoveSpeedBackward = 10.0f;

	MoveAccel = 200.0f;
	YawSpeed = 180.0f;
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

	{
		FVector DesiredMovementDirection = FVector(InputAdapter.MovementInput.X, InputAdapter.MovementInput.Y, 0.0f);
		if (!DesiredMovementDirection.IsNearlyZero())
		{
			// Rotate the tank. This process deals with ShipDirection directly, not the actor/RootComponent, because we don't want to affect the camera.
			FRotator MovementAngle = DesiredMovementDirection.Rotation();
			float DeltaYaw = UStaticFunctions::FindDeltaAngleDegrees(ShipDirection->GetComponentRotation().Yaw, MovementAngle.Yaw);
			bool bReverse = false;
			if (DeltaYaw != 0.0f)
			{
				float AdjustedDeltaYaw = DeltaYaw;
				// If we're trying to go more than 90 degrees away from our current facing, reverse.
				/*if (AdjustedDeltaYaw < -136.0f)
				{
					bReverse = true;
					AdjustedDeltaYaw += 180.0f;
					UE_LOG(LogTemp, Warning, TEXT("REVERSE: Anegle = %f"), AdjustedDeltaYaw)
				}
				else if (AdjustedDeltaYaw > 136.0f)
				{
					bReverse = true;
					AdjustedDeltaYaw -= 180.0f;
					UE_LOG(LogTemp, Warning, TEXT("REVERSE: Anegle = %f"), AdjustedDeltaYaw)
				}*/

				// Adjust toward our desired angle, and stop if we've reached it.
				float MaxYawThisFrame = YawSpeed * DeltaTime;
				if (MaxYawThisFrame >= FMath::Abs(AdjustedDeltaYaw))
				{
					if (bReverse)
					{
						// Move backward. Use a temp variable in case we need MovementAngle to be correct later.
						FRotator FacingAngle = MovementAngle;
						FacingAngle.Yaw = MovementAngle.Yaw + 180.0f;
						ShipDirection->SetWorldRotation(FacingAngle);
					}
					else
					{
						// Finish, moving forward. Facing and movement are the same angle.
						ShipDirection->SetWorldRotation(MovementAngle);
					}
				}
				else
				{
					// Adjust as far as we can this frame, because we know we can't reach the goal yet.
					ShipDirection->AddLocalRotation(FRotator(0.0f, FMath::Sign(AdjustedDeltaYaw) * MaxYawThisFrame, 0.0f));
				}
			}

			// Move the tank
			{
				FVector SpeedVector = ShipDirection->GetForwardVector() * (bReverse ? -1.0f*MoveSpeedBackward : MoveSpeedForward);
				FVector Pos = GetActorLocation();
				Pos += SpeedVector * DeltaTime;
				SetActorLocation(Pos);
			}
		}
	}
}

// Called to bind functionality to input
void ABasic_Ship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveX", this, &ABasic_Ship::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &ABasic_Ship::MoveY);
}

void ABasic_Ship::MoveX(float AxisValue)
{
	InputAdapter.MoveX(AxisValue);
}

void ABasic_Ship::MoveY(float AxisValue)
{
	InputAdapter.MoveY(AxisValue);
}