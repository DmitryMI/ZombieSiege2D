// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInfo.h"
#include "MeleeWeaponInfo.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UMeleeWeaponInfo : public UWeaponInfo
{
	GENERATED_BODY()
	
public:

	virtual void AttackTarget(AUnitBase* attacker, AUnitBase* target, const FAttackParameters& params = FAttackParameters()) override;
};
