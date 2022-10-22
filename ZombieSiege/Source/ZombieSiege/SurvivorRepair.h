// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInfo.h"
#include "ResourceType.h"
#include "Building.h"
#include "Macros.h"
#include "SurvivorRepair.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API USurvivorRepair : public UWeaponInfo
{
	GENERATED_BODY()

protected:
	TMap<EResourceType, float> CalculateRequiredResourceAmount(ABuilding* building, float repairAmount) const;
	
public:

	virtual void AttackTarget(AUnitBase* attacker, AActor* target, const FAttackParameters& params = FAttackParameters()) override;

	virtual bool CanAttackTarget(AUnitBase* attacker, AActor* target) override;

};
