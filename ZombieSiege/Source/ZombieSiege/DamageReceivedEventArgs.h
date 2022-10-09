// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageReceivedEventArgs.generated.h"

class AUnitBase;
class UWeaponInfo;

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
	UWeaponInfo* weaponInfo;

	UPROPERTY(BlueprintReadWrite)
	float inflictedDamageAmount;

public:

	FDamageReceivedEventArgs()
	{
		target = nullptr;
		source = nullptr;
		inflictedDamageAmount = 0;
		weaponInfo = nullptr;
	}

	FDamageReceivedEventArgs(AUnitBase* targetArg, AUnitBase* sourceArg, float amountArg, UWeaponInfo* weaponInfoArg)
	{
		target = targetArg;
		source = sourceArg;
		inflictedDamageAmount = amountArg;
		weaponInfo = weaponInfoArg;

		check(inflictedDamageAmount >= 0);
	}
	
	~FDamageReceivedEventArgs()
	{
	}
};
