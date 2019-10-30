// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BasicShip.h"
#include "StaticFunctions.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/MovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerInput.h"

FName ABasicShip::MovementComponentName(TEXT("MovementComponent0"));
FName ABasicShip::CollisionComponentName(TEXT("CollisionComponent0"));
FName ABasicShip::MeshComponentName(TEXT("MeshComponent0"));

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

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;

	BaseEyeHeight = 0.0f;
	bCollideWhenPlacing = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(ABasicShip::CollisionComponentName);
	CollisionComponent->InitSphereRadius(35.0f);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetShouldUpdatePhysicsVolume(true);
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->bDynamicObstacle = true;

	RootComponent = CollisionComponent;

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(ABasicShip::MovementComponentName);
	MovementComponent->UpdatedComponent = CollisionComponent;

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh;
		FConstructorStatics()
			: SphereMesh(TEXT("/Engine/EngineMeshes/Sphere")) {}
	};

	static FConstructorStatics ConstructorStatics;

	MeshComponent = CreateOptionalDefaultSubobject<UStaticMeshComponent>(ABasicShip::MeshComponentName);
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(ConstructorStatics.SphereMesh.Object);
		MeshComponent->AlwaysLoadOnClient = true;
		MeshComponent->AlwaysLoadOnServer = true;
		MeshComponent->bOwnerNoSee = true;
		MeshComponent->bCastDynamicShadow = true;
		MeshComponent->bAffectDynamicIndirectLighting = false;
		MeshComponent->bAffectDistanceFieldLighting = false;
		MeshComponent->bVisibleInRayTracing = false;
		MeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		MeshComponent->SetupAttachment(RootComponent);
		MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
		const float Scale = CollisionComponent->GetUnscaledSphereRadius() / 160.f; // @TODO: hardcoding known size of EngineMeshes.Sphere. Should use a unit sphere instead.
		MeshComponent->SetRelativeScale3D(FVector(Scale));
		MeshComponent->SetGenerateOverlapEvents(false);
		MeshComponent->SetCanEverAffectNavigation(false);
	}

	// This is the default pawn class, we want to have it be able to move out of the box.
	bAddDefaultMovementBindings = true;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollider;
	BoxCollider->SetGenerateOverlapEvents(true);
	BoxCollider->SetNotifyRigidBodyCollision(true);

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
	BoxShape = FVector(45.0f, 20.0f, 50.0f);
}

// Called when the game starts or when spawned
void ABasicShip::BeginPlay()
{
	Super::BeginPlay();	
	CurrentHealth = MaxHealth;

	Artillery->SetCannonNum(CannonNum);
	Artillery->SetRandomStd(RandomStd);
	Artillery->SetCannonNum(Fire1Cooldown);

	// BoxCollider->SetBoxExtent(BoxShape, true);
	// BoxCollider->SetCollisionProfileName(FName(TEXT("BlockAll")));
	// BoxCollider->OnComponentHit.AddDynamic(this, &ABasicShip::OnHit);
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
	BoxCollider->SetCollisionProfileName("BlockAll");
	
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

void InitializeDefaultPawnInputBindings()
{
	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::S, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::Up, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::Down, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::Gamepad_LeftY, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveRight", EKeys::A, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveRight", EKeys::D, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveRight", EKeys::Gamepad_LeftX, 1.f));

		// HACK: Android controller bindings in ini files seem to not work
		//  Direct overrides here some to work
#if !PLATFORM_ANDROID
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_LeftThumbstick, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_RightThumbstick, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_FaceButton_Bottom, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::LeftControl, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::SpaceBar, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::C, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::E, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Q, -1.f));
#else
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_LeftTriggerAxis, -0.5f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveUp", EKeys::Gamepad_RightTriggerAxis, 0.5f));
#endif

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_TurnRate", EKeys::Gamepad_RightX, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_TurnRate", EKeys::Left, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_TurnRate", EKeys::Right, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_Turn", EKeys::MouseX, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_LookUpRate", EKeys::Gamepad_RightY, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_LookUp", EKeys::MouseY, -1.f));
	}
}

//void ABasicShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	check(PlayerInputComponent);
//
//	if (bAddDefaultMovementBindings)
//	{
//		InitializeDefaultPawnInputBindings();
//
//		PlayerInputComponent->BindAxis("DefaultPawn_MoveForward", this, &ABasicShip::MoveForward);
//		PlayerInputComponent->BindAxis("DefaultPawn_MoveRight", this, &ABasicShip::MoveRight);
//		PlayerInputComponent->BindAxis("DefaultPawn_MoveUp", this, &ABasicShip::MoveUp_World);
//		PlayerInputComponent->BindAxis("DefaultPawn_Turn", this, &ABasicShip::AddControllerYawInput);
//		PlayerInputComponent->BindAxis("DefaultPawn_TurnRate", this, &ABasicShip::TurnAtRate);
//		PlayerInputComponent->BindAxis("DefaultPawn_LookUp", this, &ABasicShip::AddControllerPitchInput);
//		PlayerInputComponent->BindAxis("DefaultPawn_LookUpRate", this, &ABasicShip::LookUpAtRate);
//	}
//}

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
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// transform to world space and add it
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), Val);
		}
	}
}

void ABasicShip::MoveUp_World(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(FVector::UpVector, Val);
	}
}

void ABasicShip::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

void ABasicShip::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

UPawnMovementComponent* ABasicShip::GetMovementComponent() const
{
	return MovementComponent;
}
