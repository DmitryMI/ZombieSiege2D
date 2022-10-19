// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitOrder.h"
#include "AttackUnitOrder.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UAttackUnitOrder : public UUnitOrder
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	AUnitBase* targetUnit;
public:
	UAttackUnitOrder();

	UFUNCTION(BlueprintCallable)
	void SetTargetUnit(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	AUnitBase* GetTargetUnit();

	virtual void Execute() override;
};
