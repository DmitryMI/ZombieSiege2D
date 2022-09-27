// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "UnitGraphicsData.h"
#include "Building.h"
#include "BuildingPlacementMarker.generated.h"

UCLASS()
class ZOMBIESIEGE_API ABuildingPlacementMarker : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly)
	FLinearColor buildingPossibleColor = FLinearColor(0.3, 0.3, 1, 0.6);

	UPROPERTY(EditDefaultsOnly)
	FLinearColor obstacleDetectedColor = FLinearColor(1, 0.3, 0.3, 0.6);;

	UPROPERTY(Transient)
	UPaperFlipbookComponent* markerRenderer;

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<ABuilding> buildingClass;

	ABuilding* buildingDefaultObject;

	UUnitGraphicsData* buildingGraphicsData;

	bool bCanBeBuiltNow;
	
public:	
	// Sets default values for this actor's properties
	ABuildingPlacementMarker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateMarkerRenderer();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	TSubclassOf<ABuilding> GetBuildingClass();

	UFUNCTION(BlueprintCallable)
	void SetBuildingClass(TSubclassOf<ABuilding> clazz);

	UFUNCTION(BlueprintCallable)
	bool CanBeBuiltNow();
};
