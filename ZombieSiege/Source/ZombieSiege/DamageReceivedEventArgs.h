// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageReceivedEventArgs.generated.h"

class AUnitBase;

USTRUCT(BlueprintType)
struct ZOMBIESIEGE_API FDamageReceivedEventArgs
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* target;

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* source;

	UPROPERTY(BlueprintReadWrite)
	float inflictedDamageAmount;


public:

	FDamageReceivedEventArgs()
	{
		target = nullptr;
		source = nullptr;
		inflictedDamageAmount = 0;
	}

	FDamageReceivedEventArgs(AUnitBase* targetArg, AUnitBase* sourceArg, float amountArg)
	{
		target = targetArg;
		source = sourceArg;
		inflictedDamageAmount = amountArg;

		check(inflictedDamageAmount >= 0);
	}
	
	~FDamageReceivedEventArgs()
	{
	}
};
