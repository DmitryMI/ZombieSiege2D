// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitOrder.h"
#include "Building.h"
#include "RepairOrder.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API URepairOrder : public UUnitOrder
{
	GENERATED_BODY()

protected:
	ABuilding* targetBuilding;

public:
	URepairOrder();

	virtual void Execute() override;

	void SetTargetBuilding(ABuilding* building);
};
