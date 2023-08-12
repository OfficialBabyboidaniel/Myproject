// Fill out your copyright notice in the Description page of Project Settings.


#include "AirState.h"

UAirState::UAirState()
{
}

void UAirState::BeginPlay()
{
	Super::BeginPlay();
}

void UAirState::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bShouldTick)
	{
		Update(DeltaTime);	
	}
}

void UAirState::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("Airstate ticking"));
}