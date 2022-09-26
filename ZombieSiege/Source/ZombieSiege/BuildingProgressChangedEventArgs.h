// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingProgressChangedEventArgs.generated.h"

class AUnitBase;

USTRUCT(BlueprintType)
struct ZOMBIESIEGE_API FBuildingProgressChangedEventArgs
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* unit;

	UPROPERTY(BlueprintReadWrite)
	float progressOld;

	UPROPERTY(BlueprintReadWrite)
	float progressNew;
	
public:

	FBuildingProgressChangedEventArgs()
	{
		unit = nullptr;
		progressOld = 0;
		progressOld = 0;
	}

	FBuildingProgressChangedEventArgs(AUnitBase* unitArg, float progressOldArg, float progressNewArg)
	{
		unit = unitArg;
		progressOld = progressOldArg;
		progressNew = progressNewArg;
	}
	
	~FBuildingProgressChangedEventArgs()
	{
	}
};
