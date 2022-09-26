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

protected:
	virtual void BeginPlay() override;

public:
	ABuilding();

	virtual float ReceiveHealing(const FHealingInstance& repair);

	bool IsBuiltOnSpawn();

	bool IsFullyBuilt();

	void SetIsBuiltOnSpawn(bool isBuiltOnSpawn);
};
