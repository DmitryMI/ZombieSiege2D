// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitDiedEventArgs.generated.h"

class AUnitBase;

USTRUCT(BlueprintType)
struct ZOMBIESIEGE_API FUnitDiedEventArgs
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* deadUnit;

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* killerUnit;


public:

	FUnitDiedEventArgs()
	{
		deadUnit = nullptr;
		killerUnit = nullptr;
	}

	FUnitDiedEventArgs(AUnitBase* deadUnitArg, AUnitBase* killer)
	{
		deadUnit = deadUnitArg;
		killerUnit = killer;
	}
	
	~FUnitDiedEventArgs()
	{
	}
};
