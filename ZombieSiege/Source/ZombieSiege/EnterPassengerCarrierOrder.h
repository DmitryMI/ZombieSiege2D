// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitOrder.h"
#include "EnterPassengerCarrierOrder.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UEnterPassengerCarrierOrder : public UUnitOrder
{
	GENERATED_BODY()

private:
	UPROPERTY()
	AUnitBase* targetCarrier;

public:

	UFUNCTION(BlueprintCallable)
	AUnitBase* GetTargetUnit();
	
	UFUNCTION(BlueprintCallable)
	void SetTargetUnit(AUnitBase* targetUnit);

	virtual void ExecuteOrder() override;
};
