// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharThreeD.generated.h"

class UStateMachineComponent;
class UCameraComponent;
UCLASS()
class MYPROJECT_API APlayerCharThreeD : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharThreeD();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//get and set functions
	FVector GetCurrentInput() const;
	FVector GetJumpInput() const;
	FVector GetPlayerVelocity() const;
	float GetPitchAxisValue() const;
	float GetYawAxisValue() const;
	UStateMachineComponent* GetStateMachine() const;
	void SetCurrentInput(const FVector NewValue);
	void SetJumpInput(const FVector NewValue);
	void SetVelocity(const FVector NewValue) ; 

private:
	void XInput(float AxisValue);
	void YInput(float AxisValue);
	void JumpInput();
	void LookRight(float AxisValue);
	void LookUp(float AxisValue);

	//Vectors values
	FVector CurrentInput;
	FVector JumpMovement;
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	double YawAxisValue;
	UPROPERTY(VisibleAnywhere, Category="Camera Stats")
	double PitchAxisValue;

	//Sweep values
	FVector Origin, Extent;
	FCollisionQueryParams Params;

	//jump info 
	UPROPERTY(EditAnywhere, Category="Stats")
	float GroundCheckDistance = 2.2f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float JumpForce = 500.0f;
	UPROPERTY(EditAnywhere, Category="Stats")
	float SkinWidth = 2.0f;

	UPROPERTY(VisibleAnywhere, Category="State")
	class UStateComponent* CurrentState;
	UPROPERTY(VisibleAnywhere, Category="State")
	class UStateMachineComponent* StateMachineComponent;
};
