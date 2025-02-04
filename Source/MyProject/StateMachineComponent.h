// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "AirState.h"
#include "GroundState.h"
#include "Components/ActorComponent.h"
#include  "StateComponent.h"
#include "StateMachineComponent.generated.h"


class APlayerCharThreeD;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateMachineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<UStateComponent*> States;
	UStateComponent* CurrentState;

	void ChangeState(UStateComponent* State);
	UGroundState* GroundState;
	UAirState* AirState;
	APlayerCharThreeD* PlayerCharThreeD;
	
};
