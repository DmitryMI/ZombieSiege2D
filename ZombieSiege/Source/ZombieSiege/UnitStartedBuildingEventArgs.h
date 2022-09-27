// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "UnitStartedBuildingEventArgs.generated.h"

class AUnitBase;

USTRUCT(BlueprintType)
struct ZOMBIESIEGE_API FUnitStartedBuildingEventArgs
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* builderUnit;
	
	UPROPERTY(BlueprintReadWrite)
	ABuilding* building;
	
public:

	FUnitStartedBuildingEventArgs()
	{
		builderUnit = nullptr;
		building = nullptr;
	}

	FUnitStartedBuildingEventArgs(AUnitBase* builderUnitArg, ABuilding* buildingArg)
	{
		builderUnit = builderUnitArg;
		building = buildingArg;
	}
	
	~FUnitStartedBuildingEventArgs()
	{
	}
};
