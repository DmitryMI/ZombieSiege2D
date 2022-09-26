// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthChangedEventArgs.generated.h"

class AUnitBase;

USTRUCT(BlueprintType)
struct ZOMBIESIEGE_API FHealthChangedEventArgs
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* unit;

	UPROPERTY(BlueprintReadWrite)
	float healthOld;

	UPROPERTY(BlueprintReadWrite)
	float healthNew;
	
public:

	FHealthChangedEventArgs()
	{
		unit = nullptr;
		healthOld = 0;
		healthNew = 0;
	}

	FHealthChangedEventArgs(AUnitBase* unitArg, float healthOldArg, float healthNewArg)
	{
		unit = unitArg;
		healthOld = healthOldArg;
		healthNew = healthNewArg;
	}
	
	~FHealthChangedEventArgs()
	{
	}
};
