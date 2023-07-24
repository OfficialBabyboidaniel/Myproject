// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"

#include <Windows.Graphics.Display.h>

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Sampling/NormalHistogram.h"
#include "StaticHelperClass.h"
#include "Styling/StyleColors.h"
#include "Util/ColorConstants.h"
#include "DrawDebugHelpers.h"
// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// aktivera tick
	PrimaryActorTick.bCanEverTick = true;
	// auto posses player 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// create root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// create mesh
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	// create camera
	auto PlayerCamera = CreateDefaultSubobject<UCameraComponent>(
		TEXT("PlayerCamera"));

	// setup camera
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->SetRelativeLocation(FVector(0.0f, 1600.0f, 200.0f));
	PlayerCamera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// set to 2d view
	PlayerCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	PlayerCamera->OrthoWidth = 3200.f;

	OurVisibleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DrawDebugBox(GetWorld(), Origin, Extent, FColor::Red, true);
	Gravity = FVector::DownVector * GravityForce * DeltaTime;

	//addera jump och gravity 
	Velocity += Gravity + JumpMovement;
	//calculera input.
	CalculateInput(DeltaTime);
	//multiplicera med luftmotstånd
	Velocity *=  FMath::Pow(AirResistanceCoefficient, DeltaTime);
	UpdateVelocity(DeltaTime);
	// Y axis alltid noll för att stanna i 2D. 
	Velocity.Y = 0;
	
	UE_LOG(LogTemp, Warning, TEXT("Final Velocity before delta time %s"), *Velocity.ToString());
	const FVector CurrentLocation = GetActorLocation();
	SetActorLocation(CurrentLocation + Velocity * DeltaTime);
	JumpMovement = FVector::ZeroVector;
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind inputs
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::JumpInput);
	PlayerInputComponent->BindAxis("Vertical", this, &APlayerPawn::VerticalInput);
	PlayerInputComponent->BindAxis("Horizontal", this, &APlayerPawn::HorizontalInput);
}

// horizontal axis input
void APlayerPawn::HorizontalInput(float AxisValue)
{
	CurrentInput = FVector(AxisValue, 0.0f, CurrentInput.Z);
}

// vertical axis input
void APlayerPawn::VerticalInput(float AxisValue)
{
	CurrentInput = FVector(CurrentInput.X, 0.0f, AxisValue);
}

// jump input
void APlayerPawn::JumpInput()
{
	// kolla igenom sen?
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FVector TraceEnd = Origin + FVector::DownVector * (GroundCheckDistance + SkinWidth);
	Params.AddIgnoredActor(this);
	//använda get actor location? 
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Origin, TraceEnd, FQuat::Identity, ECC_Pawn,
	                                             FCollisionShape::MakeBox(Extent), Params);
	if (bHit)
	{
		JumpMovement = FVector(0, 0, JumpForce);
	}
}

void APlayerPawn::UpdateVelocity(float DeltaTime)
{
	GetActorBounds(true, Origin, Extent);
	FHitResult Hit;
	FHitResult NormalHit;
	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + Velocity.GetSafeNormal() * Velocity.Size() * DeltaTime + SkinWidth;
	Params.AddIgnoredActor(this);
	bool bHit;
	bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeBox(Extent),
		Params);

	DrawDebugLine(
		GetWorld(),
		TraceStart,
		TraceEnd,
		FColor::Red, // Line color (you can change this to any color you like)
		false, // Persistent (false means the line will disappear after one frame)
		5.0f, // LifeTime (negative value means the line will stay forever)
		0, // DepthPriority (you can adjust this if necessary)
		2.0f // Thickness (you can adjust this to change the line's thickness)
	);
	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("box cast hit  true"));

		TraceEnd = Origin - Hit.Normal * Hit.Distance;
		bHit = GetWorld()->SweepSingleByChannel(NormalHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Pawn,
		                                        FCollisionShape::MakeBox(Extent), Params);
		//flytta actor mto normalen av träffpunkten
		SetActorLocation(GetActorLocation() - Hit.Normal * (NormalHit.Distance - SkinWidth));
	}

	if (Velocity.Size() /** GetWorld()->DeltaTimeSeconds*/ < 0.1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Movement too small, returning zero vector."));
		RecursivCounter = 0;
		//ska  det vara zero vector här?
		Velocity = FVector::ZeroVector;
		return;
	}

	if (RecursivCounter > 10)
	{
		UE_LOG(LogTemp, Warning, TEXT("Recursive counter 10"));
		RecursivCounter = 0;
		Velocity = FVector::ZeroVector;
	}
	// UE_LOG(LogTemp, Warning, TEXT("Counter %d"), RecursivCounter);
	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("2nd collision found, Collision Movement = %s"), *Velocity.ToString());

		//dubbel kolla normal kraft beräkning, det gungar ftf, kan ej vara helt still.  
		FVector NormalPower = StaticHelperClass::DotProduct(Velocity, Hit.ImpactNormal);
		//skriva kod här


		UE_LOG(LogTemp, Warning, TEXT("Normal power %s"), *NormalPower.ToString());
		Velocity += NormalPower /*+ NormalHit.ImpactNormal * SkinWidth*/;
		UE_LOG(LogTemp, Warning, TEXT("velocity after added normal power %s"), *Velocity.ToString());
		RecursivCounter++;
		
		ApplyFriction(DeltaTime, NormalPower.Size());
		UpdateVelocity(DeltaTime);
	}
	RecursivCounter = 0;
}

void APlayerPawn::CalculateInput(float DeltaTime)
{
	Velocity += CurrentInput.GetSafeNormal() * Acceleration * DeltaTime;
}

void APlayerPawn::ApplyFriction(float DeltaTime, float NormalMagnitude)
{
	if (Velocity.Size() <
		NormalMagnitude * StaticFrictionCoefficient)
	{
		Velocity = FVector::ZeroVector;
	}
	else
	{
		Velocity -= Velocity.GetSafeNormal() * NormalMagnitude *
			KineticFrictionCoefficient;
	}
}


// char fastnar i väggar ifall vi har för hög fart när vi kolliderar.
// studsar ftf mot marken (vibrerar) någon värde calculering är fel. 