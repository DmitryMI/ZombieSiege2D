// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DamageInstance.generated.h"

class UWeaponInfo;
class AUnitBase;

USTRUCT(BlueprintType)
struct ZOMBIESIEGE_API FDamageInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	/// <summary>
	/// Unit that inflicts damage. Can be nullptr (e.g. for environmental damage)
	/// </summary>
	AUnitBase* source;

	UPROPERTY(BlueprintReadWrite)	
	UWeaponInfo* weaponInfo;

	UPROPERTY(BlueprintReadWrite)
	/// <summary>
	/// Raw amount of damage before all modifiers and reductions
	/// </summary>
	float amount;

public:
	/// <summary>
	/// Default constructor for an empty Damage Instance
	/// </summary>
	FDamageInstance();

	FDamageInstance(AUnitBase* sourceArg, float amountArg, UWeaponInfo* weaponInfoArg);
	~FDamageInstance();
};
