// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPlacementMarker.h"
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
	
	UActorComponent* rendererComponent = GetComponentByClass(UPaperSpriteComponent::StaticClass());

	markerRenderer = Cast<UPaperSpriteComponent>(rendererComponent);
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

	//UPaperFlipbook* flipbook = graphics->GetPreviewFlipbook();
	//markerRenderer->SetSprite();
}

// Called every frame
void ABuildingPlacementMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	UpdateMarkerRenderer();
}

