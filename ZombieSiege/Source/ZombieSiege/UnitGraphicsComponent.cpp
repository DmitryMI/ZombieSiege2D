// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitGraphicsComponent.h"
#include "Humanoid.h"
#include "FaceDirection.h"
#include "UnitGraphicsManager.h"
#include "UnitState.h"

UUnitGraphicsComponent::UUnitGraphicsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UUnitGraphicsComponent::BeginPlay()
{
	Super::BeginPlay();

	AUnitBase* unitOwner = Cast<AUnitBase>(GetOwner());

	AUnitGraphicsManager* graphicsManager = AUnitGraphicsManager::GetInstance(GetWorld());
	check(graphicsManager);

	if (bAutoSetGraphicsData)
	{
		if (bOverrideGraphicsDataName)
		{
			unitGraphicsData = graphicsManager->GetUnitGraphicsData(graphicsDataName);
		}
		else if(unitOwner)
		{
			unitGraphicsData = graphicsManager->GetUnitGraphicsData(unitOwner->GetUnitGraphicsDataName());
		}

		if (!unitGraphicsData)
		{
			FString ownerName;
			GetOwner()->GetName(ownerName);
			UE_LOG(LogTemp, Error, TEXT("Failed to setup graphics for %s"), *ownerName);
		}
	}
	
	TArray<USceneComponent*> children;
	GetChildrenComponents(true, children);
	for (USceneComponent* child : children)
	{
		UPaperFlipbookComponent* flipbookComponent = Cast<UPaperFlipbookComponent>(child);
		if (flipbookComponent != nullptr)
		{
			flipbookRenderer = flipbookComponent;
		}
	}
	check(flipbookRenderer);

	flipbookRenderer->SetWorldRotation(FRotator(0, 0, -90));
}

void UUnitGraphicsComponent::UpdateFlipbook(EUnitState state, EFaceDirection direction)
{
	UPaperFlipbook* flipbook = nullptr;

	if (unitGraphicsData)
	{
		flipbook = unitGraphicsData->GetFlipbook(state, direction);
	}

	check(flipbookRenderer);
	flipbookRenderer->SetFlipbook(flipbook);
}


void UUnitGraphicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bAutoUpdateFlipbook && minimalFlipbookAutoUpdateInterval <= flipbookAutoUpdateIntervalCounter)
	{
		AActor* owner = GetOwner();
		check(owner);

		AUnitBase* unitOwner = Cast<AUnitBase>(owner);
		
		if (!unitOwner)
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot auto-update flipbook, because the owner is not a Unit"));
			bAutoUpdateFlipbook = false;
		}
		else
		{
			flipbookAutoUpdateIntervalCounter = 0;
			UpdateFlipbook(unitOwner->GetUnitState(), unitOwner->GetFacingDirection());
		}
	}

	if (minimalFlipbookAutoUpdateInterval > flipbookAutoUpdateIntervalCounter)
	{
		flipbookAutoUpdateIntervalCounter += DeltaTime;
	}
}

void UUnitGraphicsComponent::SetUnitGraphicsData(UUnitGraphicsData* graphicsData)
{
	unitGraphicsData = graphicsData;

	// TODO Auto update flipbook
}

UUnitGraphicsData* UUnitGraphicsComponent::GetUnitGraphicsData()
{
	return unitGraphicsData;
}
