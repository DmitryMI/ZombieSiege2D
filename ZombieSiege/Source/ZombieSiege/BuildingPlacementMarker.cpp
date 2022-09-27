// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPlacementMarker.h"
#include "UnitGraphicsManager.h"
#include "UnitGraphicsComponent.h"

// Sets default values
ABuildingPlacementMarker::ABuildingPlacementMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABuildingPlacementMarker::BeginPlay()
{
	Super::BeginPlay();
	
	UActorComponent* rendererComponent = GetComponentByClass(UPaperFlipbookComponent::StaticClass());

	markerRenderer = Cast<UPaperFlipbookComponent>(rendererComponent);
	check(markerRenderer);

	UpdateMarkerRenderer();
}

void ABuildingPlacementMarker::UpdateMarkerRenderer()
{
	if(!buildingDefaultObject)
	{
		return;
	}

	check(markerRenderer);	

	UPaperFlipbook* flipbook = buildingGraphicsData->GetFlipbook(EUnitState::None, EFaceDirection::Up);
	check(flipbook);
	markerRenderer->SetFlipbook(flipbook);
}

// Called every frame
void ABuildingPlacementMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!buildingDefaultObject)
	{
		return;
	}

	FVector location = GetActorLocation();

	bCanBeBuiltNow = buildingDefaultObject->CanBeBuildAt(GetWorld(), location);
	if (bCanBeBuiltNow)
	{
		markerRenderer->SetSpriteColor(buildingPossibleColor);
	}
	else
	{
		markerRenderer->SetSpriteColor(obstacleDetectedColor);
	}
}

TSubclassOf<ABuilding> ABuildingPlacementMarker::GetBuildingClass()
{
	return buildingClass;
}

void ABuildingPlacementMarker::SetBuildingClass(TSubclassOf<ABuilding> clazz)
{
	// Is it a correct check?
	check(clazz);

	buildingClass = clazz;
	buildingDefaultObject = Cast<ABuilding>(buildingClass->GetDefaultObject());

	check(buildingDefaultObject);

	AUnitGraphicsManager* graphicsManager = AUnitGraphicsManager::GetInstance(GetWorld());
	check(graphicsManager);

	buildingGraphicsData = graphicsManager->GetUnitGraphicsData(buildingDefaultObject->GetUnitGraphicsDataName());

	UpdateMarkerRenderer();
}

bool ABuildingPlacementMarker::CanBeBuiltNow()
{
	return bCanBeBuiltNow;
}

