// Fill out your copyright notice in the Description page of Project Settings.


#include "FootstepSoundComponent.h"

// Sets default values for this component's properties
UFootstepSoundComponent::UFootstepSoundComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bCanPlayMultipleInstances = true;
}


// Called when the game starts
void UFootstepSoundComponent::BeginPlay()
{
	Super::BeginPlay();

	lastFrameLocation = GetOwner()->GetActorLocation();
}


// Called every frame
void UFootstepSoundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector location = GetOwner()->GetActorLocation();

	float delta = FVector::Distance(location, lastFrameLocation);
	travelledDistanceAccumulator += delta;

	if (travelledDistanceAccumulator >= triggerDistance)
	{
		travelledDistanceAccumulator = 0.0f;

		Play();
	}

	lastFrameLocation = location;
}

