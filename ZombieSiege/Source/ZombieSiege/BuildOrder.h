// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitOrder.h"
#include "Building.h"
#include "BuildOrder.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UBuildOrder : public UUnitOrder
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABuilding> buildingClass;

	FVector targetLocation;

public:
	UBuildOrder();

	virtual void ExecuteOrder() override;

	void SetParameters(TSubclassOf<ABuilding> clazz, const FVector& location);
};
