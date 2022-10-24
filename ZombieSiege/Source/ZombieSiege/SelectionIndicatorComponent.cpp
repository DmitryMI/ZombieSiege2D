// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectionIndicatorComponent.h"

// Sets default values for this component's properties
USelectionIndicatorComponent::USelectionIndicatorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USelectionIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateSelectionFlipbook();	
}

void USelectionIndicatorComponent::UpdateSelectionFlipbook()
{
	if (bIsSelected)
	{
		check(indicatorFlipbook);
		SetFlipbook(indicatorFlipbook);

		if (attitude == ETeamAttitude::Friendly)
		{
			SetSpriteColor(selectionColorFriendly);
		}
		else if (attitude == ETeamAttitude::Hostile)
		{
			SetSpriteColor(selectionColorHostile);
		}
		else if (attitude == ETeamAttitude::Neutral)
		{
			SetSpriteColor(selectionColorNeutral);
		}
		else
		{
			checkNoEntry();
		}
	}
	else
	{
		SetFlipbook(nullptr);
	}
}


// Called every frame
void USelectionIndicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool USelectionIndicatorComponent::IsSelected()
{
	return bIsSelected;
}

void USelectionIndicatorComponent::SetIsSelected(bool bIsSelectedArg, ETeamAttitude::Type attitudeArg)
{
	if (bIsSelected == bIsSelectedArg)
	{
		return;
	}

	bIsSelected = bIsSelectedArg;
	attitude = attitudeArg;

	UpdateSelectionFlipbook();
}

