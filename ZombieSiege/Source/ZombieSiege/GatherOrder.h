// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitOrder.h"
#include "Doodad.h"
#include "GatherOrder.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UGatherOrder : public UUnitOrder
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	ADoodad* gatherTarget;
	
public:
	UGatherOrder();

	virtual void Execute() override;

	UFUNCTION(BlueprintCallable)
	void SetGatherTarget(ADoodad* target);
};
