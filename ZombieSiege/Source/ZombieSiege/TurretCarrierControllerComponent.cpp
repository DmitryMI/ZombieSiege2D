// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretCarrierControllerComponent.h"
#include "UnitAiController.h"

// Sets default values for this component's properties
UTurretCarrierControllerComponent::UTurretCarrierControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTurretCarrierControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	AUnitAiController* controller = GetOwningController<AUnitAiController>();
	check(controller);

	controller->OnPossessedPawnChanged().AddUObject(this, &UTurretCarrierControllerComponent::OnOwnerPossessedPawnChangedHandler);

	controlledUnit = Cast<AUnitBase>(controller->GetPawn());
}

void UTurretCarrierControllerComponent::OnOwnerPossessedPawnChangedHandler(const FPossessedPawnChangedEventArgs& args)
{
	controlledUnit = args.possessedUnitNew;
}


// Called every frame
void UTurretCarrierControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

