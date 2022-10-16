// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitEnteredPassengerCarrierEventArgs.generated.h"

class AUnitBase;

USTRUCT(BlueprintType)
struct ZOMBIESIEGE_API FUnitEnteredPassengerCarrierEventArgs
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* enteringUnit;

	UPROPERTY(BlueprintReadWrite)
	AUnitBase* passengerCarrier;
	
public:

	FUnitEnteredPassengerCarrierEventArgs()
	{
		enteringUnit = nullptr;
		passengerCarrier = nullptr;
	}

	FUnitEnteredPassengerCarrierEventArgs(AUnitBase* enteringUnitArg, AUnitBase* carrier)
	{
		enteringUnit = enteringUnitArg;
		passengerCarrier = carrier;
	}
	
	~FUnitEnteredPassengerCarrierEventArgs()
	{
	}
};
