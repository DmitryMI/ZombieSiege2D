// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "HealingInstance.h"
#include "Building.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ABuilding : public AUnitBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	bool bIsBuiltOnSpawn = false;

	UPROPERTY(VisibleAnywhere)
	float buildingProgress = 0.0f;

public:
	ABuilding();

	void ReceiveRepair(const FHealingInstance& repair);

	bool IsBuiltOnSpawn();

	void SetIsBuiltOnSpawn(bool isBuiltOnSpawn);
};
