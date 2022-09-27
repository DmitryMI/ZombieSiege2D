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

	unitGraphicsData = AUnitGraphicsManager::GetInstance(GetWorld())->GetUnitGraphicsData(unitOwner->GetUnitGraphicsDataName());

	if (!unitGraphicsData)
	{
		FString ownerName;
		GetOwner()->GetName(ownerName);
		UE_LOG(LogTemp, Error, TEXT("Failed to setup graphics for unit %s"), *ownerName);
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
	if (!unitGraphicsData)
	{
		return;
	}

	UPaperFlipbook* flipbook = unitGraphicsData->GetFlipbook(state, direction);

	check(flipbookRenderer);
	flipbookRenderer->SetFlipbook(flipbook);
}


void UUnitGraphicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* owner = GetOwner();
	check(owner);

	AHumanoid* humanoid = Cast<AHumanoid>(owner);
	check(humanoid);

	UpdateFlipbook(humanoid->GetUnitState(), humanoid->GetFacingDirection());
}
