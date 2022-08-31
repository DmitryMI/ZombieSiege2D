// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitIsDyingEventArgs.generated.h"

class AUnitBase;

USTRUCT(BlueprintType)
struct ZOMBIESIEGE_API FUnitIsDyingEventArgs
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* dyingUnit;

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* killingUnit;


public:

	FUnitIsDyingEventArgs()
	{
		dyingUnit = nullptr;
		killingUnit = nullptr;
	}

	FUnitIsDyingEventArgs(AUnitBase* dyingUnitARg, AUnitBase* killer)
	{
		dyingUnit = dyingUnitARg;
		killingUnit = killer;
	}
	
	~FUnitIsDyingEventArgs()
	{
	}
};
