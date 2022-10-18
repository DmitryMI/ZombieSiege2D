// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretControllerComponent.h"
#include "UnitAiController.h"

// Sets default values for this component's properties
UTurretControllerComponent::UTurretControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTurretControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	AUnitAiController* controller = GetOwningController<AUnitAiController>();
	check(controller);

	controller->OnPossessedPawnChanged().AddUObject(this, &UTurretControllerComponent::OnOwnerPossessedPawnChangedHandler);

	controlledUnit = Cast<AUnitBase>(controller->GetPawn());
}

void UTurretControllerComponent::OnOwnerPossessedPawnChangedHandler(const FPossessedPawnChangedEventArgs& args)
{
	controlledUnit = args.possessedUnitNew;
}


// Called every frame
void UTurretControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

